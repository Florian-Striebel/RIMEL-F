






















#include <stdlib.h>

extern int yylex (void);

int main (int argc, char *argv[])
{
(void)argc;
(void)argv;

while (yylex () != 0) ;

exit(0);
}
