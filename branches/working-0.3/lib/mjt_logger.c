#include "mjt_logger.h"

/* FIXME: not thread safe! */
static char_t datestr[30];
static FILE *flog = NULL, *ferr = NULL;

static char_t *get_logprio_desc(int_t prio)
{
    switch(prio) {
    case LOG_EMERG: return "emerg";
    case LOG_ALERT: return "alert";
    case LOG_CRIT: return "crit";
    case LOG_ERR: return "err";
    case LOG_WARNING: return "warning";
    case LOG_NOTICE: return "notice";
    case LOG_INFO: return "info";
    case LOG_DEBUG: return "debug";
    default: return "";
    }
}

static char_t *outdate()
{
    timet_t t;
    t = time(NULL);
    strftime(datestr, sizeof(datestr), "%a %b %d %H:%M:%S %Y", localtime(&t));
    return datestr;
}

static void _f_logmsg(int_t prio, char_t *fmt, va_list argp)
{
    FILE *f;
    f = (ferr==NULL) ? stderr : ferr;
    fprintf(f, "[%s] [%s %d] ", outdate(), get_logprio_desc(prio), getpid());
    vfprintf(f, fmt, argp);
    fprintf(f, "\n");
}

void mjt_logmsg(int_t prio, char_t *fmt, ...)
{
    va_list ap;
    VA_START(ap, fmt);
    _f_logmsg(prio, fmt, ap);
    va_end(ap);
}

int_t mjt_logger_init(char_t *logfile, char_t *errfile)
{
    if ((flog = freopen(logfile, "w+", stdout))==NULL) {
        fprintf(stderr, "Logger: Error opening file %s\n", logfile);
        return -1;
    }
    if ((ferr = freopen(errfile, "w+", stderr))==NULL) {
        fprintf(stderr, "Logger: Error opening file %s\n", errfile);
        fclose(flog);
        return -1;
    }
    return 0;
}

int_t mjt_logger_fini(void)
{
    int_t ret = 0;
    if (flog) ret |= fclose(flog);
    if (ferr) ret |= fclose(ferr);
    return !(ret==0);
}

void mjt_logger_flush(void)
{
    if (flog) fflush(flog);
    if (ferr) fflush(ferr);
}

