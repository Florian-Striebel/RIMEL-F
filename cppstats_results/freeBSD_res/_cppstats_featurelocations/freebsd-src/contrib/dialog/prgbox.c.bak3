






















#include <dialog.h>

static void
reapchild(int sig)
{
(void) sig;
}




FILE *
dlg_popen(const char *command, const char *type)
{
FILE *result = 0;
int fd[2];

if ((*type == 'r' || *type == 'w') && pipe(fd) == 0) {
char *blob;

switch (fork()) {
case -1:
(void) close(fd[0]);
(void) close(fd[1]);
break;
case 0:
if (*type == 'r') {
if (fd[1] != STDOUT_FILENO) {
(void) dup2(fd[1], STDOUT_FILENO);
(void) close(fd[1]);
}
(void) dup2(STDOUT_FILENO, STDERR_FILENO);
(void) close(fd[0]);
} else {
if (fd[0] != STDIN_FILENO) {
(void) dup2(fd[0], STDIN_FILENO);
(void) close(fd[0]);
}
(void) close(fd[1]);
(void) close(STDERR_FILENO);
}





if ((blob = malloc(10 + strlen(command))) != 0) {
char **argv;
sprintf(blob, "sh -c \"%s\"", command);
argv = dlg_string_to_argv(blob);
execvp("sh", argv);
}
_exit(127);

default:
if (*type == 'r') {
result = fdopen(fd[0], type);
(void) close(fd[1]);
} else {
result = fdopen(fd[1], type);
(void) close(fd[0]);
}
break;
}
}

return result;
}




int
dialog_prgbox(const char *title,
const char *cprompt,
const char *command,
int height,
int width,
int pauseopt)
{
int code;
FILE *fp;
void (*oldreaper) (int) = signal(SIGCHLD, reapchild);

fp = dlg_popen(command, "r");
if (fp == NULL)
dlg_exiterr("pipe open failed: %s", command);

code = dlg_progressbox(title, cprompt, height, width, pauseopt, fp);

pclose(fp);
signal(SIGCHLD, oldreaper);

return code;
}
