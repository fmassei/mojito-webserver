#include <mjt_types.h>
#include "fparams.h"

#define INIFILE "config.ini"

struct fparam_s *g_params;

void parse_options(const int_t argc, char_t * const argv[])
{
    int_t opt;
    while ((opt = getopt(argc, argv, "v"))!=-1) {
        switch(opt) {
        case 'v':
            printf("%s\n", PACKAGE_STRING);
            exit(EXIT_SUCCESS);
            break;
        default:
            fprintf(stderr, "Usage %s [-v]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

void clean_quit(void)
{
    mjt_logger_fini();
}

void sig_term(int_t signal)
{
    mjt_logmsg(LOG_INFO, "caught SIGTERM(%d). Stopping.", signal);
    clean_quit();
    exit(EXIT_SUCCESS);
}

int_t main (const int_t argc, char_t * const argv[])
{
    parse_options(argc, argv);
    if ((g_params = params_loadFromINIFile(INIFILE))==NULL) {
        mjt_logmsg(LOG_CRIT, "Error parsing config file, exiting");
        exit(EXIT_FAILURE);
    }
    if (fork_to_background(g_params->pidfile, g_params->uid, g_params->gid,
                                            g_params->http_root, sig_term)!=0) {
        mjt_logmsg(LOG_CRIT, "Could not fork to background.");
        exit(EXIT_FAILURE);
    }
    if (mjt_logger_init(g_params->logfile, g_params->errfile)!=0) {
        mjt_logmsg(LOG_CRIT, "Could not start logging service.");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

