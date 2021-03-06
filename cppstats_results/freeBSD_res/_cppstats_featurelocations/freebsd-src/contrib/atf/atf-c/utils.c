
























#include "atf-c/utils.h"

#include <sys/stat.h>
#include <sys/wait.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <atf-c.h>

#include "atf-c/detail/dynstr.h"


void atf_tc_set_resultsfile(const char *);










static void
init_out_filename(atf_dynstr_t *name, const pid_t pid, const char *suffix,
const bool in_parent)
{
atf_error_t error;

error = atf_dynstr_init_fmt(name, "atf_utils_fork_%d_%s.txt",
(int)pid, suffix);
if (atf_is_error(error)) {
char buffer[1024];
atf_error_format(error, buffer, sizeof(buffer));
if (in_parent) {
atf_tc_fail("Failed to create output file: %s", buffer);
} else {
err(EXIT_FAILURE, "Failed to create output file: %s", buffer);
}
}
}







static
bool
grep_string(const char *regex, const char *str)
{
int res;
regex_t preg;

printf("Looking for '%s' in '%s'\n", regex, str);
ATF_REQUIRE(regcomp(&preg, regex, REG_EXTENDED) == 0);

res = regexec(&preg, str, 0, NULL, 0);
ATF_REQUIRE(res == 0 || res == REG_NOMATCH);

regfree(&preg);

return res == 0;
}






void
atf_utils_cat_file(const char *name, const char *prefix)
{
const int fd = open(name, O_RDONLY);
ATF_REQUIRE_MSG(fd != -1, "Cannot open %s", name);

char buffer[1024];
ssize_t count;
bool continued = false;
while ((count = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
buffer[count] = '\0';

if (!continued)
printf("%s", prefix);

char *iter = buffer;
char *end;
while ((end = strchr(iter, '\n')) != NULL) {
*end = '\0';
printf("%s\n", iter);

iter = end + 1;
if (iter != buffer + count)
printf("%s", prefix);
else
continued = false;
}
if (iter < buffer + count) {
printf("%s", iter);
continued = true;
}
}
ATF_REQUIRE(count == 0);
}







bool
atf_utils_compare_file(const char *name, const char *contents)
{
const int fd = open(name, O_RDONLY);
ATF_REQUIRE_MSG(fd != -1, "Cannot open %s", name);

const char *pos = contents;
ssize_t remaining = strlen(contents);

char buffer[1024];
ssize_t count;
while ((count = read(fd, buffer, sizeof(buffer))) > 0 &&
count <= remaining) {
if (memcmp(pos, buffer, count) != 0) {
close(fd);
return false;
}
remaining -= count;
pos += count;
}
close(fd);
return count == 0 && remaining == 0;
}





void
atf_utils_copy_file(const char *source, const char *destination)
{
const int input = open(source, O_RDONLY);
ATF_REQUIRE_MSG(input != -1, "Failed to open source file during "
"copy (%s)", source);

const int output = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0777);
ATF_REQUIRE_MSG(output != -1, "Failed to open destination file during "
"copy (%s)", destination);

char buffer[1024];
ssize_t length;
while ((length = read(input, buffer, sizeof(buffer))) > 0)
ATF_REQUIRE_MSG(write(output, buffer, length) == length,
"Failed to write to %s during copy", destination);
ATF_REQUIRE_MSG(length != -1, "Failed to read from %s during copy", source);

struct stat sb;
ATF_REQUIRE_MSG(fstat(input, &sb) != -1,
"Failed to stat source file %s during copy", source);
ATF_REQUIRE_MSG(fchmod(output, sb.st_mode) != -1,
"Failed to chmod destination file %s during copy",
destination);

close(output);
close(input);
}






void
atf_utils_create_file(const char *name, const char *contents, ...)
{
va_list ap;
atf_dynstr_t formatted;
atf_error_t error;

va_start(ap, contents);
error = atf_dynstr_init_ap(&formatted, contents, ap);
va_end(ap);
ATF_REQUIRE(!atf_is_error(error));

const int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
ATF_REQUIRE_MSG(fd != -1, "Cannot create file %s", name);
ATF_REQUIRE(write(fd, atf_dynstr_cstring(&formatted),
atf_dynstr_length(&formatted)) != -1);
close(fd);

atf_dynstr_fini(&formatted);
}






bool
atf_utils_file_exists(const char *path)
{
const int ret = access(path, F_OK);
if (ret == -1) {
if (errno != ENOENT)
atf_tc_fail("Failed to check the existence of %s: %s", path,
strerror(errno));
else
return false;
} else
return true;
}








