#include <cinttypes>
#include <err.h>
#include <perfmon/pfmlib.h>
#include <cstring>
#include <cstdlib>
#include <perfmon/pfmlib_perf_event.h>

int main(int argc, char **argv) {
    int ret;

    ret = pfm_initialize();
    if (ret != PFM_SUCCESS)
        errx(1, "cannot initialize library %s", pfm_strerror(ret));

    pfm_perf_encode_arg_t arg;
    memset(&arg, 0, sizeof(arg));
    perf_event_attr attr{0};
    arg.attr = &attr;
    arg.size = sizeof(pfm_perf_encode_arg_t);

    ret = pfm_get_os_event_encoding("cs", PFM_PLM3, PFM_OS_PERF_EVENT, &arg);
    if (ret != PFM_SUCCESS)
        printf("cannot get encoding %s", pfm_strerror(ret));

    return 0;
}
