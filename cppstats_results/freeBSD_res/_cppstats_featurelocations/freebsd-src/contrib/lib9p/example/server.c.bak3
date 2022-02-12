


























#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include "../lib9p.h"
#include "../backend/fs.h"
#include "../transport/socket.h"

int
main(int argc, char **argv)
{
struct l9p_backend *fs_backend;
struct l9p_server *server;
char *host = "0.0.0.0";
char *port = "564";
char *path;
bool ro = false;
int rootfd;
int opt;

while ((opt = getopt(argc, argv, "h:p:r")) != -1) {
switch (opt) {
case 'h':
host = optarg;
break;
case 'p':
port = optarg;
break;
case 'r':
ro = true;
break;
case '?':
default:
goto usage;
}
}

if (optind >= argc) {
usage:
errx(1, "Usage: server [-h <host>] [-p <port>] [-r] <path>");
}

path = argv[optind];
rootfd = open(path, O_DIRECTORY);

if (rootfd < 0)
err(1, "cannot open root directory");

if (l9p_backend_fs_init(&fs_backend, rootfd, ro) != 0)
err(1, "cannot init backend");

if (l9p_server_init(&server, fs_backend) != 0)
err(1, "cannot create server");

server->ls_max_version = L9P_2000L;
if (l9p_start_server(server, host, port))
err(1, "l9p_start_server() failed");


exit(0);
}
