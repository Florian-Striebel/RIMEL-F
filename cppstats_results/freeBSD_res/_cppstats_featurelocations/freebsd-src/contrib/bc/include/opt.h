






































#if !defined(BC_OPT_H)
#define BC_OPT_H

#include <stdbool.h>
#include <stdlib.h>


typedef struct BcOpt {


char **argv;


size_t optind;


int optopt;


int subopt;


char *optarg;

} BcOpt;


typedef enum BcOptType {


BC_OPT_NONE,


BC_OPT_REQUIRED,


BC_OPT_BC_ONLY,


BC_OPT_REQUIRED_BC_ONLY,


BC_OPT_DC_ONLY,

} BcOptType;


typedef struct BcOptLong {


const char *name;


BcOptType type;


int val;

} BcOptLong;






void bc_opt_init(BcOpt *o, char **argv);








int bc_opt_parse(BcOpt *o, const BcOptLong *longopts);






#define BC_OPT_ISDASHDASH(a) ((a) != NULL && (a)[0] == '-' && (a)[1] == '-' && (a)[2] == '\0')







#define BC_OPT_ISSHORTOPT(a) ((a) != NULL && (a)[0] == '-' && (a)[1] != '-' && (a)[1] != '\0')







#define BC_OPT_ISLONGOPT(a) ((a) != NULL && (a)[0] == '-' && (a)[1] == '-' && (a)[2] != '\0')


#endif
