
























#include "atf-c/detail/test_helpers.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <atf-c.h>

#include "atf-c/build.h"
#include "atf-c/check.h"
#include "atf-c/detail/dynstr.h"
#include "atf-c/detail/env.h"
#include "atf-c/detail/fs.h"
#include "atf-c/detail/process.h"
#include "atf-c/error.h"

bool
build_check_c_o(const char *path)
{
bool success;
atf_dynstr_t iflag;
const char *optargs[4];

RE(atf_dynstr_init_fmt(&iflag, "-I%s", atf_env_get_with_default(
"ATF_INCLUDEDIR", ATF_INCLUDEDIR)));

optargs[0] = atf_dynstr_cstring(&iflag);
optargs[1] = "-Wall";
optargs[2] = "-Werror";
optargs[3] = NULL;

RE(atf_check_build_c_o(path, "test.o", optargs, &success));

atf_dynstr_fini(&iflag);

return success;
}

bool
build_check_c_o_srcdir(const atf_tc_t *tc, const char *sfile)
{
atf_fs_path_t path;

RE(atf_fs_path_init_fmt(&path, "%s/%s",
atf_tc_get_config_var(tc, "srcdir"), sfile));
const bool result = build_check_c_o(atf_fs_path_cstring(&path));
atf_fs_path_fini(&path);
return result;
}

void
header_check(const char *hdrname)
{
FILE *srcfile;
char failmsg[128];

srcfile = fopen("test.c", "w");
ATF_REQUIRE(srcfile != NULL);
fprintf(srcfile, "#include <%s>\n", hdrname);
fclose(srcfile);

snprintf(failmsg, sizeof(failmsg),
"Header check failed; %s is not self-contained", hdrname);

if (!build_check_c_o("test.c"))
atf_tc_fail("%s", failmsg);
}

void
get_process_helpers_path(const atf_tc_t *tc, const bool is_detail,
atf_fs_path_t *path)
{
RE(atf_fs_path_init_fmt(path, "%s/%sprocess_helpers",
atf_tc_get_config_var(tc, "srcdir"),
is_detail ? "" : "detail/"));
}

struct run_h_tc_data {
atf_tc_t *m_tc;
const char *m_resname;
};

static
void
run_h_tc_child(void *v)
{
struct run_h_tc_data *data = (struct run_h_tc_data *)v;

RE(atf_tc_run(data->m_tc, data->m_resname));
}




void
run_h_tc(atf_tc_t *tc, const char *outname, const char *errname,
const char *resname)
{
atf_fs_path_t outpath, errpath;
atf_process_stream_t outb, errb;
atf_process_child_t child;
atf_process_status_t status;

RE(atf_fs_path_init_fmt(&outpath, outname));
RE(atf_fs_path_init_fmt(&errpath, errname));

struct run_h_tc_data data = { tc, resname };

RE(atf_process_stream_init_redirect_path(&outb, &outpath));
RE(atf_process_stream_init_redirect_path(&errb, &errpath));
RE(atf_process_fork(&child, run_h_tc_child, &outb, &errb, &data));
atf_process_stream_fini(&errb);
atf_process_stream_fini(&outb);

RE(atf_process_child_wait(&child, &status));
ATF_CHECK(atf_process_status_exited(&status));
atf_process_status_fini(&status);

atf_fs_path_fini(&errpath);
atf_fs_path_fini(&outpath);
}
