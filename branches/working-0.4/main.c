#include <disml/disml.h>
#include <mmp/mmp_trace.h>
#include "config_manager.h"
#include "defaults.h"

int main(/*const int argc, const char *argv[]*/)
{
    fprintf(stdout, "starting\n");
    if (config_manager_loadfile(DEFAULT_CONFIGFILE)!=MMP_ERR_OK) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    printf("ok\n");
    config_manager_freeall();
    return EXIT_SUCCESS;
}

