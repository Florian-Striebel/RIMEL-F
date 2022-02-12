

































struct element_type {
const char *et_name;
int et_type;
};

struct changer_command {
const char *cc_name;

int (*cc_handler)(const char *, int, char **);
};

struct special_word {
const char *sw_name;
int sw_value;
};


#define SW_INVERT 1
#define SW_INVERT1 2
#define SW_INVERT2 3


#define CHANGER_ENV_VAR "CHANGER"
