#include "mjt_httprequest.h"

static const char_t *method_strs[] = { "", "GET", "HEAD", "POST" };

const char_t *mjt_get_http_method_name(int_t http_method)
{
    if (http_method<HTTP_METHOD_GET || http_method>HTTP_METHOD_POST)
        return NULL;
    return method_strs[http_method];
}

/* create an empty request */
struct mjt_request_s *mjt_request_create()
{
    struct mjt_request_s *ret;
    if ((ret = mjt_malloc(sizeof(*ret)))==NULL)
        return NULL;
    ret->in_ip = ret->URI = NULL;
    ret->http_method = 0;
    ret->content_length = 0;
    ret->content_type = ret->page = ret->query_string = NULL;
    ret->accept_encoding = NULL;
    ret->post_fd = 0;
    ret->abs_filename = NULL;
    ret->sb = NULL;
    return ret;
}

void mjt_request_destroy(struct mjt_request_s **req)
{
    if (req==NULL || *req==NULL)
        return;
    mjt_free(&((*req)->in_ip));
    mjt_free(&((*req)->URI));
    mjt_free(&((*req)->content_type));
    mjt_free(&((*req)->abs_filename));
    mjt_free(&((*req)->page));
    mjt_free(&((*req)->query_string));
    mjt_qhead_free(&((*req)->accept_encoding));
    mjt_free2null(req);
}

/* convert to uppercase and change '-'s into '_'s */
static char_t *smart_upper(char_t *str)
{
    char_t *st = str;
    while(*str) {
        if (*str=='-') *str = '_';
        else *str = toupper(*str);
        ++str;
    }
    return st;
}

static int_t parse_first_line(struct mjt_request_s *req, char_t *line)
{
    char_t *st, *st2;
    if (!memcmp(line, "GET ", 4))
        req->http_method = HTTP_METHOD_GET;
    else if (!memcmp(line, "HEAD ", 5))
        req->http_method = HTTP_METHOD_HEAD;
    else if (!memcmp(line, "POST ", 5))
        req->http_method = HTTP_METHOD_POST;
    else
        return -1;
    /* go to next arg. Here and below we skip more than one whitespace if
     * present between arguments. This is not RFC2616 conforming, but adds
     * some robustness to the client/server communication. */
    st = line + 3;
    if (*st!=' ') ++st;
    while (*(++st)==' ' && *st!='\0');
    /* find end or space */
    for (st2=st; *st2!='\0' && *st2!=' '; ++st2);
    if ((req->URI = mjt_malloc(st2-st+1))==NULL)
        return -1;
    memcpy(req->URI, st, st2-st);
    req->URI[st2-st] = '\0';
    /* find protocol version */
    st = st2;
    while(*(++st)==' ' && *st!='\0');
    /* again, we skip whitespaces at the end, breaking the specs. */
    for (st2=st; *st2!='\0' && *st2!=' '; ++st2);
    req->proto_version = HTTP_VER_UNKNOWN;
    if ((st2-st)>=8) {
        if (!memcmp(st, "HTTP/1.1", 8)) {
            req->proto_version = HTTP_VER_11;
            req->can_keepalive = TRUE; /* by default (RFC2616-8.1.2) */
        } else if (!memcmp(st, "HTTP/1.0", 8)) {
            req->proto_version = HTTP_VER_10;
        }
    }
    return 0;
}

/* parse a generic header line */
static int_t parse_header_line(struct mjt_request_s *req, char_t *line)
{
    char_t *value, c, *endptr;
    if ((value = strchr(line, ':'))==NULL)
        return 0;
    *value++ = '\0';
    while((c=*value)&&(c==' ' || c=='\t')) ++value;
    smart_upper(line);
    if (!memcmp(line, "CONTENT_LENGTH", 15) && req->content_length==0) {
        req->content_length = strtol(value, &endptr, 10);
        if ((errno==ERANGE
                && (req->content_length==LONG_MAX ||
                    req->content_length==LONG_MIN))
                || (errno!=0 && value==0) || (endptr==value)) {
            req->content_length = 0;
            return 0;
        }
    } else if (!memcmp(line, "ACCEPT_ENCODING", 15) &&
            req->accept_encoding==NULL) {
        DEBUG_LOG((LOG_DEBUG, "parsing acceptable encoding: %s", value));
        req->accept_encoding = mjt_qhead_parse(value);
    } else if (!memcmp(line, "CONTENT_TYPE", 12) && req->content_type==NULL) {
        if ((req->content_type = strdup(value))==NULL)
            return -1;
    } else if (!memcmp(line, "CONNECTION", 10)) {
        if ((req->proto_version==HTTP_VER_10) &&
                (!strncasecmp(value, "keep-alive", strlen(value))) )
            req->can_keepalive = TRUE;
        else if ( (req->proto_version==HTTP_VER_11) &&
                (!strncasecmp(value, "close", strlen(value))) )
            req->can_keepalive = FALSE;
    }
    return 0;
}

