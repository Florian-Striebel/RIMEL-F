































#if !defined(snmp_agent_h_)
#define snmp_agent_h_

struct snmp_dependency;

enum snmp_ret {

SNMP_RET_OK = 0,

SNMP_RET_IGN = 1,

SNMP_RET_ERR = 2
};


struct snmp_context {
u_int var_index;
struct snmp_scratch *scratch;
struct snmp_dependency *dep;
void *data;
enum snmp_ret code;
};

struct snmp_scratch {
void *ptr1;
void *ptr2;
uint32_t int1;
uint32_t int2;
};

enum snmp_depop {
SNMP_DEPOP_COMMIT,
SNMP_DEPOP_ROLLBACK,
SNMP_DEPOP_FINISH
};

typedef int (*snmp_depop_t)(struct snmp_context *, struct snmp_dependency *,
enum snmp_depop);

struct snmp_dependency {
struct asn_oid obj;
struct asn_oid idx;
};




enum snmp_node_type {
SNMP_NODE_LEAF = 1,
SNMP_NODE_COLUMN
};

enum snmp_op {
SNMP_OP_GET = 1,
SNMP_OP_GETNEXT,
SNMP_OP_SET,
SNMP_OP_COMMIT,
SNMP_OP_ROLLBACK,
};

typedef int (*snmp_op_t)(struct snmp_context *, struct snmp_value *,
u_int, u_int, enum snmp_op);

struct snmp_node {
struct asn_oid oid;
const char *name;
enum snmp_node_type type;
enum snmp_syntax syntax;
snmp_op_t op;
u_int flags;
uint32_t index;
void *data;
void *tree_data;
};
extern struct snmp_node *tree;
extern u_int tree_size;

#define SNMP_NODE_CANSET 0x0001

#define SNMP_INDEXES_MAX 7
#define SNMP_INDEX_SHIFT 4
#define SNMP_INDEX_MASK 0xf
#define SNMP_INDEX_COUNT(V) ((V) & SNMP_INDEX_MASK)
#define SNMP_INDEX(V,I) (((V) >> (((I) + 1) * SNMP_INDEX_SHIFT)) & SNMP_INDEX_MASK)


enum {
SNMP_TRACE_GET = 0x00000001,
SNMP_TRACE_GETNEXT = 0x00000002,
SNMP_TRACE_SET = 0x00000004,
SNMP_TRACE_DEPEND = 0x00000008,
SNMP_TRACE_FIND = 0x00000010,
};

extern u_int snmp_trace;


extern void (*snmp_debug)(const char *fmt, ...);

enum snmp_ret snmp_get(struct snmp_pdu *pdu, struct asn_buf *resp_b,
struct snmp_pdu *resp, void *);
enum snmp_ret snmp_getnext(struct snmp_pdu *pdu, struct asn_buf *resp_b,
struct snmp_pdu *resp, void *);
enum snmp_ret snmp_getbulk(struct snmp_pdu *pdu, struct asn_buf *resp_b,
struct snmp_pdu *resp, void *);
enum snmp_ret snmp_set(struct snmp_pdu *pdu, struct asn_buf *resp_b,
struct snmp_pdu *resp, void *);

enum snmp_ret snmp_make_errresp(const struct snmp_pdu *, struct asn_buf *,
struct asn_buf *);

struct snmp_dependency *snmp_dep_lookup(struct snmp_context *,
const struct asn_oid *, const struct asn_oid *, size_t, snmp_depop_t);

struct snmp_context *snmp_init_context(void);
int snmp_dep_commit(struct snmp_context *);
int snmp_dep_rollback(struct snmp_context *);
void snmp_dep_finish(struct snmp_context *);

#endif