pid_t
atf_utils_fork(void)
{
const pid_t pid = fork();
if (pid == -1)
atf_tc_fail("fork failed");

if (pid == 0) {
atf_dynstr_t out_name;
init_out_filename(&out_name, getpid(), "out", false);

atf_dynstr_t err_name;
init_out_filename(&err_name, getpid(), "err", false);

atf_utils_redirect(STDOUT_FILENO, atf_dynstr_cstring(&out_name));
atf_utils_redirect(STDERR_FILENO, atf_dynstr_cstring(&err_name));

atf_dynstr_fini(&err_name);
atf_dynstr_fini(&out_name);
}
return pid;
}

void
atf_utils_reset_resultsfile(void)
{

atf_tc_set_resultsfile("/dev/null");
}





void
atf_utils_free_charpp(char **argv)
{
char **ptr;

for (ptr = argv; *ptr != NULL; ptr++)
free(*ptr);

free(argv);
}








bool
atf_utils_grep_file(const char *regex, const char *file, ...)
{
int fd;
va_list ap;
atf_dynstr_t formatted;
atf_error_t error;

va_start(ap, file);
error = atf_dynstr_init_ap(&formatted, regex, ap);
va_end(ap);
ATF_REQUIRE(!atf_is_error(error));

ATF_REQUIRE((fd = open(file, O_RDONLY)) != -1);
bool found = false;
char *line = NULL;
while (!found && (line = atf_utils_readline(fd)) != NULL) {
found = grep_string(atf_dynstr_cstring(&formatted), line);
free(line);
}
close(fd);

atf_dynstr_fini(&formatted);

return found;
}








bool
atf_utils_grep_string(const char *regex, const char *str, ...)
{
bool res;
va_list ap;
atf_dynstr_t formatted;
atf_error_t error;

va_start(ap, str);
error = atf_dynstr_init_ap(&formatted, regex, ap);
va_end(ap);
ATF_REQUIRE(!atf_is_error(error));

res = grep_string(atf_dynstr_cstring(&formatted), str);

atf_dynstr_fini(&formatted);

return res;
}







char *
atf_utils_readline(const int fd)
{
char ch;
ssize_t cnt;
atf_dynstr_t temp;
atf_error_t error;

error = atf_dynstr_init(&temp);
ATF_REQUIRE(!atf_is_error(error));

while ((cnt = read(fd, &ch, sizeof(ch))) == sizeof(ch) &&
ch != '\n') {
error = atf_dynstr_append_fmt(&temp, "%c", ch);
ATF_REQUIRE(!atf_is_error(error));
}
ATF_REQUIRE(cnt != -1);

if (cnt == 0 && atf_dynstr_length(&temp) == 0) {
atf_dynstr_fini(&temp);
return NULL;
} else
return atf_dynstr_fini_disown(&temp);
}









void
atf_utils_redirect(const int target_fd, const char *name)
{
if (target_fd == STDOUT_FILENO)
fflush(stdout);
else if (target_fd == STDERR_FILENO)
fflush(stderr);

const int new_fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
if (new_fd == -1)
err(EXIT_FAILURE, "Cannot create %s", name);
if (new_fd != target_fd) {
if (dup2(new_fd, target_fd) == -1)
err(EXIT_FAILURE, "Cannot redirect to fd %d", target_fd);
}
close(new_fd);
}








void
atf_utils_wait(const pid_t pid, const int exitstatus, const char *expout,
const char *experr)
{
int status;
ATF_REQUIRE(waitpid(pid, &status, 0) != -1);

atf_dynstr_t out_name;
init_out_filename(&out_name, pid, "out", true);

atf_dynstr_t err_name;
init_out_filename(&err_name, pid, "err", true);

atf_utils_cat_file(atf_dynstr_cstring(&out_name), "subprocess stdout: ");
atf_utils_cat_file(atf_dynstr_cstring(&err_name), "subprocess stderr: ");

ATF_REQUIRE(WIFEXITED(status));
ATF_REQUIRE_EQ(exitstatus, WEXITSTATUS(status));

const char *save_prefix = "save:";
const size_t save_prefix_length = strlen(save_prefix);

if (strlen(expout) > save_prefix_length &&
strncmp(expout, save_prefix, save_prefix_length) == 0) {
atf_utils_copy_file(atf_dynstr_cstring(&out_name),
expout + save_prefix_length);
} else {
ATF_REQUIRE(atf_utils_compare_file(atf_dynstr_cstring(&out_name),
expout));
}

if (strlen(experr) > save_prefix_length &&
strncmp(experr, save_prefix, save_prefix_length) == 0) {
atf_utils_copy_file(atf_dynstr_cstring(&err_name),
experr + save_prefix_length);
} else {
ATF_REQUIRE(atf_utils_compare_file(atf_dynstr_cstring(&err_name),
experr));
}

ATF_REQUIRE(unlink(atf_dynstr_cstring(&out_name)) != -1);
ATF_REQUIRE(unlink(atf_dynstr_cstring(&err_name)) != -1);
}