/* parse an option */
static int_t parse_option(struct mjt_request_s *req, char_t *line)
{
    if (req->http_method==0)
        return parse_first_line(req, line);
    else
        return parse_header_line(req, line);
}

/* ok - let's the kernel do its job :-) For post-data we create a temporary 
 * file to pass to cgi apps. We rely on two very important features: buffering
 * and delayed unlink. As long as I know every unix-like kernel behave the
 * same way in our conditions. */
static int_t create_post_file(char_t *tmpdir)
{
    static char_t tmp_file[2049];
    int_t fd;
    snprintf(tmp_file, 2049, "%s/mojito-temp.XXXXXX", tmpdir);
    if ((fd = mkstemp(tmp_file))==-1) {
        perror("mkstemp");
        return -1;
    }
    if (unlink(tmp_file)==-1) {
        close(fd);
        perror("unlink");
        return 0; /* FIXME: Right now we go on without post-data. Maybe it's not
                    * the safer choice. Think about just returning 501. */
    }
    return fd;
}

/* parser constants */
#define PARSE_HEAD  1
#define PARSE_CR1   2
#define PARSE_LF1   3
#define PARSE_CR2   4
#define PARSE_BODY  5
/* cycle the socket data, reading a socket-buffer-size bytes a time, until we 
 * got everything */
int_t mjt_request_read(int_t sock, struct mjt_request_s *req)
{
    int_t reline;
    int_t rb, mr, len, pos;
    int_t sockbufsize;
    int_t state;
    char_t c, *buf, *cur_head, *post_data;
    sockbufsize = mjt_socket_getbuffersize();
    len = pos = 0;
    buf = cur_head = post_data = NULL;
    state = PARSE_HEAD;
    while(1) {
        mr = sockbufsize+pos;
        if (len<mr) {
            reline = (int_t)cur_head - (int_t)buf;
            if ((buf = mjt_realloc(buf, mr))==NULL) {
                mjt_free(&buf);
                return REQUEST_PARSE_ERROR;
            }
            cur_head = (char_t*)((int_t)buf + reline);
            len = mr;
        }
        mr = read(sock, buf+pos, sockbufsize);
        if (rb<=0)
            break;
        /* BOA-like loop. An insane mess, but pretty clear after two or three
         * hours spent on reading this twenty lines. Good luck. */
        while (rb-->0) {
            c = *(buf+pos);
            if (c=='\0') continue;
            switch(state) {
            case PARSE_HEAD:
                if (c=='\r') state = PARSE_CR1;
                else if (c=='\n') state = PARSE_LF1;
                break;
            case PARSE_CR1:
                if (c=='\n') state = PARSE_LF1;
                else if (c=='\r') state = PARSE_HEAD;
                break;
            case PARSE_LF1:
                if (c=='\r') state = PARSE_CR2;
                else if (c=='\n') state = PARSE_BODY;
                else state = PARSE_HEAD;
            case PARSE_CR2:
                if (c=='\n') state = PARSE_BODY;
                else if (c=='\r') state = PARSE_HEAD;
                break;
            }
            ++pos;
            if (state==PARSE_LF1) {
                /* header found */
                buf[pos-2] = '\0';
                if (parse_option(req, cur_head)!=0) {
                    mjt_free(&buf);
                    return REQUEST_PARSE_ERROR;
                }
                cur_head = buf+pos;
            } else if (state==PARSE_BODY) {
                /* sanitize accept_encoding */
                /* TODO */
                /* just check if the client has done */
                if (req->content_length<=0 ||
                        req->http_method!=HTTP_METHOD_POST)
                    return REQUEST_PARSE_OK;
                /* get the post-data */
                post_data = buf+pos;
                if (req->content_length > rb) {
                    /* if here we have to realloc the buffer and get the 
                     * missing data stored in the socket buffer */
                    buf = mjt_realloc(buf, mr-rb+req->content_length);
                    if (buf==NULL) {
                        mjt_free(&buf);
                        return REQUEST_PARSE_ERROR;
                    }
                    do {
                        len = req->content_length - rb;
                        rb = read(sock, buf+pos, len);
                        len -= rb;
                        pos += rb;
                    } while (len>0);
                }
                /* write to file */
                /* FIXME: right now we don't take the config value! */
                if ((req->post_fd = create_post_file("/tmp"))==-1) {
                    mjt_free(&buf);
                    return REQUEST_PARSE_ERROR;
                }
                if (mjt_justwrite(req->post_fd, post_data,
                                                req->content_length)==FALSE) {
                    mjt_free(&buf);
                    return REQUEST_PARSE_ERROR;
                }
                mjt_free(&buf);
                return REQUEST_PARSE_OK;
            }
        }
    }
    mjt_free(&buf);
    return REQUEST_PARSE_INCOMPLETE;
}

