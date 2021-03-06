























#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>




#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#include "brssl.h"

static void
usage(void)
{
fprintf(stderr, "usage: brssl command [ options ]\n");
fprintf(stderr, "available commands:\n");
fprintf(stderr, " client run SSL client\n");
fprintf(stderr, " server run SSL server\n");
fprintf(stderr, " verify verify certificate chain\n");
fprintf(stderr, " skey decode private key\n");
fprintf(stderr, " ta decode trust anchors\n");
fprintf(stderr, " chain make C code for certificate chains\n");
fprintf(stderr, " twrch run the Twrch protocol\n");
fprintf(stderr, " impl report on implementations\n");
}

int
main(int argc, char *argv[])
{
char *cmd;

if (argc < 2) {
usage();
return EXIT_FAILURE;
}
#if defined(_WIN32)
{
WSADATA wd;
int r;

r = WSAStartup(MAKEWORD(2, 2), &wd);
if (r != 0) {
fprintf(stderr, "WARNING: network initialisation"
" failed (WSAStartup() returned %d)\n", r);
}
}
#endif
cmd = argv[1];
if (eqstr(cmd, "client")) {
if (do_client(argc - 2, argv + 2) < 0) {
return EXIT_FAILURE;
}
} else if (eqstr(cmd, "server")) {
if (do_server(argc - 2, argv + 2) < 0) {
return EXIT_FAILURE;
}
} else if (eqstr(cmd, "verify")) {
if (do_verify(argc - 2, argv + 2) < 0) {
return EXIT_FAILURE;
}
} else if (eqstr(cmd, "skey")) {
if (do_skey(argc - 2, argv + 2) < 0) {
return EXIT_FAILURE;
}
} else if (eqstr(cmd, "ta")) {
if (do_ta(argc - 2, argv + 2) < 0) {
return EXIT_FAILURE;
}
} else if (eqstr(cmd, "chain")) {
if (do_chain(argc - 2, argv + 2) < 0) {
return EXIT_FAILURE;
}
} else if (eqstr(cmd, "twrch")) {
int ret;

ret = do_twrch(argc - 2, argv + 2);
if (ret < 0) {
return EXIT_FAILURE;
} else {
return ret;
}
} else if (eqstr(cmd, "impl")) {
if (do_impl(argc - 2, argv + 2) < 0) {
return EXIT_FAILURE;
}
} else {
fprintf(stderr, "unknown command: '%s'\n", cmd);
usage();
return EXIT_FAILURE;
}
return 0;
}
