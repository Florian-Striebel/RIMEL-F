









#define END_OPTION_STRING ('$')




#define BOOL 01
#define TRIPLE 02
#define NUMBER 04
#define STRING 010
#define NOVAR 020
#define REPAINT 040
#define NO_TOGGLE 0100
#define HL_REPAINT 0200
#define NO_QUERY 0400
#define INIT_HANDLER 01000

#define OTYPE (BOOL|TRIPLE|NUMBER|STRING|NOVAR)

#define OLETTER_NONE '\1'




#define INIT 0
#define QUERY 1
#define TOGGLE 2


#define OPT_NO_TOGGLE 0
#define OPT_TOGGLE 1
#define OPT_UNSET 2
#define OPT_SET 3
#define OPT_NO_PROMPT 0100


#define OPT_AMBIG 1

struct optname
{
char *oname;
struct optname *onext;
};

#define OPTNAME_MAX 32

struct loption
{
char oletter;
struct optname *onames;
int otype;
int odefault;
int *ovar;
void (*ofunc) LESSPARAMS ((int, char*));
char *odesc[3];
};

