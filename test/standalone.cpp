#include <cinttypes>
#include <err.h>
#include <perfmon/pfmlib.h>
#include <cstring>
#include <cstdlib>

int main(int argc, char **argv) {
    pfm_pmu_encode_arg_t arg;
    int ret;

    ret = pfm_initialize();
    if (ret != PFM_SUCCESS)
        errx(1, "cannot initialize library %s", pfm_strerror(ret));

    memset(&arg, 0, sizeof(arg));

    ret = pfm_get_os_event_encoding("RETIRED_INSTRUCTIONS:k", PFM_PLM3, PFM_OS_NONE, &arg);
    if (ret != PFM_SUCCESS)
        printf("cannot get encoding %s", pfm_strerror(ret));

    for (int i = 0; i < arg.count; i++)
        printf("count[%d]=0x%" PRIx64"\n", i, arg.codes[i]);

    free(arg.codes);
    return 0;
}
