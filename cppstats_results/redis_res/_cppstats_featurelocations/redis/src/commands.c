

#include "server.h"








commandHistory BITCOUNT_History[] = {
{"7.0.0","Added the `BYTE|BIT` option."},
{0}
};


#define BITCOUNT_tips NULL


struct redisCommandArg BITCOUNT_index_index_unit_Subargs[] = {
{"byte",ARG_TYPE_PURE_TOKEN,-1,"BYTE",NULL,NULL,CMD_ARG_NONE},
{"bit",ARG_TYPE_PURE_TOKEN,-1,"BIT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg BITCOUNT_index_Subargs[] = {
{"start",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"end",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"index_unit",ARG_TYPE_ONEOF,-1,NULL,NULL,"7.0.0",CMD_ARG_OPTIONAL,.subargs=BITCOUNT_index_index_unit_Subargs},
{0}
};


struct redisCommandArg BITCOUNT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"index",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=BITCOUNT_index_Subargs},
{0}
};




#define BITFIELD_History NULL


#define BITFIELD_tips NULL


struct redisCommandArg BITFIELD_encoding_offset_Subargs[] = {
{"encoding",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg BITFIELD_encoding_offset_value_Subargs[] = {
{"encoding",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg BITFIELD_encoding_offset_increment_Subargs[] = {
{"encoding",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"increment",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg BITFIELD_wrap_sat_fail_Subargs[] = {
{"wrap",ARG_TYPE_PURE_TOKEN,-1,"WRAP",NULL,NULL,CMD_ARG_NONE},
{"sat",ARG_TYPE_PURE_TOKEN,-1,"SAT",NULL,NULL,CMD_ARG_NONE},
{"fail",ARG_TYPE_PURE_TOKEN,-1,"FAIL",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg BITFIELD_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"encoding_offset",ARG_TYPE_BLOCK,-1,"GET",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=BITFIELD_encoding_offset_Subargs},
{"encoding_offset_value",ARG_TYPE_BLOCK,-1,"SET",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=BITFIELD_encoding_offset_value_Subargs},
{"encoding_offset_increment",ARG_TYPE_BLOCK,-1,"INCRBY",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=BITFIELD_encoding_offset_increment_Subargs},
{"wrap_sat_fail",ARG_TYPE_ONEOF,-1,"OVERFLOW",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=BITFIELD_wrap_sat_fail_Subargs},
{0}
};




#define BITFIELD_RO_History NULL


#define BITFIELD_RO_tips NULL


struct redisCommandArg BITFIELD_RO_encoding_offset_Subargs[] = {
{"encoding",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg BITFIELD_RO_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"encoding_offset",ARG_TYPE_BLOCK,-1,"GET",NULL,NULL,CMD_ARG_NONE,.subargs=BITFIELD_RO_encoding_offset_Subargs},
{0}
};




#define BITOP_History NULL


#define BITOP_tips NULL


struct redisCommandArg BITOP_Args[] = {
{"operation",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"destkey",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




commandHistory BITPOS_History[] = {
{"7.0.0","Added the `BYTE|BIT` option."},
{0}
};


#define BITPOS_tips NULL


struct redisCommandArg BITPOS_index_end_index_index_unit_Subargs[] = {
{"byte",ARG_TYPE_PURE_TOKEN,-1,"BYTE",NULL,NULL,CMD_ARG_NONE},
{"bit",ARG_TYPE_PURE_TOKEN,-1,"BIT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg BITPOS_index_end_index_Subargs[] = {
{"end",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"index_unit",ARG_TYPE_ONEOF,-1,NULL,NULL,"7.0.0",CMD_ARG_OPTIONAL,.subargs=BITPOS_index_end_index_index_unit_Subargs},
{0}
};


struct redisCommandArg BITPOS_index_Subargs[] = {
{"start",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"end_index",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=BITPOS_index_end_index_Subargs},
{0}
};


struct redisCommandArg BITPOS_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"bit",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"index",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=BITPOS_index_Subargs},
{0}
};




#define GETBIT_History NULL


#define GETBIT_tips NULL


struct redisCommandArg GETBIT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define SETBIT_History NULL


#define SETBIT_tips NULL


struct redisCommandArg SETBIT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define ASKING_History NULL


#define ASKING_tips NULL




#define CLUSTER_ADDSLOTS_History NULL


const char *CLUSTER_ADDSLOTS_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_ADDSLOTS_Args[] = {
{"slot",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define CLUSTER_ADDSLOTSRANGE_History NULL


const char *CLUSTER_ADDSLOTSRANGE_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_ADDSLOTSRANGE_start_slot_end_slot_Subargs[] = {
{"start-slot",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"end-slot",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLUSTER_ADDSLOTSRANGE_Args[] = {
{"start-slot_end-slot",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE,.subargs=CLUSTER_ADDSLOTSRANGE_start_slot_end_slot_Subargs},
{0}
};




#define CLUSTER_BUMPEPOCH_History NULL


const char *CLUSTER_BUMPEPOCH_tips[] = {
"nondeterministic_output",
NULL
};




#define CLUSTER_COUNT_FAILURE_REPORTS_History NULL


const char *CLUSTER_COUNT_FAILURE_REPORTS_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_COUNT_FAILURE_REPORTS_Args[] = {
{"node-id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define CLUSTER_COUNTKEYSINSLOT_History NULL


const char *CLUSTER_COUNTKEYSINSLOT_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_COUNTKEYSINSLOT_Args[] = {
{"slot",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define CLUSTER_DELSLOTS_History NULL


const char *CLUSTER_DELSLOTS_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_DELSLOTS_Args[] = {
{"slot",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define CLUSTER_DELSLOTSRANGE_History NULL


const char *CLUSTER_DELSLOTSRANGE_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_DELSLOTSRANGE_start_slot_end_slot_Subargs[] = {
{"start-slot",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"end-slot",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLUSTER_DELSLOTSRANGE_Args[] = {
{"start-slot_end-slot",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE,.subargs=CLUSTER_DELSLOTSRANGE_start_slot_end_slot_Subargs},
{0}
};




#define CLUSTER_FAILOVER_History NULL


const char *CLUSTER_FAILOVER_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_FAILOVER_options_Subargs[] = {
{"force",ARG_TYPE_PURE_TOKEN,-1,"FORCE",NULL,NULL,CMD_ARG_NONE},
{"takeover",ARG_TYPE_PURE_TOKEN,-1,"TAKEOVER",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLUSTER_FAILOVER_Args[] = {
{"options",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=CLUSTER_FAILOVER_options_Subargs},
{0}
};




#define CLUSTER_FLUSHSLOTS_History NULL


const char *CLUSTER_FLUSHSLOTS_tips[] = {
"nondeterministic_output",
NULL
};




#define CLUSTER_FORGET_History NULL


const char *CLUSTER_FORGET_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_FORGET_Args[] = {
{"node-id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define CLUSTER_GETKEYSINSLOT_History NULL


const char *CLUSTER_GETKEYSINSLOT_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_GETKEYSINSLOT_Args[] = {
{"slot",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define CLUSTER_HELP_History NULL


#define CLUSTER_HELP_tips NULL




#define CLUSTER_INFO_History NULL


const char *CLUSTER_INFO_tips[] = {
"nondeterministic_output",
NULL
};




#define CLUSTER_KEYSLOT_History NULL


const char *CLUSTER_KEYSLOT_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_KEYSLOT_Args[] = {
{"key",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define CLUSTER_LINKS_History NULL


const char *CLUSTER_LINKS_tips[] = {
"nondeterministic_output",
NULL
};




#define CLUSTER_MEET_History NULL


const char *CLUSTER_MEET_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_MEET_Args[] = {
{"ip",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"port",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define CLUSTER_MYID_History NULL


const char *CLUSTER_MYID_tips[] = {
"nondeterministic_output",
NULL
};




#define CLUSTER_NODES_History NULL


const char *CLUSTER_NODES_tips[] = {
"nondeterministic_output",
NULL
};




#define CLUSTER_REPLICAS_History NULL


const char *CLUSTER_REPLICAS_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_REPLICAS_Args[] = {
{"node-id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define CLUSTER_REPLICATE_History NULL


const char *CLUSTER_REPLICATE_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_REPLICATE_Args[] = {
{"node-id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define CLUSTER_RESET_History NULL


const char *CLUSTER_RESET_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_RESET_hard_soft_Subargs[] = {
{"hard",ARG_TYPE_PURE_TOKEN,-1,"HARD",NULL,NULL,CMD_ARG_NONE},
{"soft",ARG_TYPE_PURE_TOKEN,-1,"SOFT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLUSTER_RESET_Args[] = {
{"hard_soft",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=CLUSTER_RESET_hard_soft_Subargs},
{0}
};




#define CLUSTER_SAVECONFIG_History NULL


const char *CLUSTER_SAVECONFIG_tips[] = {
"nondeterministic_output",
NULL
};




#define CLUSTER_SET_CONFIG_EPOCH_History NULL


const char *CLUSTER_SET_CONFIG_EPOCH_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_SET_CONFIG_EPOCH_Args[] = {
{"config-epoch",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define CLUSTER_SETSLOT_History NULL


const char *CLUSTER_SETSLOT_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_SETSLOT_subcommand_Subargs[] = {
{"node-id",ARG_TYPE_INTEGER,-1,"IMPORTING",NULL,NULL,CMD_ARG_NONE},
{"node-id",ARG_TYPE_INTEGER,-1,"MIGRATING",NULL,NULL,CMD_ARG_NONE},
{"node-id",ARG_TYPE_INTEGER,-1,"NODE",NULL,NULL,CMD_ARG_NONE},
{"stable",ARG_TYPE_PURE_TOKEN,-1,"STABLE",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLUSTER_SETSLOT_Args[] = {
{"slot",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"subcommand",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=CLUSTER_SETSLOT_subcommand_Subargs},
{0}
};




#define CLUSTER_SLAVES_History NULL


const char *CLUSTER_SLAVES_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLUSTER_SLAVES_Args[] = {
{"node-id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory CLUSTER_SLOTS_History[] = {
{"4.0.0","Added node IDs."},
{0}
};


const char *CLUSTER_SLOTS_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommand CLUSTER_Subcommands[] = {
{"addslots","Assign new hash slots to receiving node","O(N) where N is the total number of hash slot arguments","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_ADDSLOTS_History,CLUSTER_ADDSLOTS_tips,clusterCommand,-3,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_ADDSLOTS_Args},
{"addslotsrange","Assign new hash slots to receiving node","O(N) where N is the total number of the slots between the start slot and end slot arguments.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_ADDSLOTSRANGE_History,CLUSTER_ADDSLOTSRANGE_tips,clusterCommand,-4,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_ADDSLOTSRANGE_Args},
{"bumpepoch","Advance the cluster config epoch","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_BUMPEPOCH_History,CLUSTER_BUMPEPOCH_tips,clusterCommand,2,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0},
{"count-failure-reports","Return the number of failure reports active for a given node","O(N) where N is the number of failure reports","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_COUNT_FAILURE_REPORTS_History,CLUSTER_COUNT_FAILURE_REPORTS_tips,clusterCommand,3,CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_COUNT_FAILURE_REPORTS_Args},
{"countkeysinslot","Return the number of local keys in the specified hash slot","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_COUNTKEYSINSLOT_History,CLUSTER_COUNTKEYSINSLOT_tips,clusterCommand,3,CMD_STALE,0,.args=CLUSTER_COUNTKEYSINSLOT_Args},
{"delslots","Set hash slots as unbound in receiving node","O(N) where N is the total number of hash slot arguments","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_DELSLOTS_History,CLUSTER_DELSLOTS_tips,clusterCommand,-3,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_DELSLOTS_Args},
{"delslotsrange","Set hash slots as unbound in receiving node","O(N) where N is the total number of the slots between the start slot and end slot arguments.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_DELSLOTSRANGE_History,CLUSTER_DELSLOTSRANGE_tips,clusterCommand,-4,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_DELSLOTSRANGE_Args},
{"failover","Forces a replica to perform a manual failover of its master.","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_FAILOVER_History,CLUSTER_FAILOVER_tips,clusterCommand,-2,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_FAILOVER_Args},
{"flushslots","Delete a node's own slots information","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_FLUSHSLOTS_History,CLUSTER_FLUSHSLOTS_tips,clusterCommand,2,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0},
{"forget","Remove a node from the nodes table","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_FORGET_History,CLUSTER_FORGET_tips,clusterCommand,3,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_FORGET_Args},
{"getkeysinslot","Return local key names in the specified hash slot","O(log(N)) where N is the number of requested keys","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_GETKEYSINSLOT_History,CLUSTER_GETKEYSINSLOT_tips,clusterCommand,4,CMD_STALE,0,.args=CLUSTER_GETKEYSINSLOT_Args},
{"help","Show helpful text about the different subcommands","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_HELP_History,CLUSTER_HELP_tips,clusterCommand,2,CMD_LOADING|CMD_STALE,0},
{"info","Provides info about Redis Cluster node state","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_INFO_History,CLUSTER_INFO_tips,clusterCommand,2,CMD_STALE,0},
{"keyslot","Returns the hash slot of the specified key","O(N) where N is the number of bytes in the key","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_KEYSLOT_History,CLUSTER_KEYSLOT_tips,clusterCommand,3,CMD_STALE,0,.args=CLUSTER_KEYSLOT_Args},
{"links","Returns a list of all TCP links to and from peer nodes in cluster","O(N) where N is the total number of Cluster nodes","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_LINKS_History,CLUSTER_LINKS_tips,clusterCommand,2,CMD_STALE,0},
{"meet","Force a node cluster to handshake with another node","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_MEET_History,CLUSTER_MEET_tips,clusterCommand,-4,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_MEET_Args},
{"myid","Return the node id","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_MYID_History,CLUSTER_MYID_tips,clusterCommand,2,CMD_STALE,0},
{"nodes","Get Cluster config for the node","O(N) where N is the total number of Cluster nodes","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_NODES_History,CLUSTER_NODES_tips,clusterCommand,2,CMD_STALE,0},
{"replicas","List replica nodes of the specified master node","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_REPLICAS_History,CLUSTER_REPLICAS_tips,clusterCommand,3,CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_REPLICAS_Args},
{"replicate","Reconfigure a node as a replica of the specified master node","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_REPLICATE_History,CLUSTER_REPLICATE_tips,clusterCommand,3,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_REPLICATE_Args},
{"reset","Reset a Redis Cluster node","O(N) where N is the number of known nodes. The command may execute a FLUSHALL as a side effect.","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_RESET_History,CLUSTER_RESET_tips,clusterCommand,3,CMD_ADMIN|CMD_STALE|CMD_NOSCRIPT,0,.args=CLUSTER_RESET_Args},
{"saveconfig","Forces the node to save cluster state on disk","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_SAVECONFIG_History,CLUSTER_SAVECONFIG_tips,clusterCommand,2,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0},
{"set-config-epoch","Set the configuration epoch in a new node","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_SET_CONFIG_EPOCH_History,CLUSTER_SET_CONFIG_EPOCH_tips,clusterCommand,3,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_SET_CONFIG_EPOCH_Args},
{"setslot","Bind a hash slot to a specific node","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_SETSLOT_History,CLUSTER_SETSLOT_tips,clusterCommand,-4,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_SETSLOT_Args},
{"slaves","List replica nodes of the specified master node","O(1)","3.0.0",CMD_DOC_NONE,"`CLUSTER REPLICAS`","5.0.0",COMMAND_GROUP_CLUSTER,CLUSTER_SLAVES_History,CLUSTER_SLAVES_tips,clusterCommand,3,CMD_ADMIN|CMD_STALE,0,.args=CLUSTER_SLAVES_Args},
{"slots","Get array of Cluster slot to node mappings","O(N) where N is the total number of Cluster nodes","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_SLOTS_History,CLUSTER_SLOTS_tips,clusterCommand,2,CMD_STALE,0},
{0}
};




#define CLUSTER_History NULL


#define CLUSTER_tips NULL




#define READONLY_History NULL


#define READONLY_tips NULL




#define READWRITE_History NULL


#define READWRITE_tips NULL




commandHistory AUTH_History[] = {
{"6.0.0","Added ACL style (username and password)."},
{0}
};


#define AUTH_tips NULL


struct redisCommandArg AUTH_Args[] = {
{"username",ARG_TYPE_STRING,-1,NULL,NULL,"6.0.0",CMD_ARG_OPTIONAL},
{"password",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define CLIENT_CACHING_History NULL


#define CLIENT_CACHING_tips NULL


struct redisCommandArg CLIENT_CACHING_mode_Subargs[] = {
{"yes",ARG_TYPE_PURE_TOKEN,-1,"YES",NULL,NULL,CMD_ARG_NONE},
{"no",ARG_TYPE_PURE_TOKEN,-1,"NO",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLIENT_CACHING_Args[] = {
{"mode",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=CLIENT_CACHING_mode_Subargs},
{0}
};




#define CLIENT_GETNAME_History NULL


#define CLIENT_GETNAME_tips NULL




#define CLIENT_GETREDIR_History NULL


#define CLIENT_GETREDIR_tips NULL




#define CLIENT_HELP_History NULL


#define CLIENT_HELP_tips NULL




#define CLIENT_ID_History NULL


#define CLIENT_ID_tips NULL




#define CLIENT_INFO_History NULL


const char *CLIENT_INFO_tips[] = {
"nondeterministic_output",
NULL
};




commandHistory CLIENT_KILL_History[] = {
{"2.8.12","Added new filter format."},
{"2.8.12","`ID` option."},
{"3.2.0","Added `master` type in for `TYPE` option."},
{"5.0.0","Replaced `slave` `TYPE` with `replica`. `slave` still supported for backward compatibility."},
{"6.2.0","`LADDR` option."},
{0}
};


#define CLIENT_KILL_tips NULL


struct redisCommandArg CLIENT_KILL_normal_master_slave_pubsub_Subargs[] = {
{"normal",ARG_TYPE_PURE_TOKEN,-1,"NORMAL",NULL,NULL,CMD_ARG_NONE},
{"master",ARG_TYPE_PURE_TOKEN,-1,"MASTER",NULL,"3.2.0",CMD_ARG_NONE},
{"slave",ARG_TYPE_PURE_TOKEN,-1,"SLAVE",NULL,NULL,CMD_ARG_NONE},
{"replica",ARG_TYPE_PURE_TOKEN,-1,"REPLICA",NULL,"5.0.0",CMD_ARG_NONE},
{"pubsub",ARG_TYPE_PURE_TOKEN,-1,"PUBSUB",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLIENT_KILL_Args[] = {
{"ip:port",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL},
{"client-id",ARG_TYPE_INTEGER,-1,"ID",NULL,"2.8.12",CMD_ARG_OPTIONAL},
{"normal_master_slave_pubsub",ARG_TYPE_ONEOF,-1,"TYPE",NULL,"2.8.12",CMD_ARG_OPTIONAL,.subargs=CLIENT_KILL_normal_master_slave_pubsub_Subargs},
{"username",ARG_TYPE_STRING,-1,"USER",NULL,NULL,CMD_ARG_OPTIONAL},
{"ip:port",ARG_TYPE_STRING,-1,"ADDR",NULL,NULL,CMD_ARG_OPTIONAL},
{"ip:port",ARG_TYPE_STRING,-1,"LADDR",NULL,"6.2.0",CMD_ARG_OPTIONAL},
{"yes/no",ARG_TYPE_STRING,-1,"SKIPME",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




commandHistory CLIENT_LIST_History[] = {
{"2.8.12","Added unique client `id` field."},
{"5.0.0","Added optional `TYPE` filter."},
{"6.2.0","Added `laddr` field and the optional `ID` filter."},
{0}
};


const char *CLIENT_LIST_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg CLIENT_LIST_normal_master_replica_pubsub_Subargs[] = {
{"normal",ARG_TYPE_PURE_TOKEN,-1,"NORMAL",NULL,NULL,CMD_ARG_NONE},
{"master",ARG_TYPE_PURE_TOKEN,-1,"MASTER",NULL,NULL,CMD_ARG_NONE},
{"replica",ARG_TYPE_PURE_TOKEN,-1,"REPLICA",NULL,NULL,CMD_ARG_NONE},
{"pubsub",ARG_TYPE_PURE_TOKEN,-1,"PUBSUB",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLIENT_LIST_id_Subargs[] = {
{"client-id",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};


struct redisCommandArg CLIENT_LIST_Args[] = {
{"normal_master_replica_pubsub",ARG_TYPE_ONEOF,-1,"TYPE",NULL,"5.0.0",CMD_ARG_OPTIONAL,.subargs=CLIENT_LIST_normal_master_replica_pubsub_Subargs},
{"id",ARG_TYPE_BLOCK,-1,"ID",NULL,"6.2.0",CMD_ARG_OPTIONAL,.subargs=CLIENT_LIST_id_Subargs},
{0}
};




#define CLIENT_NO_EVICT_History NULL


#define CLIENT_NO_EVICT_tips NULL


struct redisCommandArg CLIENT_NO_EVICT_enabled_Subargs[] = {
{"on",ARG_TYPE_PURE_TOKEN,-1,"ON",NULL,NULL,CMD_ARG_NONE},
{"off",ARG_TYPE_PURE_TOKEN,-1,"OFF",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLIENT_NO_EVICT_Args[] = {
{"enabled",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=CLIENT_NO_EVICT_enabled_Subargs},
{0}
};




commandHistory CLIENT_PAUSE_History[] = {
{"3.2.10","Client pause prevents client pause and key eviction as well."},
{"6.2.0","`CLIENT PAUSE WRITE` mode added along with the `mode` option."},
{0}
};


#define CLIENT_PAUSE_tips NULL


struct redisCommandArg CLIENT_PAUSE_mode_Subargs[] = {
{"write",ARG_TYPE_PURE_TOKEN,-1,"WRITE",NULL,NULL,CMD_ARG_NONE},
{"all",ARG_TYPE_PURE_TOKEN,-1,"ALL",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLIENT_PAUSE_Args[] = {
{"timeout",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"mode",ARG_TYPE_ONEOF,-1,NULL,NULL,"6.2.0",CMD_ARG_OPTIONAL,.subargs=CLIENT_PAUSE_mode_Subargs},
{0}
};




#define CLIENT_REPLY_History NULL


#define CLIENT_REPLY_tips NULL


struct redisCommandArg CLIENT_REPLY_on_off_skip_Subargs[] = {
{"on",ARG_TYPE_PURE_TOKEN,-1,"ON",NULL,NULL,CMD_ARG_NONE},
{"off",ARG_TYPE_PURE_TOKEN,-1,"OFF",NULL,NULL,CMD_ARG_NONE},
{"skip",ARG_TYPE_PURE_TOKEN,-1,"SKIP",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLIENT_REPLY_Args[] = {
{"on_off_skip",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=CLIENT_REPLY_on_off_skip_Subargs},
{0}
};




#define CLIENT_SETNAME_History NULL


#define CLIENT_SETNAME_tips NULL


struct redisCommandArg CLIENT_SETNAME_Args[] = {
{"connection-name",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define CLIENT_TRACKING_History NULL


#define CLIENT_TRACKING_tips NULL


struct redisCommandArg CLIENT_TRACKING_status_Subargs[] = {
{"on",ARG_TYPE_PURE_TOKEN,-1,"ON",NULL,NULL,CMD_ARG_NONE},
{"off",ARG_TYPE_PURE_TOKEN,-1,"OFF",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLIENT_TRACKING_Args[] = {
{"status",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=CLIENT_TRACKING_status_Subargs},
{"client-id",ARG_TYPE_INTEGER,-1,"REDIRECT",NULL,NULL,CMD_ARG_OPTIONAL},
{"prefix",ARG_TYPE_STRING,-1,"PREFIX",NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE|CMD_ARG_MULTIPLE_TOKEN},
{"bcast",ARG_TYPE_PURE_TOKEN,-1,"BCAST",NULL,NULL,CMD_ARG_OPTIONAL},
{"optin",ARG_TYPE_PURE_TOKEN,-1,"OPTIN",NULL,NULL,CMD_ARG_OPTIONAL},
{"optout",ARG_TYPE_PURE_TOKEN,-1,"OPTOUT",NULL,NULL,CMD_ARG_OPTIONAL},
{"noloop",ARG_TYPE_PURE_TOKEN,-1,"NOLOOP",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define CLIENT_TRACKINGINFO_History NULL


#define CLIENT_TRACKINGINFO_tips NULL




#define CLIENT_UNBLOCK_History NULL


#define CLIENT_UNBLOCK_tips NULL


struct redisCommandArg CLIENT_UNBLOCK_timeout_error_Subargs[] = {
{"timeout",ARG_TYPE_PURE_TOKEN,-1,"TIMEOUT",NULL,NULL,CMD_ARG_NONE},
{"error",ARG_TYPE_PURE_TOKEN,-1,"ERROR",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CLIENT_UNBLOCK_Args[] = {
{"client-id",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"timeout_error",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=CLIENT_UNBLOCK_timeout_error_Subargs},
{0}
};




#define CLIENT_UNPAUSE_History NULL


#define CLIENT_UNPAUSE_tips NULL


struct redisCommand CLIENT_Subcommands[] = {
{"caching","Instruct the server about tracking or not keys in the next request","O(1)","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_CACHING_History,CLIENT_CACHING_tips,clientCommand,3,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION,.args=CLIENT_CACHING_Args},
{"getname","Get the current connection name","O(1)","2.6.9",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_GETNAME_History,CLIENT_GETNAME_tips,clientCommand,2,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION},
{"getredir","Get tracking notifications redirection client ID if any","O(1)","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_GETREDIR_History,CLIENT_GETREDIR_tips,clientCommand,2,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION},
{"help","Show helpful text about the different subcommands","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_HELP_History,CLIENT_HELP_tips,clientCommand,2,CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION},
{"id","Returns the client ID for the current connection","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_ID_History,CLIENT_ID_tips,clientCommand,2,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION},
{"info","Returns information about the current client connection.","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_INFO_History,CLIENT_INFO_tips,clientCommand,2,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION},
{"kill","Kill the connection of a client","O(N) where N is the number of client connections","2.4.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_KILL_History,CLIENT_KILL_tips,clientCommand,-3,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION,.args=CLIENT_KILL_Args},
{"list","Get the list of client connections","O(N) where N is the number of client connections","2.4.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_LIST_History,CLIENT_LIST_tips,clientCommand,-2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION,.args=CLIENT_LIST_Args},
{"no-evict","Set client eviction mode for the current connection","O(1)","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_NO_EVICT_History,CLIENT_NO_EVICT_tips,clientCommand,3,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION,.args=CLIENT_NO_EVICT_Args},
{"pause","Stop processing commands from clients for some time","O(1)","2.9.50",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_PAUSE_History,CLIENT_PAUSE_tips,clientCommand,-3,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION,.args=CLIENT_PAUSE_Args},
{"reply","Instruct the server whether to reply to commands","O(1)","3.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_REPLY_History,CLIENT_REPLY_tips,clientCommand,3,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION,.args=CLIENT_REPLY_Args},
{"setname","Set the current connection name","O(1)","2.6.9",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_SETNAME_History,CLIENT_SETNAME_tips,clientCommand,3,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION,.args=CLIENT_SETNAME_Args},
{"tracking","Enable or disable server assisted client side caching support","O(1). Some options may introduce additional complexity.","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_TRACKING_History,CLIENT_TRACKING_tips,clientCommand,-3,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION,.args=CLIENT_TRACKING_Args},
{"trackinginfo","Return information about server assisted client side caching for the current connection","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_TRACKINGINFO_History,CLIENT_TRACKINGINFO_tips,clientCommand,2,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION},
{"unblock","Unblock a client blocked in a blocking command from a different connection","O(log N) where N is the number of client connections","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_UNBLOCK_History,CLIENT_UNBLOCK_tips,clientCommand,-3,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION,.args=CLIENT_UNBLOCK_Args},
{"unpause","Resume processing of clients that were paused","O(N) Where N is the number of paused clients","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_UNPAUSE_History,CLIENT_UNPAUSE_tips,clientCommand,2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION},
{0}
};




#define CLIENT_History NULL


#define CLIENT_tips NULL




#define ECHO_History NULL


#define ECHO_tips NULL


struct redisCommandArg ECHO_Args[] = {
{"message",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory HELLO_History[] = {
{"6.2.0","`protover` made optional; when called without arguments the command reports the current connection's context."},
{0}
};


#define HELLO_tips NULL


struct redisCommandArg HELLO_arguments_username_password_Subargs[] = {
{"username",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"password",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg HELLO_arguments_Subargs[] = {
{"protover",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"username_password",ARG_TYPE_BLOCK,-1,"AUTH",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=HELLO_arguments_username_password_Subargs},
{"clientname",ARG_TYPE_STRING,-1,"SETNAME",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg HELLO_Args[] = {
{"arguments",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=HELLO_arguments_Subargs},
{0}
};




#define PING_History NULL


const char *PING_tips[] = {
"request_policy:all_shards",
"response_policy:all_succeeded",
NULL
};


struct redisCommandArg PING_Args[] = {
{"message",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define QUIT_History NULL


#define QUIT_tips NULL




#define RESET_History NULL


#define RESET_tips NULL




#define SELECT_History NULL


#define SELECT_tips NULL


struct redisCommandArg SELECT_Args[] = {
{"index",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define COPY_History NULL


#define COPY_tips NULL


struct redisCommandArg COPY_Args[] = {
{"source",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"destination",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_NONE},
{"destination-db",ARG_TYPE_INTEGER,-1,"DB",NULL,NULL,CMD_ARG_OPTIONAL},
{"replace",ARG_TYPE_PURE_TOKEN,-1,"REPLACE",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define DEL_History NULL


const char *DEL_tips[] = {
"request_policy:multi_shard",
"response_policy:agg_sum",
NULL
};


struct redisCommandArg DEL_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define DUMP_History NULL


const char *DUMP_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg DUMP_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory EXISTS_History[] = {
{"3.0.3","Accepts multiple `key` arguments."},
{0}
};


const char *EXISTS_tips[] = {
"request_policy:multi_shard",
"response_policy:agg_sum",
NULL
};


struct redisCommandArg EXISTS_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




commandHistory EXPIRE_History[] = {
{"7.0.0","Added options: `NX`, `XX`, `GT` and `LT`."},
{0}
};


#define EXPIRE_tips NULL


struct redisCommandArg EXPIRE_condition_Subargs[] = {
{"nx",ARG_TYPE_PURE_TOKEN,-1,"NX",NULL,NULL,CMD_ARG_NONE},
{"xx",ARG_TYPE_PURE_TOKEN,-1,"XX",NULL,NULL,CMD_ARG_NONE},
{"gt",ARG_TYPE_PURE_TOKEN,-1,"GT",NULL,NULL,CMD_ARG_NONE},
{"lt",ARG_TYPE_PURE_TOKEN,-1,"LT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg EXPIRE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"seconds",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"condition",ARG_TYPE_ONEOF,-1,NULL,NULL,"7.0.0",CMD_ARG_OPTIONAL,.subargs=EXPIRE_condition_Subargs},
{0}
};




commandHistory EXPIREAT_History[] = {
{"7.0.0","Added options: `NX`, `XX`, `GT` and `LT`."},
{0}
};


#define EXPIREAT_tips NULL


struct redisCommandArg EXPIREAT_condition_Subargs[] = {
{"nx",ARG_TYPE_PURE_TOKEN,-1,"NX",NULL,NULL,CMD_ARG_NONE},
{"xx",ARG_TYPE_PURE_TOKEN,-1,"XX",NULL,NULL,CMD_ARG_NONE},
{"gt",ARG_TYPE_PURE_TOKEN,-1,"GT",NULL,NULL,CMD_ARG_NONE},
{"lt",ARG_TYPE_PURE_TOKEN,-1,"LT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg EXPIREAT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"timestamp",ARG_TYPE_UNIX_TIME,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"condition",ARG_TYPE_ONEOF,-1,NULL,NULL,"7.0.0",CMD_ARG_OPTIONAL,.subargs=EXPIREAT_condition_Subargs},
{0}
};




#define EXPIRETIME_History NULL


#define EXPIRETIME_tips NULL


struct redisCommandArg EXPIRETIME_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define KEYS_History NULL


const char *KEYS_tips[] = {
"request_policy:all_shards",
"nondeterministic_output_order",
NULL
};


struct redisCommandArg KEYS_Args[] = {
{"pattern",ARG_TYPE_PATTERN,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory MIGRATE_History[] = {
{"3.0.0","Added the `COPY` and `REPLACE` options."},
{"3.0.6","Added the `KEYS` option."},
{"4.0.7","Added the `AUTH` option."},
{"6.0.0","Added the `AUTH2` option."},
{0}
};


const char *MIGRATE_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg MIGRATE_key_or_empty_string_Subargs[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"empty_string",ARG_TYPE_PURE_TOKEN,-1,"""",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg MIGRATE_username_password_Subargs[] = {
{"username",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"password",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg MIGRATE_Args[] = {
{"host",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"port",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key_or_empty_string",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=MIGRATE_key_or_empty_string_Subargs},
{"destination-db",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"timeout",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"copy",ARG_TYPE_PURE_TOKEN,-1,"COPY",NULL,"3.0.0",CMD_ARG_OPTIONAL},
{"replace",ARG_TYPE_PURE_TOKEN,-1,"REPLACE",NULL,"3.0.0",CMD_ARG_OPTIONAL},
{"password",ARG_TYPE_STRING,-1,"AUTH",NULL,"4.0.7",CMD_ARG_OPTIONAL},
{"username_password",ARG_TYPE_BLOCK,-1,"AUTH2",NULL,"6.0.0",CMD_ARG_OPTIONAL,.subargs=MIGRATE_username_password_Subargs},
{"key",ARG_TYPE_KEY,1,"KEYS",NULL,"3.0.6",CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define MOVE_History NULL


#define MOVE_tips NULL


struct redisCommandArg MOVE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"db",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define OBJECT_ENCODING_History NULL


const char *OBJECT_ENCODING_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg OBJECT_ENCODING_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define OBJECT_FREQ_History NULL


const char *OBJECT_FREQ_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg OBJECT_FREQ_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define OBJECT_HELP_History NULL


#define OBJECT_HELP_tips NULL




#define OBJECT_IDLETIME_History NULL


const char *OBJECT_IDLETIME_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg OBJECT_IDLETIME_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define OBJECT_REFCOUNT_History NULL


#define OBJECT_REFCOUNT_tips NULL


struct redisCommandArg OBJECT_REFCOUNT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommand OBJECT_Subcommands[] = {
{"encoding","Inspect the internal encoding of a Redis object","O(1)","2.2.3",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,OBJECT_ENCODING_History,OBJECT_ENCODING_tips,objectCommand,3,CMD_READONLY,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=OBJECT_ENCODING_Args},
{"freq","Get the logarithmic access frequency counter of a Redis object","O(1)","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,OBJECT_FREQ_History,OBJECT_FREQ_tips,objectCommand,3,CMD_READONLY,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=OBJECT_FREQ_Args},
{"help","Show helpful text about the different subcommands","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,OBJECT_HELP_History,OBJECT_HELP_tips,objectCommand,2,CMD_LOADING|CMD_STALE,ACL_CATEGORY_KEYSPACE},
{"idletime","Get the time since a Redis object was last accessed","O(1)","2.2.3",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,OBJECT_IDLETIME_History,OBJECT_IDLETIME_tips,objectCommand,3,CMD_READONLY,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=OBJECT_IDLETIME_Args},
{"refcount","Get the number of references to the value of the key","O(1)","2.2.3",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,OBJECT_REFCOUNT_History,OBJECT_REFCOUNT_tips,objectCommand,3,CMD_READONLY,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=OBJECT_REFCOUNT_Args},
{0}
};




#define OBJECT_History NULL


#define OBJECT_tips NULL




#define PERSIST_History NULL


#define PERSIST_tips NULL


struct redisCommandArg PERSIST_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory PEXPIRE_History[] = {
{"7.0.0","Added options: `NX`, `XX`, `GT` and `LT`."},
{0}
};


#define PEXPIRE_tips NULL


struct redisCommandArg PEXPIRE_condition_Subargs[] = {
{"nx",ARG_TYPE_PURE_TOKEN,-1,"NX",NULL,NULL,CMD_ARG_NONE},
{"xx",ARG_TYPE_PURE_TOKEN,-1,"XX",NULL,NULL,CMD_ARG_NONE},
{"gt",ARG_TYPE_PURE_TOKEN,-1,"GT",NULL,NULL,CMD_ARG_NONE},
{"lt",ARG_TYPE_PURE_TOKEN,-1,"LT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg PEXPIRE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"milliseconds",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"condition",ARG_TYPE_ONEOF,-1,NULL,NULL,"7.0.0",CMD_ARG_OPTIONAL,.subargs=PEXPIRE_condition_Subargs},
{0}
};




commandHistory PEXPIREAT_History[] = {
{"7.0.0","Added options: `NX`, `XX`, `GT` and `LT`."},
{0}
};


#define PEXPIREAT_tips NULL


struct redisCommandArg PEXPIREAT_condition_Subargs[] = {
{"nx",ARG_TYPE_PURE_TOKEN,-1,"NX",NULL,NULL,CMD_ARG_NONE},
{"xx",ARG_TYPE_PURE_TOKEN,-1,"XX",NULL,NULL,CMD_ARG_NONE},
{"gt",ARG_TYPE_PURE_TOKEN,-1,"GT",NULL,NULL,CMD_ARG_NONE},
{"lt",ARG_TYPE_PURE_TOKEN,-1,"LT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg PEXPIREAT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"milliseconds-timestamp",ARG_TYPE_UNIX_TIME,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"condition",ARG_TYPE_ONEOF,-1,NULL,NULL,"7.0.0",CMD_ARG_OPTIONAL,.subargs=PEXPIREAT_condition_Subargs},
{0}
};




#define PEXPIRETIME_History NULL


#define PEXPIRETIME_tips NULL


struct redisCommandArg PEXPIRETIME_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory PTTL_History[] = {
{"2.8.0","Added the -2 reply."},
{0}
};


const char *PTTL_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg PTTL_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define RANDOMKEY_History NULL


const char *RANDOMKEY_tips[] = {
"request_policy:all_shards",
"nondeterministic_output",
NULL
};




commandHistory RENAME_History[] = {
{"3.2.0","The command no longer returns an error when source and destination names are the same."},
{0}
};


#define RENAME_tips NULL


struct redisCommandArg RENAME_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"newkey",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory RENAMENX_History[] = {
{"3.2.0","The command no longer returns an error when source and destination names are the same."},
{0}
};


#define RENAMENX_tips NULL


struct redisCommandArg RENAMENX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"newkey",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory RESTORE_History[] = {
{"3.0.0","Added the `REPLACE` modifier."},
{"5.0.0","Added the `ABSTTL` modifier."},
{"5.0.0","Added the `IDLETIME` and `FREQ` options."},
{0}
};


#define RESTORE_tips NULL


struct redisCommandArg RESTORE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"ttl",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"serialized-value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"replace",ARG_TYPE_PURE_TOKEN,-1,"REPLACE",NULL,"3.0.0",CMD_ARG_OPTIONAL},
{"absttl",ARG_TYPE_PURE_TOKEN,-1,"ABSTTL",NULL,"5.0.0",CMD_ARG_OPTIONAL},
{"seconds",ARG_TYPE_INTEGER,-1,"IDLETIME",NULL,"5.0.0",CMD_ARG_OPTIONAL},
{"frequency",ARG_TYPE_INTEGER,-1,"FREQ",NULL,"5.0.0",CMD_ARG_OPTIONAL},
{0}
};




commandHistory SCAN_History[] = {
{"6.0.0","Added the `TYPE` subcommand."},
{0}
};


const char *SCAN_tips[] = {
"nondeterministic_output",
"request_policy:special",
NULL
};


struct redisCommandArg SCAN_Args[] = {
{"cursor",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"pattern",ARG_TYPE_PATTERN,-1,"MATCH",NULL,NULL,CMD_ARG_OPTIONAL},
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{"type",ARG_TYPE_STRING,-1,"TYPE",NULL,"6.0.0",CMD_ARG_OPTIONAL},
{0}
};




#define SORT_History NULL


#define SORT_tips NULL


struct redisCommandArg SORT_offset_count_Subargs[] = {
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg SORT_order_Subargs[] = {
{"asc",ARG_TYPE_PURE_TOKEN,-1,"ASC",NULL,NULL,CMD_ARG_NONE},
{"desc",ARG_TYPE_PURE_TOKEN,-1,"DESC",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg SORT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"pattern",ARG_TYPE_PATTERN,-1,"BY",NULL,NULL,CMD_ARG_OPTIONAL},
{"offset_count",ARG_TYPE_BLOCK,-1,"LIMIT",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=SORT_offset_count_Subargs},
{"pattern",ARG_TYPE_STRING,-1,"GET",NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE|CMD_ARG_MULTIPLE_TOKEN},
{"order",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=SORT_order_Subargs},
{"sorting",ARG_TYPE_PURE_TOKEN,-1,"ALPHA",NULL,NULL,CMD_ARG_OPTIONAL},
{"destination",ARG_TYPE_KEY,1,"STORE",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define SORT_RO_History NULL


#define SORT_RO_tips NULL


struct redisCommandArg SORT_RO_offset_count_Subargs[] = {
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg SORT_RO_order_Subargs[] = {
{"asc",ARG_TYPE_PURE_TOKEN,-1,"ASC",NULL,NULL,CMD_ARG_NONE},
{"desc",ARG_TYPE_PURE_TOKEN,-1,"DESC",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg SORT_RO_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"pattern",ARG_TYPE_PATTERN,-1,"BY",NULL,NULL,CMD_ARG_OPTIONAL},
{"offset_count",ARG_TYPE_BLOCK,-1,"LIMIT",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=SORT_RO_offset_count_Subargs},
{"pattern",ARG_TYPE_STRING,-1,"GET",NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE|CMD_ARG_MULTIPLE_TOKEN},
{"order",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=SORT_RO_order_Subargs},
{"sorting",ARG_TYPE_PURE_TOKEN,-1,"ALPHA",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define TOUCH_History NULL


const char *TOUCH_tips[] = {
"request_policy:multi_shard",
"response_policy:agg_sum",
NULL
};


struct redisCommandArg TOUCH_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




commandHistory TTL_History[] = {
{"2.8.0","Added the -2 reply."},
{0}
};


const char *TTL_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg TTL_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define TYPE_History NULL


#define TYPE_tips NULL


struct redisCommandArg TYPE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define UNLINK_History NULL


const char *UNLINK_tips[] = {
"request_policy:multi_shard",
"response_policy:agg_sum",
NULL
};


struct redisCommandArg UNLINK_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define WAIT_History NULL


const char *WAIT_tips[] = {
"request_policy:all_shards",
"response_policy:agg_min",
NULL
};


struct redisCommandArg WAIT_Args[] = {
{"numreplicas",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"timeout",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory GEOADD_History[] = {
{"6.2.0","Added the `CH`, `NX` and `XX` options."},
{0}
};


#define GEOADD_tips NULL


struct redisCommandArg GEOADD_condition_Subargs[] = {
{"nx",ARG_TYPE_PURE_TOKEN,-1,"NX",NULL,NULL,CMD_ARG_NONE},
{"xx",ARG_TYPE_PURE_TOKEN,-1,"XX",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEOADD_longitude_latitude_member_Subargs[] = {
{"longitude",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"latitude",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEOADD_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"condition",ARG_TYPE_ONEOF,-1,NULL,NULL,"6.2.0",CMD_ARG_OPTIONAL,.subargs=GEOADD_condition_Subargs},
{"change",ARG_TYPE_PURE_TOKEN,-1,"CH",NULL,"6.2.0",CMD_ARG_OPTIONAL},
{"longitude_latitude_member",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE,.subargs=GEOADD_longitude_latitude_member_Subargs},
{0}
};




#define GEODIST_History NULL


#define GEODIST_tips NULL


struct redisCommandArg GEODIST_unit_Subargs[] = {
{"m",ARG_TYPE_PURE_TOKEN,-1,"M",NULL,NULL,CMD_ARG_NONE},
{"km",ARG_TYPE_PURE_TOKEN,-1,"KM",NULL,NULL,CMD_ARG_NONE},
{"ft",ARG_TYPE_PURE_TOKEN,-1,"FT",NULL,NULL,CMD_ARG_NONE},
{"mi",ARG_TYPE_PURE_TOKEN,-1,"MI",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEODIST_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member1",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"member2",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"unit",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEODIST_unit_Subargs},
{0}
};




#define GEOHASH_History NULL


#define GEOHASH_tips NULL


struct redisCommandArg GEOHASH_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define GEOPOS_History NULL


#define GEOPOS_tips NULL


struct redisCommandArg GEOPOS_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




commandHistory GEORADIUS_History[] = {
{"6.2.0","Added the `ANY` option for `COUNT`."},
{0}
};


#define GEORADIUS_tips NULL


struct redisCommandArg GEORADIUS_unit_Subargs[] = {
{"m",ARG_TYPE_PURE_TOKEN,-1,"M",NULL,NULL,CMD_ARG_NONE},
{"km",ARG_TYPE_PURE_TOKEN,-1,"KM",NULL,NULL,CMD_ARG_NONE},
{"ft",ARG_TYPE_PURE_TOKEN,-1,"FT",NULL,NULL,CMD_ARG_NONE},
{"mi",ARG_TYPE_PURE_TOKEN,-1,"MI",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEORADIUS_count_Subargs[] = {
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_NONE},
{"any",ARG_TYPE_PURE_TOKEN,-1,"ANY",NULL,"6.2.0",CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg GEORADIUS_order_Subargs[] = {
{"asc",ARG_TYPE_PURE_TOKEN,-1,"ASC",NULL,NULL,CMD_ARG_NONE},
{"desc",ARG_TYPE_PURE_TOKEN,-1,"DESC",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEORADIUS_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"longitude",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"latitude",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"radius",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"unit",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=GEORADIUS_unit_Subargs},
{"withcoord",ARG_TYPE_PURE_TOKEN,-1,"WITHCOORD",NULL,NULL,CMD_ARG_OPTIONAL},
{"withdist",ARG_TYPE_PURE_TOKEN,-1,"WITHDIST",NULL,NULL,CMD_ARG_OPTIONAL},
{"withhash",ARG_TYPE_PURE_TOKEN,-1,"WITHHASH",NULL,NULL,CMD_ARG_OPTIONAL},
{"count",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEORADIUS_count_Subargs},
{"order",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEORADIUS_order_Subargs},
{"key",ARG_TYPE_KEY,1,"STORE",NULL,NULL,CMD_ARG_OPTIONAL},
{"key",ARG_TYPE_KEY,2,"STOREDIST",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define GEORADIUSBYMEMBER_History NULL


#define GEORADIUSBYMEMBER_tips NULL


struct redisCommandArg GEORADIUSBYMEMBER_unit_Subargs[] = {
{"m",ARG_TYPE_PURE_TOKEN,-1,"M",NULL,NULL,CMD_ARG_NONE},
{"km",ARG_TYPE_PURE_TOKEN,-1,"KM",NULL,NULL,CMD_ARG_NONE},
{"ft",ARG_TYPE_PURE_TOKEN,-1,"FT",NULL,NULL,CMD_ARG_NONE},
{"mi",ARG_TYPE_PURE_TOKEN,-1,"MI",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEORADIUSBYMEMBER_count_Subargs[] = {
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_NONE},
{"any",ARG_TYPE_PURE_TOKEN,-1,"ANY",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg GEORADIUSBYMEMBER_order_Subargs[] = {
{"asc",ARG_TYPE_PURE_TOKEN,-1,"ASC",NULL,NULL,CMD_ARG_NONE},
{"desc",ARG_TYPE_PURE_TOKEN,-1,"DESC",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEORADIUSBYMEMBER_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"radius",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"unit",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=GEORADIUSBYMEMBER_unit_Subargs},
{"withcoord",ARG_TYPE_PURE_TOKEN,-1,"WITHCOORD",NULL,NULL,CMD_ARG_OPTIONAL},
{"withdist",ARG_TYPE_PURE_TOKEN,-1,"WITHDIST",NULL,NULL,CMD_ARG_OPTIONAL},
{"withhash",ARG_TYPE_PURE_TOKEN,-1,"WITHHASH",NULL,NULL,CMD_ARG_OPTIONAL},
{"count",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEORADIUSBYMEMBER_count_Subargs},
{"order",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEORADIUSBYMEMBER_order_Subargs},
{"key",ARG_TYPE_KEY,1,"STORE",NULL,NULL,CMD_ARG_OPTIONAL},
{"key",ARG_TYPE_KEY,2,"STOREDIST",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define GEORADIUSBYMEMBER_RO_History NULL


#define GEORADIUSBYMEMBER_RO_tips NULL


struct redisCommandArg GEORADIUSBYMEMBER_RO_unit_Subargs[] = {
{"m",ARG_TYPE_PURE_TOKEN,-1,"M",NULL,NULL,CMD_ARG_NONE},
{"km",ARG_TYPE_PURE_TOKEN,-1,"KM",NULL,NULL,CMD_ARG_NONE},
{"ft",ARG_TYPE_PURE_TOKEN,-1,"FT",NULL,NULL,CMD_ARG_NONE},
{"mi",ARG_TYPE_PURE_TOKEN,-1,"MI",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEORADIUSBYMEMBER_RO_count_Subargs[] = {
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_NONE},
{"any",ARG_TYPE_PURE_TOKEN,-1,"ANY",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg GEORADIUSBYMEMBER_RO_order_Subargs[] = {
{"asc",ARG_TYPE_PURE_TOKEN,-1,"ASC",NULL,NULL,CMD_ARG_NONE},
{"desc",ARG_TYPE_PURE_TOKEN,-1,"DESC",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEORADIUSBYMEMBER_RO_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"radius",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"unit",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=GEORADIUSBYMEMBER_RO_unit_Subargs},
{"withcoord",ARG_TYPE_PURE_TOKEN,-1,"WITHCOORD",NULL,NULL,CMD_ARG_OPTIONAL},
{"withdist",ARG_TYPE_PURE_TOKEN,-1,"WITHDIST",NULL,NULL,CMD_ARG_OPTIONAL},
{"withhash",ARG_TYPE_PURE_TOKEN,-1,"WITHHASH",NULL,NULL,CMD_ARG_OPTIONAL},
{"count",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEORADIUSBYMEMBER_RO_count_Subargs},
{"order",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEORADIUSBYMEMBER_RO_order_Subargs},
{0}
};




#define GEORADIUS_RO_History NULL


#define GEORADIUS_RO_tips NULL


struct redisCommandArg GEORADIUS_RO_unit_Subargs[] = {
{"m",ARG_TYPE_PURE_TOKEN,-1,"M",NULL,NULL,CMD_ARG_NONE},
{"km",ARG_TYPE_PURE_TOKEN,-1,"KM",NULL,NULL,CMD_ARG_NONE},
{"ft",ARG_TYPE_PURE_TOKEN,-1,"FT",NULL,NULL,CMD_ARG_NONE},
{"mi",ARG_TYPE_PURE_TOKEN,-1,"MI",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEORADIUS_RO_count_Subargs[] = {
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_NONE},
{"any",ARG_TYPE_PURE_TOKEN,-1,"ANY",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg GEORADIUS_RO_order_Subargs[] = {
{"asc",ARG_TYPE_PURE_TOKEN,-1,"ASC",NULL,NULL,CMD_ARG_NONE},
{"desc",ARG_TYPE_PURE_TOKEN,-1,"DESC",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEORADIUS_RO_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"longitude",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"latitude",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"radius",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"unit",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=GEORADIUS_RO_unit_Subargs},
{"withcoord",ARG_TYPE_PURE_TOKEN,-1,"WITHCOORD",NULL,NULL,CMD_ARG_OPTIONAL},
{"withdist",ARG_TYPE_PURE_TOKEN,-1,"WITHDIST",NULL,NULL,CMD_ARG_OPTIONAL},
{"withhash",ARG_TYPE_PURE_TOKEN,-1,"WITHHASH",NULL,NULL,CMD_ARG_OPTIONAL},
{"count",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEORADIUS_RO_count_Subargs},
{"order",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEORADIUS_RO_order_Subargs},
{0}
};




#define GEOSEARCH_History NULL


#define GEOSEARCH_tips NULL


struct redisCommandArg GEOSEARCH_longitude_latitude_Subargs[] = {
{"longitude",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"latitude",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEOSEARCH_circle_unit_Subargs[] = {
{"m",ARG_TYPE_PURE_TOKEN,-1,"M",NULL,NULL,CMD_ARG_NONE},
{"km",ARG_TYPE_PURE_TOKEN,-1,"KM",NULL,NULL,CMD_ARG_NONE},
{"ft",ARG_TYPE_PURE_TOKEN,-1,"FT",NULL,NULL,CMD_ARG_NONE},
{"mi",ARG_TYPE_PURE_TOKEN,-1,"MI",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEOSEARCH_circle_Subargs[] = {
{"radius",ARG_TYPE_DOUBLE,-1,"BYRADIUS",NULL,NULL,CMD_ARG_NONE},
{"unit",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=GEOSEARCH_circle_unit_Subargs},
{0}
};


struct redisCommandArg GEOSEARCH_box_unit_Subargs[] = {
{"m",ARG_TYPE_PURE_TOKEN,-1,"M",NULL,NULL,CMD_ARG_NONE},
{"km",ARG_TYPE_PURE_TOKEN,-1,"KM",NULL,NULL,CMD_ARG_NONE},
{"ft",ARG_TYPE_PURE_TOKEN,-1,"FT",NULL,NULL,CMD_ARG_NONE},
{"mi",ARG_TYPE_PURE_TOKEN,-1,"MI",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEOSEARCH_box_Subargs[] = {
{"width",ARG_TYPE_DOUBLE,-1,"BYBOX",NULL,NULL,CMD_ARG_NONE},
{"height",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"unit",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=GEOSEARCH_box_unit_Subargs},
{0}
};


struct redisCommandArg GEOSEARCH_order_Subargs[] = {
{"asc",ARG_TYPE_PURE_TOKEN,-1,"ASC",NULL,NULL,CMD_ARG_NONE},
{"desc",ARG_TYPE_PURE_TOKEN,-1,"DESC",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEOSEARCH_count_Subargs[] = {
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_NONE},
{"any",ARG_TYPE_PURE_TOKEN,-1,"ANY",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg GEOSEARCH_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,"FROMMEMBER",NULL,NULL,CMD_ARG_OPTIONAL},
{"longitude_latitude",ARG_TYPE_BLOCK,-1,"FROMLONLAT",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEOSEARCH_longitude_latitude_Subargs},
{"circle",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEOSEARCH_circle_Subargs},
{"box",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEOSEARCH_box_Subargs},
{"order",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEOSEARCH_order_Subargs},
{"count",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEOSEARCH_count_Subargs},
{"withcoord",ARG_TYPE_PURE_TOKEN,-1,"WITHCOORD",NULL,NULL,CMD_ARG_OPTIONAL},
{"withdist",ARG_TYPE_PURE_TOKEN,-1,"WITHDIST",NULL,NULL,CMD_ARG_OPTIONAL},
{"withhash",ARG_TYPE_PURE_TOKEN,-1,"WITHHASH",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define GEOSEARCHSTORE_History NULL


#define GEOSEARCHSTORE_tips NULL


struct redisCommandArg GEOSEARCHSTORE_longitude_latitude_Subargs[] = {
{"longitude",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"latitude",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEOSEARCHSTORE_circle_unit_Subargs[] = {
{"m",ARG_TYPE_PURE_TOKEN,-1,"M",NULL,NULL,CMD_ARG_NONE},
{"km",ARG_TYPE_PURE_TOKEN,-1,"KM",NULL,NULL,CMD_ARG_NONE},
{"ft",ARG_TYPE_PURE_TOKEN,-1,"FT",NULL,NULL,CMD_ARG_NONE},
{"mi",ARG_TYPE_PURE_TOKEN,-1,"MI",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEOSEARCHSTORE_circle_Subargs[] = {
{"radius",ARG_TYPE_DOUBLE,-1,"BYRADIUS",NULL,NULL,CMD_ARG_NONE},
{"unit",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=GEOSEARCHSTORE_circle_unit_Subargs},
{0}
};


struct redisCommandArg GEOSEARCHSTORE_box_unit_Subargs[] = {
{"m",ARG_TYPE_PURE_TOKEN,-1,"M",NULL,NULL,CMD_ARG_NONE},
{"km",ARG_TYPE_PURE_TOKEN,-1,"KM",NULL,NULL,CMD_ARG_NONE},
{"ft",ARG_TYPE_PURE_TOKEN,-1,"FT",NULL,NULL,CMD_ARG_NONE},
{"mi",ARG_TYPE_PURE_TOKEN,-1,"MI",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEOSEARCHSTORE_box_Subargs[] = {
{"width",ARG_TYPE_DOUBLE,-1,"BYBOX",NULL,NULL,CMD_ARG_NONE},
{"height",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"unit",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=GEOSEARCHSTORE_box_unit_Subargs},
{0}
};


struct redisCommandArg GEOSEARCHSTORE_order_Subargs[] = {
{"asc",ARG_TYPE_PURE_TOKEN,-1,"ASC",NULL,NULL,CMD_ARG_NONE},
{"desc",ARG_TYPE_PURE_TOKEN,-1,"DESC",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GEOSEARCHSTORE_count_Subargs[] = {
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_NONE},
{"any",ARG_TYPE_PURE_TOKEN,-1,"ANY",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg GEOSEARCHSTORE_Args[] = {
{"destination",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"source",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,"FROMMEMBER",NULL,NULL,CMD_ARG_OPTIONAL},
{"longitude_latitude",ARG_TYPE_BLOCK,-1,"FROMLONLAT",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEOSEARCHSTORE_longitude_latitude_Subargs},
{"circle",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEOSEARCHSTORE_circle_Subargs},
{"box",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEOSEARCHSTORE_box_Subargs},
{"order",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEOSEARCHSTORE_order_Subargs},
{"count",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GEOSEARCHSTORE_count_Subargs},
{"storedist",ARG_TYPE_PURE_TOKEN,-1,"STOREDIST",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




commandHistory HDEL_History[] = {
{"2.4.0","Accepts multiple `field` arguments."},
{0}
};


#define HDEL_tips NULL


struct redisCommandArg HDEL_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"field",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define HEXISTS_History NULL


#define HEXISTS_tips NULL


struct redisCommandArg HEXISTS_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"field",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define HGET_History NULL


#define HGET_tips NULL


struct redisCommandArg HGET_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"field",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define HGETALL_History NULL


const char *HGETALL_tips[] = {
"nondeterministic_output_order",
NULL
};


struct redisCommandArg HGETALL_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define HINCRBY_History NULL


#define HINCRBY_tips NULL


struct redisCommandArg HINCRBY_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"field",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"increment",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define HINCRBYFLOAT_History NULL


#define HINCRBYFLOAT_tips NULL


struct redisCommandArg HINCRBYFLOAT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"field",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"increment",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define HKEYS_History NULL


const char *HKEYS_tips[] = {
"nondeterministic_output_order",
NULL
};


struct redisCommandArg HKEYS_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define HLEN_History NULL


#define HLEN_tips NULL


struct redisCommandArg HLEN_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define HMGET_History NULL


#define HMGET_tips NULL


struct redisCommandArg HMGET_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"field",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define HMSET_History NULL


#define HMSET_tips NULL


struct redisCommandArg HMSET_field_value_Subargs[] = {
{"field",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg HMSET_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"field_value",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE,.subargs=HMSET_field_value_Subargs},
{0}
};




#define HRANDFIELD_History NULL


const char *HRANDFIELD_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg HRANDFIELD_options_Subargs[] = {
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"withvalues",ARG_TYPE_PURE_TOKEN,-1,"WITHVALUES",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg HRANDFIELD_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"options",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=HRANDFIELD_options_Subargs},
{0}
};




#define HSCAN_History NULL


const char *HSCAN_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg HSCAN_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"cursor",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"pattern",ARG_TYPE_PATTERN,-1,"MATCH",NULL,NULL,CMD_ARG_OPTIONAL},
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




commandHistory HSET_History[] = {
{"4.0.0","Accepts multiple `field` and `value` arguments."},
{0}
};


#define HSET_tips NULL


struct redisCommandArg HSET_field_value_Subargs[] = {
{"field",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg HSET_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"field_value",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE,.subargs=HSET_field_value_Subargs},
{0}
};




#define HSETNX_History NULL


#define HSETNX_tips NULL


struct redisCommandArg HSETNX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"field",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define HSTRLEN_History NULL


#define HSTRLEN_tips NULL


struct redisCommandArg HSTRLEN_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"field",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define HVALS_History NULL


const char *HVALS_tips[] = {
"nondeterministic_output_order",
NULL
};


struct redisCommandArg HVALS_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define PFADD_History NULL


#define PFADD_tips NULL


struct redisCommandArg PFADD_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"element",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define PFCOUNT_History NULL


#define PFCOUNT_tips NULL


struct redisCommandArg PFCOUNT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define PFDEBUG_History NULL


#define PFDEBUG_tips NULL




#define PFMERGE_History NULL


#define PFMERGE_tips NULL


struct redisCommandArg PFMERGE_Args[] = {
{"destkey",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"sourcekey",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define PFSELFTEST_History NULL


#define PFSELFTEST_tips NULL




#define BLMOVE_History NULL


#define BLMOVE_tips NULL


struct redisCommandArg BLMOVE_wherefrom_Subargs[] = {
{"left",ARG_TYPE_PURE_TOKEN,-1,"LEFT",NULL,NULL,CMD_ARG_NONE},
{"right",ARG_TYPE_PURE_TOKEN,-1,"RIGHT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg BLMOVE_whereto_Subargs[] = {
{"left",ARG_TYPE_PURE_TOKEN,-1,"LEFT",NULL,NULL,CMD_ARG_NONE},
{"right",ARG_TYPE_PURE_TOKEN,-1,"RIGHT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg BLMOVE_Args[] = {
{"source",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"destination",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_NONE},
{"wherefrom",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=BLMOVE_wherefrom_Subargs},
{"whereto",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=BLMOVE_whereto_Subargs},
{"timeout",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define BLMPOP_History NULL


#define BLMPOP_tips NULL


struct redisCommandArg BLMPOP_where_Subargs[] = {
{"left",ARG_TYPE_PURE_TOKEN,-1,"LEFT",NULL,NULL,CMD_ARG_NONE},
{"right",ARG_TYPE_PURE_TOKEN,-1,"RIGHT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg BLMPOP_Args[] = {
{"timeout",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"where",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=BLMPOP_where_Subargs},
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




commandHistory BLPOP_History[] = {
{"6.0.0","`timeout` is interpreted as a double instead of an integer."},
{0}
};


#define BLPOP_tips NULL


struct redisCommandArg BLPOP_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"timeout",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory BRPOP_History[] = {
{"6.0.0","`timeout` is interpreted as a double instead of an integer."},
{0}
};


#define BRPOP_tips NULL


struct redisCommandArg BRPOP_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"timeout",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory BRPOPLPUSH_History[] = {
{"6.0.0","`timeout` is interpreted as a double instead of an integer."},
{0}
};


#define BRPOPLPUSH_tips NULL


struct redisCommandArg BRPOPLPUSH_Args[] = {
{"source",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"destination",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_NONE},
{"timeout",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define LINDEX_History NULL


#define LINDEX_tips NULL


struct redisCommandArg LINDEX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"index",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define LINSERT_History NULL


#define LINSERT_tips NULL


struct redisCommandArg LINSERT_where_Subargs[] = {
{"before",ARG_TYPE_PURE_TOKEN,-1,"BEFORE",NULL,NULL,CMD_ARG_NONE},
{"after",ARG_TYPE_PURE_TOKEN,-1,"AFTER",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg LINSERT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"where",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=LINSERT_where_Subargs},
{"pivot",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"element",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define LLEN_History NULL


#define LLEN_tips NULL


struct redisCommandArg LLEN_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define LMOVE_History NULL


#define LMOVE_tips NULL


struct redisCommandArg LMOVE_wherefrom_Subargs[] = {
{"left",ARG_TYPE_PURE_TOKEN,-1,"LEFT",NULL,NULL,CMD_ARG_NONE},
{"right",ARG_TYPE_PURE_TOKEN,-1,"RIGHT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg LMOVE_whereto_Subargs[] = {
{"left",ARG_TYPE_PURE_TOKEN,-1,"LEFT",NULL,NULL,CMD_ARG_NONE},
{"right",ARG_TYPE_PURE_TOKEN,-1,"RIGHT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg LMOVE_Args[] = {
{"source",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"destination",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_NONE},
{"wherefrom",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=LMOVE_wherefrom_Subargs},
{"whereto",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=LMOVE_whereto_Subargs},
{0}
};




#define LMPOP_History NULL


#define LMPOP_tips NULL


struct redisCommandArg LMPOP_where_Subargs[] = {
{"left",ARG_TYPE_PURE_TOKEN,-1,"LEFT",NULL,NULL,CMD_ARG_NONE},
{"right",ARG_TYPE_PURE_TOKEN,-1,"RIGHT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg LMPOP_Args[] = {
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"where",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=LMPOP_where_Subargs},
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




commandHistory LPOP_History[] = {
{"6.2.0","Added the `count` argument."},
{0}
};


#define LPOP_tips NULL


struct redisCommandArg LPOP_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,"6.2.0",CMD_ARG_OPTIONAL},
{0}
};




#define LPOS_History NULL


#define LPOS_tips NULL


struct redisCommandArg LPOS_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"element",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"rank",ARG_TYPE_INTEGER,-1,"RANK",NULL,NULL,CMD_ARG_OPTIONAL},
{"num-matches",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{"len",ARG_TYPE_INTEGER,-1,"MAXLEN",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




commandHistory LPUSH_History[] = {
{"2.4.0","Accepts multiple `element` arguments."},
{0}
};


#define LPUSH_tips NULL


struct redisCommandArg LPUSH_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"element",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




commandHistory LPUSHX_History[] = {
{"4.0.0","Accepts multiple `element` arguments."},
{0}
};


#define LPUSHX_tips NULL


struct redisCommandArg LPUSHX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"element",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define LRANGE_History NULL


#define LRANGE_tips NULL


struct redisCommandArg LRANGE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"start",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"stop",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define LREM_History NULL


#define LREM_tips NULL


struct redisCommandArg LREM_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"element",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define LSET_History NULL


#define LSET_tips NULL


struct redisCommandArg LSET_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"index",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"element",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define LTRIM_History NULL


#define LTRIM_tips NULL


struct redisCommandArg LTRIM_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"start",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"stop",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory RPOP_History[] = {
{"6.2.0","Added the `count` argument."},
{0}
};


#define RPOP_tips NULL


struct redisCommandArg RPOP_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,"6.2.0",CMD_ARG_OPTIONAL},
{0}
};




#define RPOPLPUSH_History NULL


#define RPOPLPUSH_tips NULL


struct redisCommandArg RPOPLPUSH_Args[] = {
{"source",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"destination",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory RPUSH_History[] = {
{"2.4.0","Accepts multiple `element` arguments."},
{0}
};


#define RPUSH_tips NULL


struct redisCommandArg RPUSH_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"element",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




commandHistory RPUSHX_History[] = {
{"4.0.0","Accepts multiple `element` arguments."},
{0}
};


#define RPUSHX_tips NULL


struct redisCommandArg RPUSHX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"element",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define PSUBSCRIBE_History NULL


#define PSUBSCRIBE_tips NULL


struct redisCommandArg PSUBSCRIBE_pattern_Subargs[] = {
{"pattern",ARG_TYPE_PATTERN,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg PSUBSCRIBE_Args[] = {
{"pattern",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE,.subargs=PSUBSCRIBE_pattern_Subargs},
{0}
};




#define PUBLISH_History NULL


#define PUBLISH_tips NULL


struct redisCommandArg PUBLISH_Args[] = {
{"channel",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"message",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define PUBSUB_CHANNELS_History NULL


#define PUBSUB_CHANNELS_tips NULL


struct redisCommandArg PUBSUB_CHANNELS_Args[] = {
{"pattern",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define PUBSUB_HELP_History NULL


#define PUBSUB_HELP_tips NULL




#define PUBSUB_NUMPAT_History NULL


#define PUBSUB_NUMPAT_tips NULL




#define PUBSUB_NUMSUB_History NULL


#define PUBSUB_NUMSUB_tips NULL


struct redisCommandArg PUBSUB_NUMSUB_Args[] = {
{"channel",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define PUBSUB_SHARDCHANNELS_History NULL


#define PUBSUB_SHARDCHANNELS_tips NULL


struct redisCommandArg PUBSUB_SHARDCHANNELS_Args[] = {
{"pattern",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define PUBSUB_SHARDNUMSUB_History NULL


#define PUBSUB_SHARDNUMSUB_tips NULL


struct redisCommand PUBSUB_Subcommands[] = {
{"channels","List active channels","O(N) where N is the number of active channels, and assuming constant time pattern matching (relatively short channels and patterns)","2.8.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,PUBSUB_CHANNELS_History,PUBSUB_CHANNELS_tips,pubsubCommand,-2,CMD_PUBSUB|CMD_LOADING|CMD_STALE,0,.args=PUBSUB_CHANNELS_Args},
{"help","Show helpful text about the different subcommands","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,PUBSUB_HELP_History,PUBSUB_HELP_tips,pubsubCommand,2,CMD_LOADING|CMD_STALE,0},
{"numpat","Get the count of unique patterns pattern subscriptions","O(1)","2.8.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,PUBSUB_NUMPAT_History,PUBSUB_NUMPAT_tips,pubsubCommand,2,CMD_PUBSUB|CMD_LOADING|CMD_STALE,0},
{"numsub","Get the count of subscribers for channels","O(N) for the NUMSUB subcommand, where N is the number of requested channels","2.8.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,PUBSUB_NUMSUB_History,PUBSUB_NUMSUB_tips,pubsubCommand,-2,CMD_PUBSUB|CMD_LOADING|CMD_STALE,0,.args=PUBSUB_NUMSUB_Args},
{"shardchannels","List active shard channels","O(N) where N is the number of active shard channels, and assuming constant time pattern matching (relatively short channels).","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,PUBSUB_SHARDCHANNELS_History,PUBSUB_SHARDCHANNELS_tips,pubsubCommand,-2,CMD_PUBSUB|CMD_LOADING|CMD_STALE,0,.args=PUBSUB_SHARDCHANNELS_Args},
{"shardnumsub","Get the count of subscribers for shard channels","O(N) for the SHARDNUMSUB subcommand, where N is the number of requested channels","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,PUBSUB_SHARDNUMSUB_History,PUBSUB_SHARDNUMSUB_tips,pubsubCommand,-2,CMD_PUBSUB|CMD_LOADING|CMD_STALE,0},
{0}
};




#define PUBSUB_History NULL


#define PUBSUB_tips NULL




#define PUNSUBSCRIBE_History NULL


#define PUNSUBSCRIBE_tips NULL


struct redisCommandArg PUNSUBSCRIBE_Args[] = {
{"pattern",ARG_TYPE_PATTERN,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define SPUBLISH_History NULL


#define SPUBLISH_tips NULL


struct redisCommandArg SPUBLISH_Args[] = {
{"channel",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"message",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define SSUBSCRIBE_History NULL


#define SSUBSCRIBE_tips NULL


struct redisCommandArg SSUBSCRIBE_Args[] = {
{"channel",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




commandHistory SUBSCRIBE_History[] = {
{"6.2.0","`RESET` can be called to exit subscribed state."},
{0}
};


#define SUBSCRIBE_tips NULL


struct redisCommandArg SUBSCRIBE_Args[] = {
{"channel",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define SUNSUBSCRIBE_History NULL


#define SUNSUBSCRIBE_tips NULL


struct redisCommandArg SUNSUBSCRIBE_Args[] = {
{"channel",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define UNSUBSCRIBE_History NULL


#define UNSUBSCRIBE_tips NULL


struct redisCommandArg UNSUBSCRIBE_Args[] = {
{"channel",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define EVAL_History NULL


#define EVAL_tips NULL


struct redisCommandArg EVAL_Args[] = {
{"script",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{"arg",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define EVALSHA_History NULL


#define EVALSHA_tips NULL


struct redisCommandArg EVALSHA_Args[] = {
{"sha1",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{"arg",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define EVALSHA_RO_History NULL


#define EVALSHA_RO_tips NULL


struct redisCommandArg EVALSHA_RO_Args[] = {
{"sha1",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"arg",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define EVAL_RO_History NULL


#define EVAL_RO_tips NULL


struct redisCommandArg EVAL_RO_Args[] = {
{"script",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"arg",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define FCALL_History NULL


#define FCALL_tips NULL


struct redisCommandArg FCALL_Args[] = {
{"function",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"arg",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define FCALL_RO_History NULL


#define FCALL_RO_tips NULL


struct redisCommandArg FCALL_RO_Args[] = {
{"function",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"arg",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define FUNCTION_DELETE_History NULL


const char *FUNCTION_DELETE_tips[] = {
"request_policy:all_shards",
"response_policy:all_succeeded",
NULL
};


struct redisCommandArg FUNCTION_DELETE_Args[] = {
{"function-name",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define FUNCTION_DUMP_History NULL


#define FUNCTION_DUMP_tips NULL




#define FUNCTION_FLUSH_History NULL


const char *FUNCTION_FLUSH_tips[] = {
"request_policy:all_shards",
"response_policy:all_succeeded",
NULL
};


struct redisCommandArg FUNCTION_FLUSH_async_Subargs[] = {
{"async",ARG_TYPE_PURE_TOKEN,-1,"ASYNC",NULL,NULL,CMD_ARG_NONE},
{"sync",ARG_TYPE_PURE_TOKEN,-1,"SYNC",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg FUNCTION_FLUSH_Args[] = {
{"async",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=FUNCTION_FLUSH_async_Subargs},
{0}
};




#define FUNCTION_HELP_History NULL


#define FUNCTION_HELP_tips NULL




#define FUNCTION_KILL_History NULL


const char *FUNCTION_KILL_tips[] = {
"request_policy:all_shards",
"response_policy:one_succeeded",
NULL
};




#define FUNCTION_LIST_History NULL


#define FUNCTION_LIST_tips NULL


struct redisCommandArg FUNCTION_LIST_Args[] = {
{"library-name-pattern",ARG_TYPE_STRING,-1,"LIBRARYNAME",NULL,NULL,CMD_ARG_OPTIONAL},
{"withcode",ARG_TYPE_PURE_TOKEN,-1,"WITHCODE",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define FUNCTION_LOAD_History NULL


const char *FUNCTION_LOAD_tips[] = {
"request_policy:all_shards",
"response_policy:all_succeeded",
NULL
};


struct redisCommandArg FUNCTION_LOAD_Args[] = {
{"engine-name",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"library-name",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"replace",ARG_TYPE_PURE_TOKEN,-1,"REPLACE",NULL,NULL,CMD_ARG_OPTIONAL},
{"library-description",ARG_TYPE_STRING,-1,"DESC",NULL,NULL,CMD_ARG_OPTIONAL},
{"function-code",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define FUNCTION_RESTORE_History NULL


const char *FUNCTION_RESTORE_tips[] = {
"request_policy:all_shards",
"response_policy:all_succeeded",
NULL
};


struct redisCommandArg FUNCTION_RESTORE_policy_Subargs[] = {
{"flush",ARG_TYPE_PURE_TOKEN,-1,"FLUSH",NULL,NULL,CMD_ARG_NONE},
{"append",ARG_TYPE_PURE_TOKEN,-1,"APPEND",NULL,NULL,CMD_ARG_NONE},
{"replace",ARG_TYPE_PURE_TOKEN,-1,"REPLACE",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg FUNCTION_RESTORE_Args[] = {
{"serialized-value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"policy",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=FUNCTION_RESTORE_policy_Subargs},
{0}
};




#define FUNCTION_STATS_History NULL


const char *FUNCTION_STATS_tips[] = {
"request_policy:all_shards",
"response_policy:one_succeeded",
NULL
};


struct redisCommand FUNCTION_Subcommands[] = {
{"delete","Delete a function by name","O(1)","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,FUNCTION_DELETE_History,FUNCTION_DELETE_tips,functionDeleteCommand,3,CMD_NOSCRIPT|CMD_WRITE,ACL_CATEGORY_SCRIPTING,.args=FUNCTION_DELETE_Args},
{"dump","Dump all functions into a serialized binary payload","O(N) where N is the number of functions","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,FUNCTION_DUMP_History,FUNCTION_DUMP_tips,functionDumpCommand,2,CMD_NOSCRIPT,ACL_CATEGORY_SCRIPTING},
{"flush","Deleting all functions","O(N) where N is the number of functions deleted","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,FUNCTION_FLUSH_History,FUNCTION_FLUSH_tips,functionFlushCommand,-2,CMD_NOSCRIPT|CMD_WRITE,ACL_CATEGORY_SCRIPTING,.args=FUNCTION_FLUSH_Args},
{"help","Show helpful text about the different subcommands","O(1)","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,FUNCTION_HELP_History,FUNCTION_HELP_tips,functionHelpCommand,2,CMD_LOADING|CMD_STALE,ACL_CATEGORY_SCRIPTING},
{"kill","Kill the function currently in execution.","O(1)","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,FUNCTION_KILL_History,FUNCTION_KILL_tips,functionKillCommand,2,CMD_NOSCRIPT|CMD_ALLOW_BUSY,ACL_CATEGORY_SCRIPTING},
{"list","List information about all the functions","O(N) where N is the number of functions","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,FUNCTION_LIST_History,FUNCTION_LIST_tips,functionListCommand,-2,CMD_NOSCRIPT,ACL_CATEGORY_SCRIPTING,.args=FUNCTION_LIST_Args},
{"load","Create a function with the given arguments (name, code, description)","O(1) (considering compilation time is redundant)","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,FUNCTION_LOAD_History,FUNCTION_LOAD_tips,functionLoadCommand,-5,CMD_NOSCRIPT|CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_SCRIPTING,.args=FUNCTION_LOAD_Args},
{"restore","Restore all the functions on the given payload","O(N) where N is the number of functions on the payload","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,FUNCTION_RESTORE_History,FUNCTION_RESTORE_tips,functionRestoreCommand,-3,CMD_NOSCRIPT|CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_SCRIPTING,.args=FUNCTION_RESTORE_Args},
{"stats","Return information about the function currently running (name, description, duration)","O(1)","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,FUNCTION_STATS_History,FUNCTION_STATS_tips,functionStatsCommand,2,CMD_NOSCRIPT|CMD_ALLOW_BUSY,ACL_CATEGORY_SCRIPTING},
{0}
};




#define FUNCTION_History NULL


#define FUNCTION_tips NULL




#define SCRIPT_DEBUG_History NULL


#define SCRIPT_DEBUG_tips NULL


struct redisCommandArg SCRIPT_DEBUG_mode_Subargs[] = {
{"yes",ARG_TYPE_PURE_TOKEN,-1,"YES",NULL,NULL,CMD_ARG_NONE},
{"sync",ARG_TYPE_PURE_TOKEN,-1,"SYNC",NULL,NULL,CMD_ARG_NONE},
{"no",ARG_TYPE_PURE_TOKEN,-1,"NO",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg SCRIPT_DEBUG_Args[] = {
{"mode",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=SCRIPT_DEBUG_mode_Subargs},
{0}
};




#define SCRIPT_EXISTS_History NULL


const char *SCRIPT_EXISTS_tips[] = {
"request_policy:all_shards",
"response_policy:agg_logical_and",
NULL
};


struct redisCommandArg SCRIPT_EXISTS_Args[] = {
{"sha1",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




commandHistory SCRIPT_FLUSH_History[] = {
{"6.2.0","Added the `ASYNC` and `SYNC` flushing mode modifiers, as well as the **lazyfree-lazy-user-flush** configuration directive."},
{0}
};


const char *SCRIPT_FLUSH_tips[] = {
"request_policy:all_nodes",
"response_policy:all_succeeded",
NULL
};


struct redisCommandArg SCRIPT_FLUSH_async_Subargs[] = {
{"async",ARG_TYPE_PURE_TOKEN,-1,"ASYNC",NULL,NULL,CMD_ARG_NONE},
{"sync",ARG_TYPE_PURE_TOKEN,-1,"SYNC",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg SCRIPT_FLUSH_Args[] = {
{"async",ARG_TYPE_ONEOF,-1,NULL,NULL,"6.2.0",CMD_ARG_OPTIONAL,.subargs=SCRIPT_FLUSH_async_Subargs},
{0}
};




#define SCRIPT_HELP_History NULL


#define SCRIPT_HELP_tips NULL




#define SCRIPT_KILL_History NULL


const char *SCRIPT_KILL_tips[] = {
"request_policy:all_shards",
"response_policy:one_succeeded",
NULL
};




#define SCRIPT_LOAD_History NULL


const char *SCRIPT_LOAD_tips[] = {
"request_policy:all_nodes",
"response_policy:all_succeeded",
NULL
};


struct redisCommandArg SCRIPT_LOAD_Args[] = {
{"script",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommand SCRIPT_Subcommands[] = {
{"debug","Set the debug mode for executed scripts.","O(1)","3.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,SCRIPT_DEBUG_History,SCRIPT_DEBUG_tips,scriptCommand,3,CMD_NOSCRIPT,ACL_CATEGORY_SCRIPTING,.args=SCRIPT_DEBUG_Args},
{"exists","Check existence of scripts in the script cache.","O(N) with N being the number of scripts to check (so checking a single script is an O(1) operation).","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,SCRIPT_EXISTS_History,SCRIPT_EXISTS_tips,scriptCommand,-3,CMD_NOSCRIPT,ACL_CATEGORY_SCRIPTING,.args=SCRIPT_EXISTS_Args},
{"flush","Remove all the scripts from the script cache.","O(N) with N being the number of scripts in cache","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,SCRIPT_FLUSH_History,SCRIPT_FLUSH_tips,scriptCommand,-2,CMD_NOSCRIPT,ACL_CATEGORY_SCRIPTING,.args=SCRIPT_FLUSH_Args},
{"help","Show helpful text about the different subcommands","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,SCRIPT_HELP_History,SCRIPT_HELP_tips,scriptCommand,2,CMD_LOADING|CMD_STALE,ACL_CATEGORY_SCRIPTING},
{"kill","Kill the script currently in execution.","O(1)","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,SCRIPT_KILL_History,SCRIPT_KILL_tips,scriptCommand,2,CMD_NOSCRIPT|CMD_ALLOW_BUSY,ACL_CATEGORY_SCRIPTING},
{"load","Load the specified Lua script into the script cache.","O(N) with N being the length in bytes of the script body.","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,SCRIPT_LOAD_History,SCRIPT_LOAD_tips,scriptCommand,3,CMD_NOSCRIPT|CMD_STALE,ACL_CATEGORY_SCRIPTING,.args=SCRIPT_LOAD_Args},
{0}
};




#define SCRIPT_History NULL


#define SCRIPT_tips NULL




#define SENTINEL_CKQUORUM_History NULL


#define SENTINEL_CKQUORUM_tips NULL




#define SENTINEL_CONFIG_History NULL


#define SENTINEL_CONFIG_tips NULL




#define SENTINEL_DEBUG_History NULL


#define SENTINEL_DEBUG_tips NULL




#define SENTINEL_FAILOVER_History NULL


#define SENTINEL_FAILOVER_tips NULL




#define SENTINEL_FLUSHCONFIG_History NULL


#define SENTINEL_FLUSHCONFIG_tips NULL




#define SENTINEL_GET_MASTER_ADDR_BY_NAME_History NULL


#define SENTINEL_GET_MASTER_ADDR_BY_NAME_tips NULL




#define SENTINEL_HELP_History NULL


#define SENTINEL_HELP_tips NULL




#define SENTINEL_INFO_CACHE_History NULL


#define SENTINEL_INFO_CACHE_tips NULL




#define SENTINEL_IS_MASTER_DOWN_BY_ADDR_History NULL


#define SENTINEL_IS_MASTER_DOWN_BY_ADDR_tips NULL




#define SENTINEL_MASTER_History NULL


#define SENTINEL_MASTER_tips NULL




#define SENTINEL_MASTERS_History NULL


#define SENTINEL_MASTERS_tips NULL




#define SENTINEL_MONITOR_History NULL


#define SENTINEL_MONITOR_tips NULL




#define SENTINEL_MYID_History NULL


#define SENTINEL_MYID_tips NULL




#define SENTINEL_PENDING_SCRIPTS_History NULL


#define SENTINEL_PENDING_SCRIPTS_tips NULL




#define SENTINEL_REMOVE_History NULL


#define SENTINEL_REMOVE_tips NULL




#define SENTINEL_REPLICAS_History NULL


#define SENTINEL_REPLICAS_tips NULL




#define SENTINEL_RESET_History NULL


#define SENTINEL_RESET_tips NULL




#define SENTINEL_SENTINELS_History NULL


#define SENTINEL_SENTINELS_tips NULL




#define SENTINEL_SET_History NULL


#define SENTINEL_SET_tips NULL




#define SENTINEL_SIMULATE_FAILURE_History NULL


#define SENTINEL_SIMULATE_FAILURE_tips NULL


struct redisCommand SENTINEL_Subcommands[] = {
{"ckquorum","Check for a Sentinel quorum",NULL,"2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_CKQUORUM_History,SENTINEL_CKQUORUM_tips,sentinelCommand,3,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"config","Configure Sentinel","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_CONFIG_History,SENTINEL_CONFIG_tips,sentinelCommand,-3,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"debug",NULL,NULL,"7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_DEBUG_History,SENTINEL_DEBUG_tips,sentinelCommand,-2,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"failover","Force a failover",NULL,"2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_FAILOVER_History,SENTINEL_FAILOVER_tips,sentinelCommand,3,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"flushconfig","Rewrite configuration file","O(1)","2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_FLUSHCONFIG_History,SENTINEL_FLUSHCONFIG_tips,sentinelCommand,2,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"get-master-addr-by-name","Get port and address of a master","O(1)","2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_GET_MASTER_ADDR_BY_NAME_History,SENTINEL_GET_MASTER_ADDR_BY_NAME_tips,sentinelCommand,3,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"help","Show helpful text about the different subcommands","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_HELP_History,SENTINEL_HELP_tips,sentinelCommand,2,CMD_LOADING|CMD_STALE|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"info-cache","Get cached INFO from the instances in the deployment","O(N) where N is the number of instances","3.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_INFO_CACHE_History,SENTINEL_INFO_CACHE_tips,sentinelCommand,3,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"is-master-down-by-addr","Check if a master is down","O(1)","2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_IS_MASTER_DOWN_BY_ADDR_History,SENTINEL_IS_MASTER_DOWN_BY_ADDR_tips,sentinelCommand,6,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"master","Shows the state of a master","O(1)","2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_MASTER_History,SENTINEL_MASTER_tips,sentinelCommand,3,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"masters","List the monitored masters","O(N) where N is the number of masters","2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_MASTERS_History,SENTINEL_MASTERS_tips,sentinelCommand,2,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"monitor","Start monitoring","O(1)","2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_MONITOR_History,SENTINEL_MONITOR_tips,sentinelCommand,6,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"myid","Get the Sentinel instance ID","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_MYID_History,SENTINEL_MYID_tips,sentinelCommand,2,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"pending-scripts","Get information about pending scripts",NULL,"2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_PENDING_SCRIPTS_History,SENTINEL_PENDING_SCRIPTS_tips,sentinelCommand,2,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"remove","Stop monitoring","O(1)","2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_REMOVE_History,SENTINEL_REMOVE_tips,sentinelCommand,3,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"replicas","List the monitored replicas","O(N) where N is the number of replicas","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_REPLICAS_History,SENTINEL_REPLICAS_tips,sentinelCommand,3,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"reset","Reset masters by name pattern","O(N) where N is the number of monitored masters","2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_RESET_History,SENTINEL_RESET_tips,sentinelCommand,3,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"sentinels","List the Sentinel instances","O(N) where N is the number of Sentinels","2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_SENTINELS_History,SENTINEL_SENTINELS_tips,sentinelCommand,3,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"set","Change the configuration of a monitored master","O(1)","2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_SET_History,SENTINEL_SET_tips,sentinelCommand,-5,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{"simulate-failure","Simulate failover scenarios",NULL,"3.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_SIMULATE_FAILURE_History,SENTINEL_SIMULATE_FAILURE_tips,sentinelCommand,-3,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0},
{0}
};




#define SENTINEL_History NULL


#define SENTINEL_tips NULL




#define ACL_CAT_History NULL


#define ACL_CAT_tips NULL


struct redisCommandArg ACL_CAT_Args[] = {
{"categoryname",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define ACL_DELUSER_History NULL


#define ACL_DELUSER_tips NULL


struct redisCommandArg ACL_DELUSER_Args[] = {
{"username",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define ACL_DRYRUN_History NULL


#define ACL_DRYRUN_tips NULL


struct redisCommandArg ACL_DRYRUN_Args[] = {
{"username",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"command",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"arg",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define ACL_GENPASS_History NULL


#define ACL_GENPASS_tips NULL


struct redisCommandArg ACL_GENPASS_Args[] = {
{"bits",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




commandHistory ACL_GETUSER_History[] = {
{"6.2.0","Added Pub/Sub channel patterns."},
{0}
};


#define ACL_GETUSER_tips NULL


struct redisCommandArg ACL_GETUSER_Args[] = {
{"username",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define ACL_HELP_History NULL


#define ACL_HELP_tips NULL




#define ACL_LIST_History NULL


#define ACL_LIST_tips NULL




#define ACL_LOAD_History NULL


#define ACL_LOAD_tips NULL




#define ACL_LOG_History NULL


#define ACL_LOG_tips NULL


struct redisCommandArg ACL_LOG_operation_Subargs[] = {
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"reset",ARG_TYPE_PURE_TOKEN,-1,"RESET",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ACL_LOG_Args[] = {
{"operation",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=ACL_LOG_operation_Subargs},
{0}
};




#define ACL_SAVE_History NULL


#define ACL_SAVE_tips NULL




commandHistory ACL_SETUSER_History[] = {
{"6.2.0","Added Pub/Sub channel patterns."},
{"7.0.0","Added selectors and key based permissions."},
{0}
};


#define ACL_SETUSER_tips NULL


struct redisCommandArg ACL_SETUSER_Args[] = {
{"username",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"rule",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define ACL_USERS_History NULL


#define ACL_USERS_tips NULL




#define ACL_WHOAMI_History NULL


#define ACL_WHOAMI_tips NULL


struct redisCommand ACL_Subcommands[] = {
{"cat","List the ACL categories or the commands inside a category","O(1) since the categories and commands are a fixed set.","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_CAT_History,ACL_CAT_tips,aclCommand,-2,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0,.args=ACL_CAT_Args},
{"deluser","Remove the specified ACL users and the associated rules","O(1) amortized time considering the typical user.","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_DELUSER_History,ACL_DELUSER_tips,aclCommand,-3,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0,.args=ACL_DELUSER_Args},
{"dryrun","Returns whether the user can execute the given command without executing the command.","O(1).","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_DRYRUN_History,ACL_DRYRUN_tips,aclCommand,-4,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0,.args=ACL_DRYRUN_Args},
{"genpass","Generate a pseudorandom secure password to use for ACL users","O(1)","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_GENPASS_History,ACL_GENPASS_tips,aclCommand,-2,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0,.args=ACL_GENPASS_Args},
{"getuser","Get the rules for a specific ACL user","O(N). Where N is the number of password, command and pattern rules that the user has.","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_GETUSER_History,ACL_GETUSER_tips,aclCommand,3,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0,.args=ACL_GETUSER_Args},
{"help","Show helpful text about the different subcommands","O(1)","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_HELP_History,ACL_HELP_tips,aclCommand,2,CMD_LOADING|CMD_STALE|CMD_SENTINEL,0},
{"list","List the current ACL rules in ACL config file format","O(N). Where N is the number of configured users.","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_LIST_History,ACL_LIST_tips,aclCommand,2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0},
{"load","Reload the ACLs from the configured ACL file","O(N). Where N is the number of configured users.","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_LOAD_History,ACL_LOAD_tips,aclCommand,2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0},
{"log","List latest events denied because of ACLs in place","O(N) with N being the number of entries shown.","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_LOG_History,ACL_LOG_tips,aclCommand,-2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0,.args=ACL_LOG_Args},
{"save","Save the current ACL rules in the configured ACL file","O(N). Where N is the number of configured users.","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_SAVE_History,ACL_SAVE_tips,aclCommand,2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0},
{"setuser","Modify or create the rules for a specific ACL user","O(N). Where N is the number of rules provided.","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_SETUSER_History,ACL_SETUSER_tips,aclCommand,-3,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0,.args=ACL_SETUSER_Args},
{"users","List the username of all the configured ACL rules","O(N). Where N is the number of configured users.","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_USERS_History,ACL_USERS_tips,aclCommand,2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0},
{"whoami","Return the name of the user associated to the current connection","O(1)","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_WHOAMI_History,ACL_WHOAMI_tips,aclCommand,2,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0},
{0}
};




#define ACL_History NULL


#define ACL_tips NULL




#define BGREWRITEAOF_History NULL


#define BGREWRITEAOF_tips NULL




commandHistory BGSAVE_History[] = {
{"3.2.2","Added the `SCHEDULE` option."},
{0}
};


#define BGSAVE_tips NULL


struct redisCommandArg BGSAVE_Args[] = {
{"schedule",ARG_TYPE_PURE_TOKEN,-1,"SCHEDULE",NULL,"3.2.2",CMD_ARG_OPTIONAL},
{0}
};




#define COMMAND_COUNT_History NULL


#define COMMAND_COUNT_tips NULL




#define COMMAND_DOCS_History NULL


#define COMMAND_DOCS_tips NULL


struct redisCommandArg COMMAND_DOCS_Args[] = {
{"command-name",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define COMMAND_GETKEYS_History NULL


#define COMMAND_GETKEYS_tips NULL




#define COMMAND_HELP_History NULL


#define COMMAND_HELP_tips NULL




commandHistory COMMAND_INFO_History[] = {
{"7.0.0","Allowed to be called with no argument to get info on all commands."},
{0}
};


#define COMMAND_INFO_tips NULL


struct redisCommandArg COMMAND_INFO_Args[] = {
{"command-name",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define COMMAND_LIST_History NULL


#define COMMAND_LIST_tips NULL


struct redisCommandArg COMMAND_LIST_filterby_Subargs[] = {
{"module-name",ARG_TYPE_STRING,-1,"MODULE",NULL,NULL,CMD_ARG_NONE},
{"category",ARG_TYPE_STRING,-1,"ACLCAT",NULL,NULL,CMD_ARG_NONE},
{"pattern",ARG_TYPE_PATTERN,-1,"PATTERN",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg COMMAND_LIST_Args[] = {
{"filterby",ARG_TYPE_ONEOF,-1,"FILTERBY",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=COMMAND_LIST_filterby_Subargs},
{0}
};


struct redisCommand COMMAND_Subcommands[] = {
{"count","Get total number of Redis commands","O(1)","2.8.13",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,COMMAND_COUNT_History,COMMAND_COUNT_tips,commandCountCommand,2,CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION},
{"docs","Get array of specific Redis command documentation","O(N) where N is the number of commands to look up","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,COMMAND_DOCS_History,COMMAND_DOCS_tips,commandDocsCommand,-2,CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION,.args=COMMAND_DOCS_Args},
{"getkeys","Extract keys given a full Redis command","O(N) where N is the number of arguments to the command","2.8.13",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,COMMAND_GETKEYS_History,COMMAND_GETKEYS_tips,commandGetKeysCommand,-4,CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION},
{"help","Show helpful text about the different subcommands","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,COMMAND_HELP_History,COMMAND_HELP_tips,commandHelpCommand,2,CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION},
{"info","Get array of specific Redis command details, or all when no argument is given.","O(N) where N is the number of commands to look up","2.8.13",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,COMMAND_INFO_History,COMMAND_INFO_tips,commandInfoCommand,-2,CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION,.args=COMMAND_INFO_Args},
{"list","Get an array of Redis command names","O(N) where N is the total number of Redis commands","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,COMMAND_LIST_History,COMMAND_LIST_tips,commandListCommand,-2,CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION,.args=COMMAND_LIST_Args},
{0}
};




#define COMMAND_History NULL


const char *COMMAND_tips[] = {
"nondeterministic_output",
NULL
};




commandHistory CONFIG_GET_History[] = {
{"7.0.0","Added the ability to pass multiple pattern parameters in one call"},
{0}
};


#define CONFIG_GET_tips NULL


struct redisCommandArg CONFIG_GET_parameter_Subargs[] = {
{"parameter",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CONFIG_GET_Args[] = {
{"parameter",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE,.subargs=CONFIG_GET_parameter_Subargs},
{0}
};




#define CONFIG_HELP_History NULL


#define CONFIG_HELP_tips NULL




#define CONFIG_RESETSTAT_History NULL


#define CONFIG_RESETSTAT_tips NULL




#define CONFIG_REWRITE_History NULL


#define CONFIG_REWRITE_tips NULL




commandHistory CONFIG_SET_History[] = {
{"7.0.0","Added the ability to set multiple parameters in one call."},
{0}
};


const char *CONFIG_SET_tips[] = {
"request_policy:all_nodes",
"response_policy:all_succeeded",
NULL
};


struct redisCommandArg CONFIG_SET_parameter_value_Subargs[] = {
{"parameter",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg CONFIG_SET_Args[] = {
{"parameter_value",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE,.subargs=CONFIG_SET_parameter_value_Subargs},
{0}
};


struct redisCommand CONFIG_Subcommands[] = {
{"get","Get the values of configuration parameters","O(N) when N is the number of configuration parameters provided","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,CONFIG_GET_History,CONFIG_GET_tips,configGetCommand,-3,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0,.args=CONFIG_GET_Args},
{"help","Show helpful text about the different subcommands","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,CONFIG_HELP_History,CONFIG_HELP_tips,configHelpCommand,2,CMD_LOADING|CMD_STALE,0},
{"resetstat","Reset the stats returned by INFO","O(1)","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,CONFIG_RESETSTAT_History,CONFIG_RESETSTAT_tips,configResetStatCommand,2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0},
{"rewrite","Rewrite the configuration file with the in memory configuration","O(1)","2.8.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,CONFIG_REWRITE_History,CONFIG_REWRITE_tips,configRewriteCommand,2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0},
{"set","Set configuration parameters to the given values","O(N) when N is the number of configuration parameters provided","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,CONFIG_SET_History,CONFIG_SET_tips,configSetCommand,-4,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0,.args=CONFIG_SET_Args},
{0}
};




#define CONFIG_History NULL


#define CONFIG_tips NULL




#define DBSIZE_History NULL


const char *DBSIZE_tips[] = {
"request_policy:all_shards",
"response_policy:agg_sum",
NULL
};




#define DEBUG_History NULL


#define DEBUG_tips NULL




#define FAILOVER_History NULL


#define FAILOVER_tips NULL


struct redisCommandArg FAILOVER_target_Subargs[] = {
{"host",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"port",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"force",ARG_TYPE_PURE_TOKEN,-1,"FORCE",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg FAILOVER_Args[] = {
{"target",ARG_TYPE_BLOCK,-1,"TO",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=FAILOVER_target_Subargs},
{"abort",ARG_TYPE_PURE_TOKEN,-1,"ABORT",NULL,NULL,CMD_ARG_OPTIONAL},
{"milliseconds",ARG_TYPE_INTEGER,-1,"TIMEOUT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




commandHistory FLUSHALL_History[] = {
{"4.0.0","Added the `ASYNC` flushing mode modifier."},
{"6.2.0","Added the `SYNC` flushing mode modifier and the **lazyfree-lazy-user-flush** configuration directive."},
{0}
};


const char *FLUSHALL_tips[] = {
"request_policy:all_shards",
"response_policy:all_succeeded",
NULL
};


struct redisCommandArg FLUSHALL_async_Subargs[] = {
{"async",ARG_TYPE_PURE_TOKEN,-1,"ASYNC",NULL,"4.0.0",CMD_ARG_NONE},
{"sync",ARG_TYPE_PURE_TOKEN,-1,"SYNC",NULL,"6.2.0",CMD_ARG_NONE},
{0}
};


struct redisCommandArg FLUSHALL_Args[] = {
{"async",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=FLUSHALL_async_Subargs},
{0}
};




commandHistory FLUSHDB_History[] = {
{"4.0.0","Added the `ASYNC` flushing mode modifier."},
{"6.2.0","Added the `SYNC` flushing mode modifier and the **lazyfree-lazy-user-flush** configuration directive."},
{0}
};


const char *FLUSHDB_tips[] = {
"request_policy:all_shards",
"response_policy:all_succeeded",
NULL
};


struct redisCommandArg FLUSHDB_async_Subargs[] = {
{"async",ARG_TYPE_PURE_TOKEN,-1,"ASYNC",NULL,"4.0.0",CMD_ARG_NONE},
{"sync",ARG_TYPE_PURE_TOKEN,-1,"SYNC",NULL,"6.2.0",CMD_ARG_NONE},
{0}
};


struct redisCommandArg FLUSHDB_Args[] = {
{"async",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=FLUSHDB_async_Subargs},
{0}
};




#define INFO_History NULL


const char *INFO_tips[] = {
"nondeterministic_output",
"request_policy:all_shards",
"response_policy:special",
NULL
};


struct redisCommandArg INFO_Args[] = {
{"section",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define LASTSAVE_History NULL


const char *LASTSAVE_tips[] = {
"nondeterministic_output",
NULL
};




#define LATENCY_DOCTOR_History NULL


#define LATENCY_DOCTOR_tips NULL




#define LATENCY_GRAPH_History NULL


#define LATENCY_GRAPH_tips NULL


struct redisCommandArg LATENCY_GRAPH_Args[] = {
{"event",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define LATENCY_HELP_History NULL


#define LATENCY_HELP_tips NULL




#define LATENCY_HISTOGRAM_History NULL


#define LATENCY_HISTOGRAM_tips NULL


struct redisCommandArg LATENCY_HISTOGRAM_Args[] = {
{"command",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define LATENCY_HISTORY_History NULL


#define LATENCY_HISTORY_tips NULL


struct redisCommandArg LATENCY_HISTORY_Args[] = {
{"event",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define LATENCY_LATEST_History NULL


#define LATENCY_LATEST_tips NULL




#define LATENCY_RESET_History NULL


#define LATENCY_RESET_tips NULL


struct redisCommandArg LATENCY_RESET_Args[] = {
{"event",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};


struct redisCommand LATENCY_Subcommands[] = {
{"doctor","Return a human readable latency analysis report.","O(1)","2.8.13",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,LATENCY_DOCTOR_History,LATENCY_DOCTOR_tips,latencyCommand,2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0},
{"graph","Return a latency graph for the event.","O(1)","2.8.13",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,LATENCY_GRAPH_History,LATENCY_GRAPH_tips,latencyCommand,3,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0,.args=LATENCY_GRAPH_Args},
{"help","Show helpful text about the different subcommands.","O(1)","2.8.13",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,LATENCY_HELP_History,LATENCY_HELP_tips,latencyCommand,2,CMD_LOADING|CMD_STALE,0},
{"histogram","Return the cumulative distribution of latencies of a subset of commands or all.","O(N) where N is the number of commands with latency information being retrieved.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,LATENCY_HISTOGRAM_History,LATENCY_HISTOGRAM_tips,latencyCommand,-2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0,.args=LATENCY_HISTOGRAM_Args},
{"history","Return timestamp-latency samples for the event.","O(1)","2.8.13",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,LATENCY_HISTORY_History,LATENCY_HISTORY_tips,latencyCommand,3,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0,.args=LATENCY_HISTORY_Args},
{"latest","Return the latest latency samples for all events.","O(1)","2.8.13",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,LATENCY_LATEST_History,LATENCY_LATEST_tips,latencyCommand,2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0},
{"reset","Reset latency data for one or more events.","O(1)","2.8.13",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,LATENCY_RESET_History,LATENCY_RESET_tips,latencyCommand,-2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0,.args=LATENCY_RESET_Args},
{0}
};




#define LATENCY_History NULL


#define LATENCY_tips NULL




#define LOLWUT_History NULL


#define LOLWUT_tips NULL


struct redisCommandArg LOLWUT_Args[] = {
{"version",ARG_TYPE_INTEGER,-1,"VERSION",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define MEMORY_DOCTOR_History NULL


const char *MEMORY_DOCTOR_tips[] = {
"nondeterministic_output",
NULL
};




#define MEMORY_HELP_History NULL


#define MEMORY_HELP_tips NULL




#define MEMORY_MALLOC_STATS_History NULL


const char *MEMORY_MALLOC_STATS_tips[] = {
"nondeterministic_output",
NULL
};




#define MEMORY_PURGE_History NULL


#define MEMORY_PURGE_tips NULL




#define MEMORY_STATS_History NULL


const char *MEMORY_STATS_tips[] = {
"nondeterministic_output",
NULL
};




#define MEMORY_USAGE_History NULL


#define MEMORY_USAGE_tips NULL


struct redisCommandArg MEMORY_USAGE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,"SAMPLES",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};


struct redisCommand MEMORY_Subcommands[] = {
{"doctor","Outputs memory problems report","O(1)","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MEMORY_DOCTOR_History,MEMORY_DOCTOR_tips,memoryCommand,2,0,0},
{"help","Show helpful text about the different subcommands","O(1)","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MEMORY_HELP_History,MEMORY_HELP_tips,memoryCommand,2,CMD_LOADING|CMD_STALE,0},
{"malloc-stats","Show allocator internal stats","Depends on how much memory is allocated, could be slow","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MEMORY_MALLOC_STATS_History,MEMORY_MALLOC_STATS_tips,memoryCommand,2,0,0},
{"purge","Ask the allocator to release memory","Depends on how much memory is allocated, could be slow","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MEMORY_PURGE_History,MEMORY_PURGE_tips,memoryCommand,2,0,0},
{"stats","Show memory usage details","O(1)","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MEMORY_STATS_History,MEMORY_STATS_tips,memoryCommand,2,0,0},
{"usage","Estimate the memory usage of a key","O(N) where N is the number of samples.","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MEMORY_USAGE_History,MEMORY_USAGE_tips,memoryCommand,-3,CMD_READONLY,0,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=MEMORY_USAGE_Args},
{0}
};




#define MEMORY_History NULL


#define MEMORY_tips NULL




#define MODULE_HELP_History NULL


#define MODULE_HELP_tips NULL




#define MODULE_LIST_History NULL


#define MODULE_LIST_tips NULL




#define MODULE_LOAD_History NULL


#define MODULE_LOAD_tips NULL


struct redisCommandArg MODULE_LOAD_Args[] = {
{"path",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"arg",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{0}
};




#define MODULE_UNLOAD_History NULL


#define MODULE_UNLOAD_tips NULL


struct redisCommandArg MODULE_UNLOAD_Args[] = {
{"name",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommand MODULE_Subcommands[] = {
{"help","Show helpful text about the different subcommands","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MODULE_HELP_History,MODULE_HELP_tips,moduleCommand,2,CMD_LOADING|CMD_STALE,0},
{"list","List all modules loaded by the server","O(N) where N is the number of loaded modules.","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MODULE_LIST_History,MODULE_LIST_tips,moduleCommand,2,CMD_ADMIN|CMD_NOSCRIPT,0},
{"load","Load a module","O(1)","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MODULE_LOAD_History,MODULE_LOAD_tips,moduleCommand,-3,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_NOSCRIPT|CMD_PROTECTED,0,.args=MODULE_LOAD_Args},
{"unload","Unload a module","O(1)","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MODULE_UNLOAD_History,MODULE_UNLOAD_tips,moduleCommand,3,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_NOSCRIPT|CMD_PROTECTED,0,.args=MODULE_UNLOAD_Args},
{0}
};




#define MODULE_History NULL


#define MODULE_tips NULL




commandHistory MONITOR_History[] = {
{"6.0.0","`AUTH` excluded from the command's output."},
{"6.2.0","`RESET` can be called to exit monitor mode."},
{"6.2.4","`AUTH`, `HELLO`, `EVAL`, `EVAL_RO`, `EVALSHA` and `EVALSHA_RO` included in the command's output."},
{0}
};


#define MONITOR_tips NULL




#define PSYNC_History NULL


#define PSYNC_tips NULL


struct redisCommandArg PSYNC_Args[] = {
{"replicationid",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define REPLCONF_History NULL


#define REPLCONF_tips NULL




#define REPLICAOF_History NULL


#define REPLICAOF_tips NULL


struct redisCommandArg REPLICAOF_Args[] = {
{"host",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"port",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define RESTORE_ASKING_History NULL


#define RESTORE_ASKING_tips NULL




#define ROLE_History NULL


#define ROLE_tips NULL




#define SAVE_History NULL


#define SAVE_tips NULL




commandHistory SHUTDOWN_History[] = {
{"7.0","Added the `NOW`, `FORCE` and `ABORT` modifiers. Introduced waiting for lagging replicas before exiting."},
{0}
};


#define SHUTDOWN_tips NULL


struct redisCommandArg SHUTDOWN_nosave_save_Subargs[] = {
{"nosave",ARG_TYPE_PURE_TOKEN,-1,"NOSAVE",NULL,NULL,CMD_ARG_NONE},
{"save",ARG_TYPE_PURE_TOKEN,-1,"SAVE",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg SHUTDOWN_Args[] = {
{"nosave_save",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=SHUTDOWN_nosave_save_Subargs},
{"now",ARG_TYPE_PURE_TOKEN,-1,"NOW",NULL,NULL,CMD_ARG_OPTIONAL},
{"force",ARG_TYPE_PURE_TOKEN,-1,"FORCE",NULL,NULL,CMD_ARG_OPTIONAL},
{"abort",ARG_TYPE_PURE_TOKEN,-1,"ABORT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define SLAVEOF_History NULL


#define SLAVEOF_tips NULL


struct redisCommandArg SLAVEOF_Args[] = {
{"host",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"port",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory SLOWLOG_GET_History[] = {
{"4.0.0","Added client IP address, port and name to the reply."},
{0}
};


const char *SLOWLOG_GET_tips[] = {
"request_policy:all_nodes",
"nondeterministic_output",
NULL
};


struct redisCommandArg SLOWLOG_GET_Args[] = {
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define SLOWLOG_HELP_History NULL


#define SLOWLOG_HELP_tips NULL




#define SLOWLOG_LEN_History NULL


const char *SLOWLOG_LEN_tips[] = {
"request_policy:all_nodes",
"response_policy:agg_sum",
"nondeterministic_output",
NULL
};




#define SLOWLOG_RESET_History NULL


const char *SLOWLOG_RESET_tips[] = {
"request_policy:all_nodes",
"response_policy:all_succeeded",
NULL
};


struct redisCommand SLOWLOG_Subcommands[] = {
{"get","Get the slow log's entries","O(N) where N is the number of entries returned","2.2.12",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,SLOWLOG_GET_History,SLOWLOG_GET_tips,slowlogCommand,-2,CMD_ADMIN|CMD_LOADING|CMD_STALE,0,.args=SLOWLOG_GET_Args},
{"help","Show helpful text about the different subcommands","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,SLOWLOG_HELP_History,SLOWLOG_HELP_tips,slowlogCommand,2,CMD_LOADING|CMD_STALE,0},
{"len","Get the slow log's length","O(1)","2.2.12",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,SLOWLOG_LEN_History,SLOWLOG_LEN_tips,slowlogCommand,2,CMD_ADMIN|CMD_LOADING|CMD_STALE,0},
{"reset","Clear all entries from the slow log","O(N) where N is the number of entries in the slowlog","2.2.12",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,SLOWLOG_RESET_History,SLOWLOG_RESET_tips,slowlogCommand,2,CMD_ADMIN|CMD_LOADING|CMD_STALE,0},
{0}
};




#define SLOWLOG_History NULL


#define SLOWLOG_tips NULL




#define SWAPDB_History NULL


#define SWAPDB_tips NULL


struct redisCommandArg SWAPDB_Args[] = {
{"index1",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"index2",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define SYNC_History NULL


#define SYNC_tips NULL




#define TIME_History NULL


const char *TIME_tips[] = {
"nondeterministic_output",
NULL
};




commandHistory SADD_History[] = {
{"2.4.0","Accepts multiple `member` arguments."},
{0}
};


#define SADD_tips NULL


struct redisCommandArg SADD_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define SCARD_History NULL


#define SCARD_tips NULL


struct redisCommandArg SCARD_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define SDIFF_History NULL


const char *SDIFF_tips[] = {
"nondeterministic_output_order",
NULL
};


struct redisCommandArg SDIFF_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define SDIFFSTORE_History NULL


#define SDIFFSTORE_tips NULL


struct redisCommandArg SDIFFSTORE_Args[] = {
{"destination",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define SINTER_History NULL


const char *SINTER_tips[] = {
"nondeterministic_output_order",
NULL
};


struct redisCommandArg SINTER_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define SINTERCARD_History NULL


#define SINTERCARD_tips NULL


struct redisCommandArg SINTERCARD_Args[] = {
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"limit",ARG_TYPE_INTEGER,-1,"LIMIT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define SINTERSTORE_History NULL


#define SINTERSTORE_tips NULL


struct redisCommandArg SINTERSTORE_Args[] = {
{"destination",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define SISMEMBER_History NULL


#define SISMEMBER_tips NULL


struct redisCommandArg SISMEMBER_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define SMEMBERS_History NULL


const char *SMEMBERS_tips[] = {
"nondeterministic_output_order",
NULL
};


struct redisCommandArg SMEMBERS_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define SMISMEMBER_History NULL


#define SMISMEMBER_tips NULL


struct redisCommandArg SMISMEMBER_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define SMOVE_History NULL


#define SMOVE_tips NULL


struct redisCommandArg SMOVE_Args[] = {
{"source",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"destination",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory SPOP_History[] = {
{"3.2.0","Added the `count` argument."},
{0}
};


const char *SPOP_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg SPOP_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,"3.2.0",CMD_ARG_OPTIONAL},
{0}
};




commandHistory SRANDMEMBER_History[] = {
{"2.6.0","Added the optional `count` argument."},
{0}
};


const char *SRANDMEMBER_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg SRANDMEMBER_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,"2.6.0",CMD_ARG_OPTIONAL},
{0}
};




commandHistory SREM_History[] = {
{"2.4.0","Accepts multiple `member` arguments."},
{0}
};


#define SREM_tips NULL


struct redisCommandArg SREM_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define SSCAN_History NULL


const char *SSCAN_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg SSCAN_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"cursor",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"pattern",ARG_TYPE_PATTERN,-1,"MATCH",NULL,NULL,CMD_ARG_OPTIONAL},
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define SUNION_History NULL


const char *SUNION_tips[] = {
"nondeterministic_output_order",
NULL
};


struct redisCommandArg SUNION_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define SUNIONSTORE_History NULL


#define SUNIONSTORE_tips NULL


struct redisCommandArg SUNIONSTORE_Args[] = {
{"destination",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define BZMPOP_History NULL


#define BZMPOP_tips NULL


struct redisCommandArg BZMPOP_where_Subargs[] = {
{"min",ARG_TYPE_PURE_TOKEN,-1,"MIN",NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_PURE_TOKEN,-1,"MAX",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg BZMPOP_Args[] = {
{"timeout",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"where",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=BZMPOP_where_Subargs},
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




commandHistory BZPOPMAX_History[] = {
{"6.0.0","`timeout` is interpreted as a double instead of an integer."},
{0}
};


#define BZPOPMAX_tips NULL


struct redisCommandArg BZPOPMAX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"timeout",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory BZPOPMIN_History[] = {
{"6.0.0","`timeout` is interpreted as a double instead of an integer."},
{0}
};


#define BZPOPMIN_tips NULL


struct redisCommandArg BZPOPMIN_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"timeout",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory ZADD_History[] = {
{"2.4.0","Accepts multiple elements."},
{"3.0.2","Added the `XX`, `NX`, `CH` and `INCR` options."},
{"6.2.0","Added the `GT` and `LT` options."},
{0}
};


#define ZADD_tips NULL


struct redisCommandArg ZADD_condition_Subargs[] = {
{"nx",ARG_TYPE_PURE_TOKEN,-1,"NX",NULL,NULL,CMD_ARG_NONE},
{"xx",ARG_TYPE_PURE_TOKEN,-1,"XX",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZADD_comparison_Subargs[] = {
{"gt",ARG_TYPE_PURE_TOKEN,-1,"GT",NULL,NULL,CMD_ARG_NONE},
{"lt",ARG_TYPE_PURE_TOKEN,-1,"LT",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZADD_score_member_Subargs[] = {
{"score",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZADD_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"condition",ARG_TYPE_ONEOF,-1,NULL,NULL,"6.2.0",CMD_ARG_OPTIONAL,.subargs=ZADD_condition_Subargs},
{"comparison",ARG_TYPE_ONEOF,-1,NULL,NULL,"3.0.2",CMD_ARG_OPTIONAL,.subargs=ZADD_comparison_Subargs},
{"change",ARG_TYPE_PURE_TOKEN,-1,"CH",NULL,"3.0.2",CMD_ARG_OPTIONAL},
{"increment",ARG_TYPE_PURE_TOKEN,-1,"INCR",NULL,"3.0.2",CMD_ARG_OPTIONAL},
{"score_member",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE,.subargs=ZADD_score_member_Subargs},
{0}
};




#define ZCARD_History NULL


#define ZCARD_tips NULL


struct redisCommandArg ZCARD_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define ZCOUNT_History NULL


#define ZCOUNT_tips NULL


struct redisCommandArg ZCOUNT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define ZDIFF_History NULL


#define ZDIFF_tips NULL


struct redisCommandArg ZDIFF_Args[] = {
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"withscores",ARG_TYPE_PURE_TOKEN,-1,"WITHSCORES",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define ZDIFFSTORE_History NULL


#define ZDIFFSTORE_tips NULL


struct redisCommandArg ZDIFFSTORE_Args[] = {
{"destination",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define ZINCRBY_History NULL


#define ZINCRBY_tips NULL


struct redisCommandArg ZINCRBY_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"increment",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define ZINTER_History NULL


#define ZINTER_tips NULL


struct redisCommandArg ZINTER_aggregate_Subargs[] = {
{"sum",ARG_TYPE_PURE_TOKEN,-1,"SUM",NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_PURE_TOKEN,-1,"MIN",NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_PURE_TOKEN,-1,"MAX",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZINTER_Args[] = {
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"weight",ARG_TYPE_INTEGER,-1,"WEIGHTS",NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{"aggregate",ARG_TYPE_ONEOF,-1,"AGGREGATE",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=ZINTER_aggregate_Subargs},
{"withscores",ARG_TYPE_PURE_TOKEN,-1,"WITHSCORES",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define ZINTERCARD_History NULL


#define ZINTERCARD_tips NULL


struct redisCommandArg ZINTERCARD_Args[] = {
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"limit",ARG_TYPE_INTEGER,-1,"LIMIT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define ZINTERSTORE_History NULL


#define ZINTERSTORE_tips NULL


struct redisCommandArg ZINTERSTORE_aggregate_Subargs[] = {
{"sum",ARG_TYPE_PURE_TOKEN,-1,"SUM",NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_PURE_TOKEN,-1,"MIN",NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_PURE_TOKEN,-1,"MAX",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZINTERSTORE_Args[] = {
{"destination",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"weight",ARG_TYPE_INTEGER,-1,"WEIGHTS",NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{"aggregate",ARG_TYPE_ONEOF,-1,"AGGREGATE",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=ZINTERSTORE_aggregate_Subargs},
{0}
};




#define ZLEXCOUNT_History NULL


#define ZLEXCOUNT_tips NULL


struct redisCommandArg ZLEXCOUNT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define ZMPOP_History NULL


#define ZMPOP_tips NULL


struct redisCommandArg ZMPOP_where_Subargs[] = {
{"min",ARG_TYPE_PURE_TOKEN,-1,"MIN",NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_PURE_TOKEN,-1,"MAX",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZMPOP_Args[] = {
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"where",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=ZMPOP_where_Subargs},
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define ZMSCORE_History NULL


#define ZMSCORE_tips NULL


struct redisCommandArg ZMSCORE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define ZPOPMAX_History NULL


#define ZPOPMAX_tips NULL


struct redisCommandArg ZPOPMAX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define ZPOPMIN_History NULL


#define ZPOPMIN_tips NULL


struct redisCommandArg ZPOPMIN_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define ZRANDMEMBER_History NULL


const char *ZRANDMEMBER_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg ZRANDMEMBER_options_Subargs[] = {
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"withscores",ARG_TYPE_PURE_TOKEN,-1,"WITHSCORES",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg ZRANDMEMBER_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"options",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=ZRANDMEMBER_options_Subargs},
{0}
};




commandHistory ZRANGE_History[] = {
{"6.2.0","Added the `REV`, `BYSCORE`, `BYLEX` and `LIMIT` options."},
{0}
};


#define ZRANGE_tips NULL


struct redisCommandArg ZRANGE_sortby_Subargs[] = {
{"byscore",ARG_TYPE_PURE_TOKEN,-1,"BYSCORE",NULL,NULL,CMD_ARG_NONE},
{"bylex",ARG_TYPE_PURE_TOKEN,-1,"BYLEX",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZRANGE_offset_count_Subargs[] = {
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZRANGE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"sortby",ARG_TYPE_ONEOF,-1,NULL,NULL,"6.2.0",CMD_ARG_OPTIONAL,.subargs=ZRANGE_sortby_Subargs},
{"rev",ARG_TYPE_PURE_TOKEN,-1,"REV",NULL,"6.2.0",CMD_ARG_OPTIONAL},
{"offset_count",ARG_TYPE_BLOCK,-1,"LIMIT",NULL,"6.2.0",CMD_ARG_OPTIONAL,.subargs=ZRANGE_offset_count_Subargs},
{"withscores",ARG_TYPE_PURE_TOKEN,-1,"WITHSCORES",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define ZRANGEBYLEX_History NULL


#define ZRANGEBYLEX_tips NULL


struct redisCommandArg ZRANGEBYLEX_offset_count_Subargs[] = {
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZRANGEBYLEX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"offset_count",ARG_TYPE_BLOCK,-1,"LIMIT",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=ZRANGEBYLEX_offset_count_Subargs},
{0}
};




commandHistory ZRANGEBYSCORE_History[] = {
{"2.0.0","Added the `WITHSCORES` modifier."},
{0}
};


#define ZRANGEBYSCORE_tips NULL


struct redisCommandArg ZRANGEBYSCORE_offset_count_Subargs[] = {
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZRANGEBYSCORE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"withscores",ARG_TYPE_PURE_TOKEN,-1,"WITHSCORES",NULL,"2.0.0",CMD_ARG_OPTIONAL},
{"offset_count",ARG_TYPE_BLOCK,-1,"LIMIT",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=ZRANGEBYSCORE_offset_count_Subargs},
{0}
};




#define ZRANGESTORE_History NULL


#define ZRANGESTORE_tips NULL


struct redisCommandArg ZRANGESTORE_sortby_Subargs[] = {
{"byscore",ARG_TYPE_PURE_TOKEN,-1,"BYSCORE",NULL,NULL,CMD_ARG_NONE},
{"bylex",ARG_TYPE_PURE_TOKEN,-1,"BYLEX",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZRANGESTORE_offset_count_Subargs[] = {
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZRANGESTORE_Args[] = {
{"dst",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"src",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"sortby",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=ZRANGESTORE_sortby_Subargs},
{"rev",ARG_TYPE_PURE_TOKEN,-1,"REV",NULL,NULL,CMD_ARG_OPTIONAL},
{"offset_count",ARG_TYPE_BLOCK,-1,"LIMIT",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=ZRANGESTORE_offset_count_Subargs},
{0}
};




#define ZRANK_History NULL


#define ZRANK_tips NULL


struct redisCommandArg ZRANK_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory ZREM_History[] = {
{"2.4.0","Accepts multiple elements."},
{0}
};


#define ZREM_tips NULL


struct redisCommandArg ZREM_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define ZREMRANGEBYLEX_History NULL


#define ZREMRANGEBYLEX_tips NULL


struct redisCommandArg ZREMRANGEBYLEX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define ZREMRANGEBYRANK_History NULL


#define ZREMRANGEBYRANK_tips NULL


struct redisCommandArg ZREMRANGEBYRANK_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"start",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"stop",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define ZREMRANGEBYSCORE_History NULL


#define ZREMRANGEBYSCORE_tips NULL


struct redisCommandArg ZREMRANGEBYSCORE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define ZREVRANGE_History NULL


#define ZREVRANGE_tips NULL


struct redisCommandArg ZREVRANGE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"start",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"stop",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"withscores",ARG_TYPE_PURE_TOKEN,-1,"WITHSCORES",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define ZREVRANGEBYLEX_History NULL


#define ZREVRANGEBYLEX_tips NULL


struct redisCommandArg ZREVRANGEBYLEX_offset_count_Subargs[] = {
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZREVRANGEBYLEX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"offset_count",ARG_TYPE_BLOCK,-1,"LIMIT",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=ZREVRANGEBYLEX_offset_count_Subargs},
{0}
};




commandHistory ZREVRANGEBYSCORE_History[] = {
{"2.1.6","`min` and `max` can be exclusive."},
{0}
};


#define ZREVRANGEBYSCORE_tips NULL


struct redisCommandArg ZREVRANGEBYSCORE_offset_count_Subargs[] = {
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZREVRANGEBYSCORE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"withscores",ARG_TYPE_PURE_TOKEN,-1,"WITHSCORES",NULL,NULL,CMD_ARG_OPTIONAL},
{"offset_count",ARG_TYPE_BLOCK,-1,"LIMIT",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=ZREVRANGEBYSCORE_offset_count_Subargs},
{0}
};




#define ZREVRANK_History NULL


#define ZREVRANK_tips NULL


struct redisCommandArg ZREVRANK_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define ZSCAN_History NULL


const char *ZSCAN_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg ZSCAN_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"cursor",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"pattern",ARG_TYPE_PATTERN,-1,"MATCH",NULL,NULL,CMD_ARG_OPTIONAL},
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define ZSCORE_History NULL


#define ZSCORE_tips NULL


struct redisCommandArg ZSCORE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"member",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define ZUNION_History NULL


#define ZUNION_tips NULL


struct redisCommandArg ZUNION_aggregate_Subargs[] = {
{"sum",ARG_TYPE_PURE_TOKEN,-1,"SUM",NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_PURE_TOKEN,-1,"MIN",NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_PURE_TOKEN,-1,"MAX",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZUNION_Args[] = {
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"weight",ARG_TYPE_INTEGER,-1,"WEIGHTS",NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{"aggregate",ARG_TYPE_ONEOF,-1,"AGGREGATE",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=ZUNION_aggregate_Subargs},
{"withscores",ARG_TYPE_PURE_TOKEN,-1,"WITHSCORES",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define ZUNIONSTORE_History NULL


#define ZUNIONSTORE_tips NULL


struct redisCommandArg ZUNIONSTORE_aggregate_Subargs[] = {
{"sum",ARG_TYPE_PURE_TOKEN,-1,"SUM",NULL,NULL,CMD_ARG_NONE},
{"min",ARG_TYPE_PURE_TOKEN,-1,"MIN",NULL,NULL,CMD_ARG_NONE},
{"max",ARG_TYPE_PURE_TOKEN,-1,"MAX",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg ZUNIONSTORE_Args[] = {
{"destination",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"numkeys",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"key",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"weight",ARG_TYPE_INTEGER,-1,"WEIGHTS",NULL,NULL,CMD_ARG_OPTIONAL|CMD_ARG_MULTIPLE},
{"aggregate",ARG_TYPE_ONEOF,-1,"AGGREGATE",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=ZUNIONSTORE_aggregate_Subargs},
{0}
};




#define XACK_History NULL


#define XACK_tips NULL


struct redisCommandArg XACK_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"group",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




commandHistory XADD_History[] = {
{"6.2.0","Added the `NOMKSTREAM` option, `MINID` trimming strategy and the `LIMIT` option."},
{"7.0.0","Added support for the `<ms>-*` explicit ID form."},
{0}
};


const char *XADD_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg XADD_trim_strategy_Subargs[] = {
{"maxlen",ARG_TYPE_PURE_TOKEN,-1,"MAXLEN",NULL,NULL,CMD_ARG_NONE},
{"minid",ARG_TYPE_PURE_TOKEN,-1,"MINID",NULL,"6.2.0",CMD_ARG_NONE},
{0}
};


struct redisCommandArg XADD_trim_operator_Subargs[] = {
{"equal",ARG_TYPE_PURE_TOKEN,-1,"=",NULL,NULL,CMD_ARG_NONE},
{"approximately",ARG_TYPE_PURE_TOKEN,-1,"~",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg XADD_trim_Subargs[] = {
{"strategy",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=XADD_trim_strategy_Subargs},
{"operator",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=XADD_trim_operator_Subargs},
{"threshold",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,"LIMIT",NULL,"6.2.0",CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg XADD_id_or_auto_Subargs[] = {
{"auto_id",ARG_TYPE_PURE_TOKEN,-1,"*",NULL,NULL,CMD_ARG_NONE},
{"id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg XADD_field_value_Subargs[] = {
{"field",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg XADD_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"nomkstream",ARG_TYPE_PURE_TOKEN,-1,"NOMKSTREAM",NULL,"6.2.0",CMD_ARG_OPTIONAL},
{"trim",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=XADD_trim_Subargs},
{"id_or_auto",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=XADD_id_or_auto_Subargs},
{"field_value",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE,.subargs=XADD_field_value_Subargs},
{0}
};




#define XAUTOCLAIM_History NULL


const char *XAUTOCLAIM_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg XAUTOCLAIM_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"group",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"consumer",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"min-idle-time",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"start",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{"justid",ARG_TYPE_PURE_TOKEN,-1,"JUSTID",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define XCLAIM_History NULL


const char *XCLAIM_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg XCLAIM_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"group",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"consumer",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"min-idle-time",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"ms",ARG_TYPE_INTEGER,-1,"IDLE",NULL,NULL,CMD_ARG_OPTIONAL},
{"ms-unix-time",ARG_TYPE_INTEGER,-1,"TIME",NULL,NULL,CMD_ARG_OPTIONAL},
{"count",ARG_TYPE_INTEGER,-1,"RETRYCOUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{"force",ARG_TYPE_PURE_TOKEN,-1,"FORCE",NULL,NULL,CMD_ARG_OPTIONAL},
{"justid",ARG_TYPE_PURE_TOKEN,-1,"JUSTID",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define XDEL_History NULL


#define XDEL_tips NULL


struct redisCommandArg XDEL_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define XGROUP_CREATE_History NULL


#define XGROUP_CREATE_tips NULL


struct redisCommandArg XGROUP_CREATE_id_Subargs[] = {
{"id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"new_id",ARG_TYPE_PURE_TOKEN,-1,"$",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg XGROUP_CREATE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"groupname",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"id",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=XGROUP_CREATE_id_Subargs},
{"mkstream",ARG_TYPE_PURE_TOKEN,-1,"MKSTREAM",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define XGROUP_CREATECONSUMER_History NULL


#define XGROUP_CREATECONSUMER_tips NULL


struct redisCommandArg XGROUP_CREATECONSUMER_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"groupname",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"consumername",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define XGROUP_DELCONSUMER_History NULL


#define XGROUP_DELCONSUMER_tips NULL


struct redisCommandArg XGROUP_DELCONSUMER_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"groupname",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"consumername",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define XGROUP_DESTROY_History NULL


#define XGROUP_DESTROY_tips NULL


struct redisCommandArg XGROUP_DESTROY_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"groupname",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define XGROUP_HELP_History NULL


#define XGROUP_HELP_tips NULL




#define XGROUP_SETID_History NULL


#define XGROUP_SETID_tips NULL


struct redisCommandArg XGROUP_SETID_id_Subargs[] = {
{"id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"new_id",ARG_TYPE_PURE_TOKEN,-1,"$",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg XGROUP_SETID_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"groupname",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"id",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=XGROUP_SETID_id_Subargs},
{0}
};


struct redisCommand XGROUP_Subcommands[] = {
{"create","Create a consumer group.","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XGROUP_CREATE_History,XGROUP_CREATE_tips,xgroupCommand,-5,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_STREAM,{{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XGROUP_CREATE_Args},
{"createconsumer","Create a consumer in a consumer group.","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XGROUP_CREATECONSUMER_History,XGROUP_CREATECONSUMER_tips,xgroupCommand,5,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_STREAM,{{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XGROUP_CREATECONSUMER_Args},
{"delconsumer","Delete a consumer from a consumer group.","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XGROUP_DELCONSUMER_History,XGROUP_DELCONSUMER_tips,xgroupCommand,5,CMD_WRITE,ACL_CATEGORY_STREAM,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XGROUP_DELCONSUMER_Args},
{"destroy","Destroy a consumer group.","O(N) where N is the number of entries in the group's pending entries list (PEL).","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XGROUP_DESTROY_History,XGROUP_DESTROY_tips,xgroupCommand,4,CMD_WRITE,ACL_CATEGORY_STREAM,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XGROUP_DESTROY_Args},
{"help","Show helpful text about the different subcommands","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XGROUP_HELP_History,XGROUP_HELP_tips,xgroupCommand,2,CMD_LOADING|CMD_STALE,ACL_CATEGORY_STREAM},
{"setid","Set a consumer group to an arbitrary last delivered ID value.","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XGROUP_SETID_History,XGROUP_SETID_tips,xgroupCommand,5,CMD_WRITE,ACL_CATEGORY_STREAM,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XGROUP_SETID_Args},
{0}
};




#define XGROUP_History NULL


#define XGROUP_tips NULL




#define XINFO_CONSUMERS_History NULL


const char *XINFO_CONSUMERS_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg XINFO_CONSUMERS_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"groupname",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define XINFO_GROUPS_History NULL


#define XINFO_GROUPS_tips NULL


struct redisCommandArg XINFO_GROUPS_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define XINFO_HELP_History NULL


#define XINFO_HELP_tips NULL




#define XINFO_STREAM_History NULL


#define XINFO_STREAM_tips NULL


struct redisCommandArg XINFO_STREAM_full_Subargs[] = {
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg XINFO_STREAM_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"full",ARG_TYPE_BLOCK,-1,"FULL",NULL,NULL,CMD_ARG_OPTIONAL,.subargs=XINFO_STREAM_full_Subargs},
{0}
};


struct redisCommand XINFO_Subcommands[] = {
{"consumers","List the consumers in a consumer group","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XINFO_CONSUMERS_History,XINFO_CONSUMERS_tips,xinfoCommand,4,CMD_READONLY,ACL_CATEGORY_STREAM,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XINFO_CONSUMERS_Args},
{"groups","List the consumer groups of a stream","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XINFO_GROUPS_History,XINFO_GROUPS_tips,xinfoCommand,3,CMD_READONLY,ACL_CATEGORY_STREAM,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XINFO_GROUPS_Args},
{"help","Show helpful text about the different subcommands","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XINFO_HELP_History,XINFO_HELP_tips,xinfoCommand,2,CMD_LOADING|CMD_STALE,ACL_CATEGORY_STREAM},
{"stream","Get information about a stream","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XINFO_STREAM_History,XINFO_STREAM_tips,xinfoCommand,-3,CMD_READONLY,ACL_CATEGORY_STREAM,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XINFO_STREAM_Args},
{0}
};




#define XINFO_History NULL


#define XINFO_tips NULL




#define XLEN_History NULL


#define XLEN_tips NULL


struct redisCommandArg XLEN_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory XPENDING_History[] = {
{"6.2.0","Added the `IDLE` option and exclusive range intervals."},
{0}
};


const char *XPENDING_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg XPENDING_filters_Subargs[] = {
{"min-idle-time",ARG_TYPE_INTEGER,-1,"IDLE",NULL,"6.2.0",CMD_ARG_OPTIONAL},
{"start",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"end",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"consumer",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg XPENDING_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"group",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"filters",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=XPENDING_filters_Subargs},
{0}
};




commandHistory XRANGE_History[] = {
{"6.2.0","Added exclusive ranges."},
{0}
};


#define XRANGE_tips NULL


struct redisCommandArg XRANGE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"start",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"end",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define XREAD_History NULL


#define XREAD_tips NULL


struct redisCommandArg XREAD_streams_Subargs[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};


struct redisCommandArg XREAD_Args[] = {
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{"milliseconds",ARG_TYPE_INTEGER,-1,"BLOCK",NULL,NULL,CMD_ARG_OPTIONAL},
{"streams",ARG_TYPE_BLOCK,-1,"STREAMS",NULL,NULL,CMD_ARG_NONE,.subargs=XREAD_streams_Subargs},
{0}
};




#define XREADGROUP_History NULL


#define XREADGROUP_tips NULL


struct redisCommandArg XREADGROUP_group_consumer_Subargs[] = {
{"group",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"consumer",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg XREADGROUP_streams_Subargs[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{"id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};


struct redisCommandArg XREADGROUP_Args[] = {
{"group_consumer",ARG_TYPE_BLOCK,-1,"GROUP",NULL,NULL,CMD_ARG_NONE,.subargs=XREADGROUP_group_consumer_Subargs},
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{"milliseconds",ARG_TYPE_INTEGER,-1,"BLOCK",NULL,NULL,CMD_ARG_OPTIONAL},
{"noack",ARG_TYPE_PURE_TOKEN,-1,"NOACK",NULL,NULL,CMD_ARG_OPTIONAL},
{"streams",ARG_TYPE_BLOCK,-1,"STREAMS",NULL,NULL,CMD_ARG_NONE,.subargs=XREADGROUP_streams_Subargs},
{0}
};




commandHistory XREVRANGE_History[] = {
{"6.2.0","Added exclusive ranges."},
{0}
};


#define XREVRANGE_tips NULL


struct redisCommandArg XREVRANGE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"end",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"start",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,"COUNT",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define XSETID_History NULL


#define XSETID_tips NULL


struct redisCommandArg XSETID_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"last-id",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory XTRIM_History[] = {
{"6.2.0","Added the `MINID` trimming strategy and the `LIMIT` option."},
{0}
};


const char *XTRIM_tips[] = {
"nondeterministic_output",
NULL
};


struct redisCommandArg XTRIM_trim_strategy_Subargs[] = {
{"maxlen",ARG_TYPE_PURE_TOKEN,-1,"MAXLEN",NULL,NULL,CMD_ARG_NONE},
{"minid",ARG_TYPE_PURE_TOKEN,-1,"MINID",NULL,"6.2.0",CMD_ARG_NONE},
{0}
};


struct redisCommandArg XTRIM_trim_operator_Subargs[] = {
{"equal",ARG_TYPE_PURE_TOKEN,-1,"=",NULL,NULL,CMD_ARG_NONE},
{"approximately",ARG_TYPE_PURE_TOKEN,-1,"~",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg XTRIM_trim_Subargs[] = {
{"strategy",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=XTRIM_trim_strategy_Subargs},
{"operator",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=XTRIM_trim_operator_Subargs},
{"threshold",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"count",ARG_TYPE_INTEGER,-1,"LIMIT",NULL,"6.2.0",CMD_ARG_OPTIONAL},
{0}
};


struct redisCommandArg XTRIM_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"trim",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_NONE,.subargs=XTRIM_trim_Subargs},
{0}
};




#define APPEND_History NULL


#define APPEND_tips NULL


struct redisCommandArg APPEND_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define DECR_History NULL


#define DECR_tips NULL


struct redisCommandArg DECR_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define DECRBY_History NULL


#define DECRBY_tips NULL


struct redisCommandArg DECRBY_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"decrement",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define GET_History NULL


#define GET_tips NULL


struct redisCommandArg GET_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define GETDEL_History NULL


#define GETDEL_tips NULL


struct redisCommandArg GETDEL_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define GETEX_History NULL


#define GETEX_tips NULL


struct redisCommandArg GETEX_expiration_Subargs[] = {
{"seconds",ARG_TYPE_INTEGER,-1,"EX",NULL,NULL,CMD_ARG_NONE},
{"milliseconds",ARG_TYPE_INTEGER,-1,"PX",NULL,NULL,CMD_ARG_NONE},
{"unix-time",ARG_TYPE_INTEGER,-1,"EXAT",NULL,NULL,CMD_ARG_NONE},
{"unix-time",ARG_TYPE_INTEGER,-1,"PXAT",NULL,NULL,CMD_ARG_NONE},
{"persist",ARG_TYPE_PURE_TOKEN,-1,"PERSIST",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg GETEX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"expiration",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=GETEX_expiration_Subargs},
{0}
};




#define GETRANGE_History NULL


#define GETRANGE_tips NULL


struct redisCommandArg GETRANGE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"start",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"end",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define GETSET_History NULL


#define GETSET_tips NULL


struct redisCommandArg GETSET_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define INCR_History NULL


#define INCR_tips NULL


struct redisCommandArg INCR_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define INCRBY_History NULL


#define INCRBY_tips NULL


struct redisCommandArg INCRBY_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"increment",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define INCRBYFLOAT_History NULL


#define INCRBYFLOAT_tips NULL


struct redisCommandArg INCRBYFLOAT_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"increment",ARG_TYPE_DOUBLE,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define LCS_History NULL


#define LCS_tips NULL


struct redisCommandArg LCS_Args[] = {
{"key1",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"key2",ARG_TYPE_KEY,1,NULL,NULL,NULL,CMD_ARG_NONE},
{"len",ARG_TYPE_PURE_TOKEN,-1,"LEN",NULL,NULL,CMD_ARG_OPTIONAL},
{"idx",ARG_TYPE_PURE_TOKEN,-1,"IDX",NULL,NULL,CMD_ARG_OPTIONAL},
{"len",ARG_TYPE_INTEGER,-1,"MINMATCHLEN",NULL,NULL,CMD_ARG_OPTIONAL},
{"withmatchlen",ARG_TYPE_PURE_TOKEN,-1,"WITHMATCHLEN",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define MGET_History NULL


const char *MGET_tips[] = {
"request_policy:multi_shard",
NULL
};


struct redisCommandArg MGET_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};




#define MSET_History NULL


const char *MSET_tips[] = {
"request_policy:multi_shard",
"response_policy:all_succeeded",
NULL
};


struct redisCommandArg MSET_key_value_Subargs[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg MSET_Args[] = {
{"key_value",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE,.subargs=MSET_key_value_Subargs},
{0}
};




#define MSETNX_History NULL


#define MSETNX_tips NULL


struct redisCommandArg MSETNX_key_value_Subargs[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg MSETNX_Args[] = {
{"key_value",ARG_TYPE_BLOCK,-1,NULL,NULL,NULL,CMD_ARG_MULTIPLE,.subargs=MSETNX_key_value_Subargs},
{0}
};




#define PSETEX_History NULL


#define PSETEX_tips NULL


struct redisCommandArg PSETEX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"milliseconds",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




commandHistory SET_History[] = {
{"2.6.12","Added the `EX`, `PX`, `NX` and `XX` options."},
{"6.0.0","Added the `KEEPTTL` option."},
{"6.2.0","Added the `GET`, `EXAT` and `PXAT` option."},
{"7.0.0","Allowed the `NX` and `GET` options to be used together."},
{0}
};


#define SET_tips NULL


struct redisCommandArg SET_expiration_Subargs[] = {
{"seconds",ARG_TYPE_INTEGER,-1,"EX",NULL,"2.6.12",CMD_ARG_NONE},
{"milliseconds",ARG_TYPE_INTEGER,-1,"PX",NULL,"2.6.12",CMD_ARG_NONE},
{"unix-time-seconds",ARG_TYPE_UNIX_TIME,-1,"EXAT",NULL,"6.2.0",CMD_ARG_NONE},
{"unix-time-milliseconds",ARG_TYPE_UNIX_TIME,-1,"PXAT",NULL,"6.2.0",CMD_ARG_NONE},
{"keepttl",ARG_TYPE_PURE_TOKEN,-1,"KEEPTTL",NULL,"6.0.0",CMD_ARG_NONE},
{0}
};


struct redisCommandArg SET_condition_Subargs[] = {
{"nx",ARG_TYPE_PURE_TOKEN,-1,"NX",NULL,NULL,CMD_ARG_NONE},
{"xx",ARG_TYPE_PURE_TOKEN,-1,"XX",NULL,NULL,CMD_ARG_NONE},
{0}
};


struct redisCommandArg SET_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"expiration",ARG_TYPE_ONEOF,-1,NULL,NULL,NULL,CMD_ARG_OPTIONAL,.subargs=SET_expiration_Subargs},
{"condition",ARG_TYPE_ONEOF,-1,NULL,NULL,"2.6.12",CMD_ARG_OPTIONAL,.subargs=SET_condition_Subargs},
{"get",ARG_TYPE_PURE_TOKEN,-1,"GET",NULL,NULL,CMD_ARG_OPTIONAL},
{0}
};




#define SETEX_History NULL


#define SETEX_tips NULL


struct redisCommandArg SETEX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"seconds",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define SETNX_History NULL


#define SETNX_tips NULL


struct redisCommandArg SETNX_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define SETRANGE_History NULL


#define SETRANGE_tips NULL


struct redisCommandArg SETRANGE_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"offset",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"value",ARG_TYPE_STRING,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define STRLEN_History NULL


#define STRLEN_tips NULL


struct redisCommandArg STRLEN_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define SUBSTR_History NULL


#define SUBSTR_tips NULL


struct redisCommandArg SUBSTR_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_NONE},
{"start",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{"end",ARG_TYPE_INTEGER,-1,NULL,NULL,NULL,CMD_ARG_NONE},
{0}
};




#define DISCARD_History NULL


#define DISCARD_tips NULL




#define EXEC_History NULL


#define EXEC_tips NULL




#define MULTI_History NULL


#define MULTI_tips NULL




#define UNWATCH_History NULL


#define UNWATCH_tips NULL




#define WATCH_History NULL


#define WATCH_tips NULL


struct redisCommandArg WATCH_Args[] = {
{"key",ARG_TYPE_KEY,0,NULL,NULL,NULL,CMD_ARG_MULTIPLE},
{0}
};


struct redisCommand redisCommandTable[] = {

{"bitcount","Count set bits in a string","O(N)","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_BITMAP,BITCOUNT_History,BITCOUNT_tips,bitcountCommand,-2,CMD_READONLY,ACL_CATEGORY_BITMAP,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=BITCOUNT_Args},
{"bitfield","Perform arbitrary bitfield integer operations on strings","O(1) for each subcommand specified","3.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_BITMAP,BITFIELD_History,BITFIELD_tips,bitfieldCommand,-2,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_BITMAP,{{CMD_KEY_RW|CMD_KEY_UPDATE|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=BITFIELD_Args},
{"bitfield_ro","Perform arbitrary bitfield integer operations on strings. Read-only variant of BITFIELD","O(1) for each subcommand specified","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_BITMAP,BITFIELD_RO_History,BITFIELD_RO_tips,bitfieldroCommand,-2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_BITMAP,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=BITFIELD_RO_Args},
{"bitop","Perform bitwise operations between strings","O(N)","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_BITMAP,BITOP_History,BITOP_tips,bitopCommand,-4,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_BITMAP,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={3},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=BITOP_Args},
{"bitpos","Find first bit set or clear in a string","O(N)","2.8.7",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_BITMAP,BITPOS_History,BITPOS_tips,bitposCommand,-3,CMD_READONLY,ACL_CATEGORY_BITMAP,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=BITPOS_Args},
{"getbit","Returns the bit value at offset in the string value stored at key","O(1)","2.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_BITMAP,GETBIT_History,GETBIT_tips,getbitCommand,3,CMD_READONLY|CMD_FAST,ACL_CATEGORY_BITMAP,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GETBIT_Args},
{"setbit","Sets or clears the bit at offset in the string value stored at key","O(1)","2.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_BITMAP,SETBIT_History,SETBIT_tips,setbitCommand,4,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_BITMAP,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SETBIT_Args},

{"asking","Sent by cluster clients after an -ASK redirect","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,ASKING_History,ASKING_tips,askingCommand,1,CMD_FAST,ACL_CATEGORY_CONNECTION},
{"cluster","A container for cluster commands","Depends on subcommand.","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,CLUSTER_History,CLUSTER_tips,NULL,-2,0,0,.subcommands=CLUSTER_Subcommands},
{"readonly","Enables read queries for a connection to a cluster replica node","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,READONLY_History,READONLY_tips,readonlyCommand,1,CMD_FAST|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION},
{"readwrite","Disables read queries for a connection to a cluster replica node","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CLUSTER,READWRITE_History,READWRITE_tips,readwriteCommand,1,CMD_FAST|CMD_LOADING|CMD_STALE,ACL_CATEGORY_CONNECTION},

{"auth","Authenticate to the server","O(N) where N is the number of passwords defined for the user","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,AUTH_History,AUTH_tips,authCommand,-2,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_FAST|CMD_NO_AUTH|CMD_SENTINEL|CMD_ALLOW_BUSY,ACL_CATEGORY_CONNECTION,.args=AUTH_Args},
{"client","A container for client connection commands","Depends on subcommand.","2.4.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,CLIENT_History,CLIENT_tips,NULL,-2,CMD_SENTINEL,0,.subcommands=CLIENT_Subcommands},
{"echo","Echo the given string","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,ECHO_History,ECHO_tips,echoCommand,2,CMD_FAST,ACL_CATEGORY_CONNECTION,.args=ECHO_Args},
{"hello","Handshake with Redis","O(1)","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,HELLO_History,HELLO_tips,helloCommand,-1,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_FAST|CMD_NO_AUTH|CMD_SENTINEL|CMD_ALLOW_BUSY,ACL_CATEGORY_CONNECTION,.args=HELLO_Args},
{"ping","Ping the server","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,PING_History,PING_tips,pingCommand,-1,CMD_FAST|CMD_SENTINEL,ACL_CATEGORY_CONNECTION,.args=PING_Args},
{"quit","Close the connection","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,QUIT_History,QUIT_tips,quitCommand,-1,CMD_ALLOW_BUSY|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_FAST|CMD_NO_AUTH,ACL_CATEGORY_CONNECTION},
{"reset","Reset the connection","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,RESET_History,RESET_tips,resetCommand,1,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_FAST|CMD_NO_AUTH|CMD_ALLOW_BUSY,ACL_CATEGORY_CONNECTION},
{"select","Change the selected database for the current connection","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_CONNECTION,SELECT_History,SELECT_tips,selectCommand,2,CMD_LOADING|CMD_STALE|CMD_FAST,ACL_CATEGORY_CONNECTION,.args=SELECT_Args},

{"copy","Copy a key","O(N) worst case for collections, where N is the number of nested items. O(1) for string values.","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,COPY_History,COPY_tips,copyCommand,-3,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=COPY_Args},
{"del","Delete a key","O(N) where N is the number of keys that will be removed. When a key to remove holds a value other than a string, the individual complexity for this key is O(M) where M is the number of elements in the list, set, sorted set or hash. Removing a single key that holds a string value is O(1).","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,DEL_History,DEL_tips,delCommand,-2,CMD_WRITE,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RM|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=DEL_Args},
{"dump","Return a serialized version of the value stored at the specified key.","O(1) to access the key and additional O(N*M) to serialize it, where N is the number of Redis objects composing the value and M their average size. For small string values the time complexity is thus O(1)+O(1*M) where M is small, so simply O(1).","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,DUMP_History,DUMP_tips,dumpCommand,2,CMD_READONLY,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=DUMP_Args},
{"exists","Determine if a key exists","O(N) where N is the number of keys to check.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,EXISTS_History,EXISTS_tips,existsCommand,-2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=EXISTS_Args},
{"expire","Set a key's time to live in seconds","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,EXPIRE_History,EXPIRE_tips,expireCommand,-3,CMD_WRITE|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=EXPIRE_Args},
{"expireat","Set the expiration for a key as a UNIX timestamp","O(1)","1.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,EXPIREAT_History,EXPIREAT_tips,expireatCommand,-3,CMD_WRITE|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=EXPIREAT_Args},
{"expiretime","Get the expiration Unix timestamp for a key","O(1)","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,EXPIRETIME_History,EXPIRETIME_tips,expiretimeCommand,2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=EXPIRETIME_Args},
{"keys","Find all keys matching the given pattern","O(N) with N being the number of keys in the database, under the assumption that the key names in the database and the given pattern have limited length.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,KEYS_History,KEYS_tips,keysCommand,2,CMD_READONLY,ACL_CATEGORY_KEYSPACE|ACL_CATEGORY_DANGEROUS,.args=KEYS_Args},
{"migrate","Atomically transfer a key from a Redis instance to another one.","This command actually executes a DUMP+DEL in the source instance, and a RESTORE in the target instance. See the pages of these commands for time complexity. Also an O(N) data transfer between the two instances is performed.","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,MIGRATE_History,MIGRATE_tips,migrateCommand,-6,CMD_WRITE,ACL_CATEGORY_KEYSPACE|ACL_CATEGORY_DANGEROUS,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={3},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE|CMD_KEY_INCOMPLETE,KSPEC_BS_KEYWORD,.bs.keyword={"KEYS",-2},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},migrateGetKeys,.args=MIGRATE_Args},
{"move","Move a key to another database","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,MOVE_History,MOVE_tips,moveCommand,3,CMD_WRITE|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=MOVE_Args},
{"object","A container for object introspection commands","Depends on subcommand.","2.2.3",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,OBJECT_History,OBJECT_tips,NULL,-2,0,0,.subcommands=OBJECT_Subcommands},
{"persist","Remove the expiration from a key","O(1)","2.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,PERSIST_History,PERSIST_tips,persistCommand,2,CMD_WRITE|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=PERSIST_Args},
{"pexpire","Set a key's time to live in milliseconds","O(1)","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,PEXPIRE_History,PEXPIRE_tips,pexpireCommand,-3,CMD_WRITE|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=PEXPIRE_Args},
{"pexpireat","Set the expiration for a key as a UNIX timestamp specified in milliseconds","O(1)","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,PEXPIREAT_History,PEXPIREAT_tips,pexpireatCommand,-3,CMD_WRITE|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=PEXPIREAT_Args},
{"pexpiretime","Get the expiration Unix timestamp for a key in milliseconds","O(1)","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,PEXPIRETIME_History,PEXPIRETIME_tips,pexpiretimeCommand,2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=PEXPIRETIME_Args},
{"pttl","Get the time to live for a key in milliseconds","O(1)","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,PTTL_History,PTTL_tips,pttlCommand,2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=PTTL_Args},
{"randomkey","Return a random key from the keyspace","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,RANDOMKEY_History,RANDOMKEY_tips,randomkeyCommand,1,CMD_READONLY,ACL_CATEGORY_KEYSPACE},
{"rename","Rename a key","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,RENAME_History,RENAME_tips,renameCommand,3,CMD_WRITE,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=RENAME_Args},
{"renamenx","Rename a key, only if the new key does not exist","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,RENAMENX_History,RENAMENX_tips,renamenxCommand,3,CMD_WRITE|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_OW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=RENAMENX_Args},
{"restore","Create a key using the provided serialized value, previously obtained using DUMP.","O(1) to create the new key and additional O(N*M) to reconstruct the serialized value, where N is the number of Redis objects composing the value and M their average size. For small string values the time complexity is thus O(1)+O(1*M) where M is small, so simply O(1). However for sorted set values the complexity is O(N*M*log(N)) because inserting values into sorted sets is O(log(N)).","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,RESTORE_History,RESTORE_tips,restoreCommand,-4,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_KEYSPACE|ACL_CATEGORY_DANGEROUS,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=RESTORE_Args},
{"scan","Incrementally iterate the keys space","O(1) for every call. O(N) for a complete iteration, including enough command calls for the cursor to return back to 0. N is the number of elements inside the collection.","2.8.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,SCAN_History,SCAN_tips,scanCommand,-2,CMD_READONLY,ACL_CATEGORY_KEYSPACE,.args=SCAN_Args},
{"sort","Sort the elements in a list, set or sorted set","O(N+M*log(M)) where N is the number of elements in the list or set to sort, and M the number of returned elements. When the elements are not sorted, complexity is O(N).","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,SORT_History,SORT_tips,sortCommand,-2,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_SET|ACL_CATEGORY_SORTEDSET|ACL_CATEGORY_LIST|ACL_CATEGORY_DANGEROUS,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RO|CMD_KEY_ACCESS|CMD_KEY_INCOMPLETE,KSPEC_BS_UNKNOWN,{{0}},KSPEC_FK_UNKNOWN,{{0}}},{CMD_KEY_OW|CMD_KEY_UPDATE|CMD_KEY_INCOMPLETE,KSPEC_BS_UNKNOWN,{{0}},KSPEC_FK_UNKNOWN,{{0}}}},sortGetKeys,.args=SORT_Args},
{"sort_ro","Sort the elements in a list, set or sorted set. Read-only variant of SORT.","O(N+M*log(M)) where N is the number of elements in the list or set to sort, and M the number of returned elements. When the elements are not sorted, complexity is O(N).","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,SORT_RO_History,SORT_RO_tips,sortroCommand,-2,CMD_READONLY,ACL_CATEGORY_SET|ACL_CATEGORY_SORTEDSET|ACL_CATEGORY_LIST|ACL_CATEGORY_DANGEROUS,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RO|CMD_KEY_ACCESS|CMD_KEY_INCOMPLETE,KSPEC_BS_UNKNOWN,{{0}},KSPEC_FK_UNKNOWN,{{0}}}},sortROGetKeys,.args=SORT_RO_Args},
{"touch","Alters the last access time of a key(s). Returns the number of existing keys specified.","O(N) where N is the number of keys that will be touched.","3.2.1",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,TOUCH_History,TOUCH_tips,touchCommand,-2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=TOUCH_Args},
{"ttl","Get the time to live for a key in seconds","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,TTL_History,TTL_tips,ttlCommand,2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=TTL_Args},
{"type","Determine the type stored at key","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,TYPE_History,TYPE_tips,typeCommand,2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=TYPE_Args},
{"unlink","Delete a key asynchronously in another thread. Otherwise it is just as DEL, but non blocking.","O(1) for each key removed regardless of its size. Then the command does O(N) work in a different thread in order to reclaim memory, where N is the number of allocations the deleted objects where composed of.","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,UNLINK_History,UNLINK_tips,unlinkCommand,-2,CMD_WRITE|CMD_FAST,ACL_CATEGORY_KEYSPACE,{{CMD_KEY_RM|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=UNLINK_Args},
{"wait","Wait for the synchronous replication of all the write commands sent in the context of the current connection","O(1)","3.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GENERIC,WAIT_History,WAIT_tips,waitCommand,3,CMD_NOSCRIPT,ACL_CATEGORY_CONNECTION,.args=WAIT_Args},

{"geoadd","Add one or more geospatial items in the geospatial index represented using a sorted set","O(log(N)) for each item added, where N is the number of elements in the sorted set.","3.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GEO,GEOADD_History,GEOADD_tips,geoaddCommand,-5,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_GEO,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GEOADD_Args},
{"geodist","Returns the distance between two members of a geospatial index","O(log(N))","3.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GEO,GEODIST_History,GEODIST_tips,geodistCommand,-4,CMD_READONLY,ACL_CATEGORY_GEO,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GEODIST_Args},
{"geohash","Returns members of a geospatial index as standard geohash strings","O(log(N)) for each member requested, where N is the number of elements in the sorted set.","3.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GEO,GEOHASH_History,GEOHASH_tips,geohashCommand,-2,CMD_READONLY,ACL_CATEGORY_GEO,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GEOHASH_Args},
{"geopos","Returns longitude and latitude of members of a geospatial index","O(N) where N is the number of members requested.","3.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GEO,GEOPOS_History,GEOPOS_tips,geoposCommand,-2,CMD_READONLY,ACL_CATEGORY_GEO,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GEOPOS_Args},
{"georadius","Query a sorted set representing a geospatial index to fetch members matching a given maximum distance from a point","O(N+log(M)) where N is the number of elements inside the bounding box of the circular area delimited by center and radius and M is the number of items inside the index.","3.2.0",CMD_DOC_DEPRECATED,"`GEOSEARCH` and `GEOSEARCHSTORE` with the `BYRADIUS` argument","6.2.0",COMMAND_GROUP_GEO,GEORADIUS_History,GEORADIUS_tips,georadiusCommand,-6,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_GEO,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_KEYWORD,.bs.keyword={"STORE",6},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_KEYWORD,.bs.keyword={"STOREDIST",6},KSPEC_FK_RANGE,.fk.range={0,1,0}}},georadiusGetKeys,.args=GEORADIUS_Args},
{"georadiusbymember","Query a sorted set representing a geospatial index to fetch members matching a given maximum distance from a member","O(N+log(M)) where N is the number of elements inside the bounding box of the circular area delimited by center and radius and M is the number of items inside the index.","3.2.0",CMD_DOC_DEPRECATED,"`GEOSEARCH` and `GEOSEARCHSTORE` with the `BYRADIUS` and `FROMMEMBER` arguments","6.2.0",COMMAND_GROUP_GEO,GEORADIUSBYMEMBER_History,GEORADIUSBYMEMBER_tips,georadiusbymemberCommand,-5,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_GEO,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_KEYWORD,.bs.keyword={"STORE",5},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_KEYWORD,.bs.keyword={"STOREDIST",5},KSPEC_FK_RANGE,.fk.range={0,1,0}}},georadiusGetKeys,.args=GEORADIUSBYMEMBER_Args},
{"georadiusbymember_ro","A read-only variant for GEORADIUSBYMEMBER","O(N+log(M)) where N is the number of elements inside the bounding box of the circular area delimited by center and radius and M is the number of items inside the index.","3.2.10",CMD_DOC_DEPRECATED,"`GEOSEARCH` with the `BYRADIUS` and `FROMMEMBER` arguments","6.2.0",COMMAND_GROUP_GEO,GEORADIUSBYMEMBER_RO_History,GEORADIUSBYMEMBER_RO_tips,georadiusbymemberroCommand,-5,CMD_READONLY,ACL_CATEGORY_GEO,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GEORADIUSBYMEMBER_RO_Args},
{"georadius_ro","A read-only variant for GEORADIUS","O(N+log(M)) where N is the number of elements inside the bounding box of the circular area delimited by center and radius and M is the number of items inside the index.","3.2.10",CMD_DOC_DEPRECATED,"`GEOSEARCH` with the `BYRADIUS` argument","6.2.0",COMMAND_GROUP_GEO,GEORADIUS_RO_History,GEORADIUS_RO_tips,georadiusroCommand,-6,CMD_READONLY,ACL_CATEGORY_GEO,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GEORADIUS_RO_Args},
{"geosearch","Query a sorted set representing a geospatial index to fetch members inside an area of a box or a circle.","O(N+log(M)) where N is the number of elements in the grid-aligned bounding box area around the shape provided as the filter and M is the number of items inside the shape","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GEO,GEOSEARCH_History,GEOSEARCH_tips,geosearchCommand,-7,CMD_READONLY,ACL_CATEGORY_GEO,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GEOSEARCH_Args},
{"geosearchstore","Query a sorted set representing a geospatial index to fetch members inside an area of a box or a circle, and store the result in another key.","O(N+log(M)) where N is the number of elements in the grid-aligned bounding box area around the shape provided as the filter and M is the number of items inside the shape","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_GEO,GEOSEARCHSTORE_History,GEOSEARCHSTORE_tips,geosearchstoreCommand,-8,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_GEO,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GEOSEARCHSTORE_Args},

{"hdel","Delete one or more hash fields","O(N) where N is the number of fields to be removed.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HDEL_History,HDEL_tips,hdelCommand,-3,CMD_WRITE|CMD_FAST,ACL_CATEGORY_HASH,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HDEL_Args},
{"hexists","Determine if a hash field exists","O(1)","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HEXISTS_History,HEXISTS_tips,hexistsCommand,3,CMD_READONLY|CMD_FAST,ACL_CATEGORY_HASH,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HEXISTS_Args},
{"hget","Get the value of a hash field","O(1)","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HGET_History,HGET_tips,hgetCommand,3,CMD_READONLY|CMD_FAST,ACL_CATEGORY_HASH,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HGET_Args},
{"hgetall","Get all the fields and values in a hash","O(N) where N is the size of the hash.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HGETALL_History,HGETALL_tips,hgetallCommand,2,CMD_READONLY,ACL_CATEGORY_HASH,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HGETALL_Args},
{"hincrby","Increment the integer value of a hash field by the given number","O(1)","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HINCRBY_History,HINCRBY_tips,hincrbyCommand,4,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_HASH,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HINCRBY_Args},
{"hincrbyfloat","Increment the float value of a hash field by the given amount","O(1)","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HINCRBYFLOAT_History,HINCRBYFLOAT_tips,hincrbyfloatCommand,4,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_HASH,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HINCRBYFLOAT_Args},
{"hkeys","Get all the fields in a hash","O(N) where N is the size of the hash.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HKEYS_History,HKEYS_tips,hkeysCommand,2,CMD_READONLY,ACL_CATEGORY_HASH,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HKEYS_Args},
{"hlen","Get the number of fields in a hash","O(1)","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HLEN_History,HLEN_tips,hlenCommand,2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_HASH,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HLEN_Args},
{"hmget","Get the values of all the given hash fields","O(N) where N is the number of fields being requested.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HMGET_History,HMGET_tips,hmgetCommand,-3,CMD_READONLY|CMD_FAST,ACL_CATEGORY_HASH,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HMGET_Args},
{"hmset","Set multiple hash fields to multiple values","O(N) where N is the number of fields being set.","2.0.0",CMD_DOC_DEPRECATED,"`HSET` with multiple field-value pairs","4.0.0",COMMAND_GROUP_HASH,HMSET_History,HMSET_tips,hsetCommand,-4,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_HASH,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HMSET_Args},
{"hrandfield","Get one or multiple random fields from a hash","O(N) where N is the number of fields returned","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HRANDFIELD_History,HRANDFIELD_tips,hrandfieldCommand,-2,CMD_READONLY,ACL_CATEGORY_HASH,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HRANDFIELD_Args},
{"hscan","Incrementally iterate hash fields and associated values","O(1) for every call. O(N) for a complete iteration, including enough command calls for the cursor to return back to 0. N is the number of elements inside the collection..","2.8.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HSCAN_History,HSCAN_tips,hscanCommand,-3,CMD_READONLY,ACL_CATEGORY_HASH,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HSCAN_Args},
{"hset","Set the string value of a hash field","O(1) for each field/value pair added, so O(N) to add N field/value pairs when the command is called with multiple field/value pairs.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HSET_History,HSET_tips,hsetCommand,-4,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_HASH,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HSET_Args},
{"hsetnx","Set the value of a hash field, only if the field does not exist","O(1)","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HSETNX_History,HSETNX_tips,hsetnxCommand,4,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_HASH,{{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HSETNX_Args},
{"hstrlen","Get the length of the value of a hash field","O(1)","3.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HSTRLEN_History,HSTRLEN_tips,hstrlenCommand,3,CMD_READONLY|CMD_FAST,ACL_CATEGORY_HASH,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HSTRLEN_Args},
{"hvals","Get all the values in a hash","O(N) where N is the size of the hash.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HASH,HVALS_History,HVALS_tips,hvalsCommand,2,CMD_READONLY,ACL_CATEGORY_HASH,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=HVALS_Args},

{"pfadd","Adds the specified elements to the specified HyperLogLog.","O(1) to add every element.","2.8.9",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HYPERLOGLOG,PFADD_History,PFADD_tips,pfaddCommand,-2,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_HYPERLOGLOG,{{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=PFADD_Args},
{"pfcount","Return the approximated cardinality of the set(s) observed by the HyperLogLog at key(s).","O(1) with a very small average constant time when called with a single key. O(N) with N being the number of keys, and much bigger constant times, when called with multiple keys.","2.8.9",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HYPERLOGLOG,PFCOUNT_History,PFCOUNT_tips,pfcountCommand,-2,CMD_READONLY|CMD_MAY_REPLICATE,ACL_CATEGORY_HYPERLOGLOG,{{CMD_KEY_RW|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=PFCOUNT_Args},
{"pfdebug","Internal commands for debugging HyperLogLog values","N/A","2.8.9",CMD_DOC_SYSCMD,NULL,NULL,COMMAND_GROUP_HYPERLOGLOG,PFDEBUG_History,PFDEBUG_tips,pfdebugCommand,-3,CMD_WRITE|CMD_DENYOOM|CMD_ADMIN,ACL_CATEGORY_HYPERLOGLOG,{{CMD_KEY_RW|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}}},
{"pfmerge","Merge N different HyperLogLogs into a single one.","O(N) to merge N HyperLogLogs, but with high constant times.","2.8.9",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_HYPERLOGLOG,PFMERGE_History,PFMERGE_tips,pfmergeCommand,-2,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_HYPERLOGLOG,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=PFMERGE_Args},
{"pfselftest","An internal command for testing HyperLogLog values","N/A","2.8.9",CMD_DOC_SYSCMD,NULL,NULL,COMMAND_GROUP_HYPERLOGLOG,PFSELFTEST_History,PFSELFTEST_tips,pfselftestCommand,1,CMD_ADMIN,ACL_CATEGORY_HYPERLOGLOG},

{"blmove","Pop an element from a list, push it to another list and return it; or block until one is available","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,BLMOVE_History,BLMOVE_tips,blmoveCommand,6,CMD_WRITE|CMD_DENYOOM|CMD_NOSCRIPT|CMD_BLOCKING,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=BLMOVE_Args},
{"blmpop","Pop elements from a list, or block until one is available","O(N+M) where N is the number of provided keys and M is the number of elements returned.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,BLMPOP_History,BLMPOP_tips,blmpopCommand,-5,CMD_WRITE|CMD_BLOCKING,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},blmpopGetKeys,.args=BLMPOP_Args},
{"blpop","Remove and get the first element in a list, or block until one is available","O(N) where N is the number of provided keys.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,BLPOP_History,BLPOP_tips,blpopCommand,-3,CMD_WRITE|CMD_NOSCRIPT|CMD_BLOCKING,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-2,1,0}}},.args=BLPOP_Args},
{"brpop","Remove and get the last element in a list, or block until one is available","O(N) where N is the number of provided keys.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,BRPOP_History,BRPOP_tips,brpopCommand,-3,CMD_WRITE|CMD_NOSCRIPT|CMD_BLOCKING,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-2,1,0}}},.args=BRPOP_Args},
{"brpoplpush","Pop an element from a list, push it to another list and return it; or block until one is available","O(1)","2.2.0",CMD_DOC_DEPRECATED,"`BLMOVE` with the `RIGHT` and `LEFT` arguments","6.2.0",COMMAND_GROUP_LIST,BRPOPLPUSH_History,BRPOPLPUSH_tips,brpoplpushCommand,4,CMD_WRITE|CMD_DENYOOM|CMD_NOSCRIPT|CMD_BLOCKING,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=BRPOPLPUSH_Args},
{"lindex","Get an element from a list by its index","O(N) where N is the number of elements to traverse to get to the element at index. This makes asking for the first or the last element of the list O(1).","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LINDEX_History,LINDEX_tips,lindexCommand,3,CMD_READONLY,ACL_CATEGORY_LIST,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=LINDEX_Args},
{"linsert","Insert an element before or after another element in a list","O(N) where N is the number of elements to traverse before seeing the value pivot. This means that inserting somewhere on the left end on the list (head) can be considered O(1) and inserting somewhere on the right end (tail) is O(N).","2.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LINSERT_History,LINSERT_tips,linsertCommand,5,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=LINSERT_Args},
{"llen","Get the length of a list","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LLEN_History,LLEN_tips,llenCommand,2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_LIST,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=LLEN_Args},
{"lmove","Pop an element from a list, push it to another list and return it","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LMOVE_History,LMOVE_tips,lmoveCommand,5,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=LMOVE_Args},
{"lmpop","Pop elements from a list","O(N+M) where N is the number of provided keys and M is the number of elements returned.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LMPOP_History,LMPOP_tips,lmpopCommand,-4,CMD_WRITE,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},lmpopGetKeys,.args=LMPOP_Args},
{"lpop","Remove and get the first elements in a list","O(N) where N is the number of elements returned","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LPOP_History,LPOP_tips,lpopCommand,-2,CMD_WRITE|CMD_FAST,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=LPOP_Args},
{"lpos","Return the index of matching elements on a list","O(N) where N is the number of elements in the list, for the average case. When searching for elements near the head or the tail of the list, or when the MAXLEN option is provided, the command may run in constant time.","6.0.6",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LPOS_History,LPOS_tips,lposCommand,-3,CMD_READONLY,ACL_CATEGORY_LIST,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=LPOS_Args},
{"lpush","Prepend one or multiple elements to a list","O(1) for each element added, so O(N) to add N elements when the command is called with multiple arguments.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LPUSH_History,LPUSH_tips,lpushCommand,-3,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=LPUSH_Args},
{"lpushx","Prepend an element to a list, only if the list exists","O(1) for each element added, so O(N) to add N elements when the command is called with multiple arguments.","2.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LPUSHX_History,LPUSHX_tips,lpushxCommand,-3,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=LPUSHX_Args},
{"lrange","Get a range of elements from a list","O(S+N) where S is the distance of start offset from HEAD for small lists, from nearest end (HEAD or TAIL) for large lists; and N is the number of elements in the specified range.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LRANGE_History,LRANGE_tips,lrangeCommand,4,CMD_READONLY,ACL_CATEGORY_LIST,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=LRANGE_Args},
{"lrem","Remove elements from a list","O(N+M) where N is the length of the list and M is the number of elements removed.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LREM_History,LREM_tips,lremCommand,4,CMD_WRITE,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=LREM_Args},
{"lset","Set the value of an element in a list by its index","O(N) where N is the length of the list. Setting either the first or the last element of the list is O(1).","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LSET_History,LSET_tips,lsetCommand,4,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=LSET_Args},
{"ltrim","Trim a list to the specified range","O(N) where N is the number of elements to be removed by the operation.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,LTRIM_History,LTRIM_tips,ltrimCommand,4,CMD_WRITE,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=LTRIM_Args},
{"rpop","Remove and get the last elements in a list","O(N) where N is the number of elements returned","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,RPOP_History,RPOP_tips,rpopCommand,-2,CMD_WRITE|CMD_FAST,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=RPOP_Args},
{"rpoplpush","Remove the last element in a list, prepend it to another list and return it","O(1)","1.2.0",CMD_DOC_DEPRECATED,"`LMOVE` with the `RIGHT` and `LEFT` arguments","6.2.0",COMMAND_GROUP_LIST,RPOPLPUSH_History,RPOPLPUSH_tips,rpoplpushCommand,3,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=RPOPLPUSH_Args},
{"rpush","Append one or multiple elements to a list","O(1) for each element added, so O(N) to add N elements when the command is called with multiple arguments.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,RPUSH_History,RPUSH_tips,rpushCommand,-3,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=RPUSH_Args},
{"rpushx","Append an element to a list, only if the list exists","O(1) for each element added, so O(N) to add N elements when the command is called with multiple arguments.","2.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_LIST,RPUSHX_History,RPUSHX_tips,rpushxCommand,-3,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_LIST,{{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=RPUSHX_Args},

{"psubscribe","Listen for messages published to channels matching the given patterns","O(N) where N is the number of patterns the client is already subscribed to.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,PSUBSCRIBE_History,PSUBSCRIBE_tips,psubscribeCommand,-2,CMD_PUBSUB|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0,.args=PSUBSCRIBE_Args},
{"publish","Post a message to a channel","O(N+M) where N is the number of clients subscribed to the receiving channel and M is the total number of subscribed patterns (by any client).","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,PUBLISH_History,PUBLISH_tips,publishCommand,3,CMD_PUBSUB|CMD_LOADING|CMD_STALE|CMD_FAST|CMD_MAY_REPLICATE|CMD_SENTINEL,0,.args=PUBLISH_Args},
{"pubsub","A container for Pub/Sub commands","Depends on subcommand.","2.8.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,PUBSUB_History,PUBSUB_tips,NULL,-2,0,0,.subcommands=PUBSUB_Subcommands},
{"punsubscribe","Stop listening for messages posted to channels matching the given patterns","O(N+M) where N is the number of patterns the client is already subscribed and M is the number of total patterns subscribed in the system (by any client).","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,PUNSUBSCRIBE_History,PUNSUBSCRIBE_tips,punsubscribeCommand,-1,CMD_PUBSUB|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0,.args=PUNSUBSCRIBE_Args},
{"spublish","Post a message to a shard channel","O(N) where N is the number of clients subscribed to the receiving shard channel.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,SPUBLISH_History,SPUBLISH_tips,spublishCommand,3,CMD_PUBSUB|CMD_LOADING|CMD_STALE|CMD_FAST|CMD_MAY_REPLICATE,0,{{CMD_KEY_CHANNEL,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SPUBLISH_Args},
{"ssubscribe","Listen for messages published to the given shard channels","O(N) where N is the number of shard channels to subscribe to.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,SSUBSCRIBE_History,SSUBSCRIBE_tips,ssubscribeCommand,-2,CMD_PUBSUB|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0,{{CMD_KEY_CHANNEL,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=SSUBSCRIBE_Args},
{"subscribe","Listen for messages published to the given channels","O(N) where N is the number of channels to subscribe to.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,SUBSCRIBE_History,SUBSCRIBE_tips,subscribeCommand,-2,CMD_PUBSUB|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0,.args=SUBSCRIBE_Args},
{"sunsubscribe","Stop listening for messages posted to the given shard channels","O(N) where N is the number of clients already subscribed to a channel.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,SUNSUBSCRIBE_History,SUNSUBSCRIBE_tips,sunsubscribeCommand,-1,CMD_PUBSUB|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0,{{CMD_KEY_CHANNEL,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=SUNSUBSCRIBE_Args},
{"unsubscribe","Stop listening for messages posted to the given channels","O(N) where N is the number of clients already subscribed to a channel.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_PUBSUB,UNSUBSCRIBE_History,UNSUBSCRIBE_tips,unsubscribeCommand,-1,CMD_PUBSUB|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SENTINEL,0,.args=UNSUBSCRIBE_Args},

{"eval","Execute a Lua script server side","Depends on the script that is executed.","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,EVAL_History,EVAL_tips,evalCommand,-3,CMD_NOSCRIPT|CMD_SKIP_MONITOR|CMD_MAY_REPLICATE|CMD_NO_MANDATORY_KEYS|CMD_STALE,ACL_CATEGORY_SCRIPTING,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},evalGetKeys,.args=EVAL_Args},
{"evalsha","Execute a Lua script server side","Depends on the script that is executed.","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,EVALSHA_History,EVALSHA_tips,evalShaCommand,-3,CMD_NOSCRIPT|CMD_SKIP_MONITOR|CMD_MAY_REPLICATE|CMD_NO_MANDATORY_KEYS|CMD_STALE,ACL_CATEGORY_SCRIPTING,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},evalGetKeys,.args=EVALSHA_Args},
{"evalsha_ro","Execute a read-only Lua script server side","Depends on the script that is executed.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,EVALSHA_RO_History,EVALSHA_RO_tips,evalShaRoCommand,-3,CMD_NOSCRIPT|CMD_SKIP_MONITOR|CMD_NO_MANDATORY_KEYS|CMD_STALE,ACL_CATEGORY_SCRIPTING,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},evalGetKeys,.args=EVALSHA_RO_Args},
{"eval_ro","Execute a read-only Lua script server side","Depends on the script that is executed.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,EVAL_RO_History,EVAL_RO_tips,evalRoCommand,-3,CMD_NOSCRIPT|CMD_SKIP_MONITOR|CMD_NO_MANDATORY_KEYS|CMD_STALE,ACL_CATEGORY_SCRIPTING,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},evalGetKeys,.args=EVAL_RO_Args},
{"fcall","PATCH__TBD__38__","PATCH__TBD__37__","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,FCALL_History,FCALL_tips,fcallCommand,-3,CMD_NOSCRIPT|CMD_SKIP_MONITOR|CMD_MAY_REPLICATE|CMD_NO_MANDATORY_KEYS|CMD_STALE,ACL_CATEGORY_SCRIPTING,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},functionGetKeys,.args=FCALL_Args},
{"fcall_ro","PATCH__TBD__7__","PATCH__TBD__6__","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,FCALL_RO_History,FCALL_RO_tips,fcallroCommand,-3,CMD_NOSCRIPT|CMD_SKIP_MONITOR|CMD_NO_MANDATORY_KEYS|CMD_STALE,ACL_CATEGORY_SCRIPTING,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},functionGetKeys,.args=FCALL_RO_Args},
{"function","A container for function commands","Depends on subcommand.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,FUNCTION_History,FUNCTION_tips,NULL,-2,0,0,.subcommands=FUNCTION_Subcommands},
{"script","A container for Lua scripts management commands","Depends on subcommand.","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SCRIPTING,SCRIPT_History,SCRIPT_tips,NULL,-2,0,0,.subcommands=SCRIPT_Subcommands},

{"sentinel","A container for Sentinel commands","Depends on subcommand.","2.8.4",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SENTINEL,SENTINEL_History,SENTINEL_tips,NULL,-2,CMD_ADMIN|CMD_SENTINEL|CMD_ONLY_SENTINEL,0,.subcommands=SENTINEL_Subcommands},

{"acl","A container for Access List Control commands ","Depends on subcommand.","6.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ACL_History,ACL_tips,NULL,-2,CMD_SENTINEL,0,.subcommands=ACL_Subcommands},
{"bgrewriteaof","Asynchronously rewrite the append-only file","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,BGREWRITEAOF_History,BGREWRITEAOF_tips,bgrewriteaofCommand,1,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_NOSCRIPT,0},
{"bgsave","Asynchronously save the dataset to disk","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,BGSAVE_History,BGSAVE_tips,bgsaveCommand,-1,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_NOSCRIPT,0,.args=BGSAVE_Args},
{"command","Get array of Redis command details","O(N) where N is the total number of Redis commands","2.8.13",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,COMMAND_History,COMMAND_tips,commandCommand,-1,CMD_LOADING|CMD_STALE|CMD_SENTINEL,ACL_CATEGORY_CONNECTION,.subcommands=COMMAND_Subcommands},
{"config","A container for server configuration commands","Depends on subcommand.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,CONFIG_History,CONFIG_tips,NULL,-2,0,0,.subcommands=CONFIG_Subcommands},
{"dbsize","Return the number of keys in the selected database","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,DBSIZE_History,DBSIZE_tips,dbsizeCommand,1,CMD_READONLY|CMD_FAST,ACL_CATEGORY_KEYSPACE},
{"debug","A container for debugging commands","Depends on subcommand.","1.0.0",CMD_DOC_SYSCMD,NULL,NULL,COMMAND_GROUP_SERVER,DEBUG_History,DEBUG_tips,debugCommand,-2,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_PROTECTED,0},
{"failover","Start a coordinated failover between this server and one of its replicas.","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,FAILOVER_History,FAILOVER_tips,failoverCommand,-1,CMD_ADMIN|CMD_NOSCRIPT|CMD_STALE,0,.args=FAILOVER_Args},
{"flushall","Remove all keys from all databases","O(N) where N is the total number of keys in all databases","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,FLUSHALL_History,FLUSHALL_tips,flushallCommand,-1,CMD_WRITE,ACL_CATEGORY_KEYSPACE|ACL_CATEGORY_DANGEROUS,.args=FLUSHALL_Args},
{"flushdb","Remove all keys from the current database","O(N) where N is the number of keys in the selected database","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,FLUSHDB_History,FLUSHDB_tips,flushdbCommand,-1,CMD_WRITE,ACL_CATEGORY_KEYSPACE|ACL_CATEGORY_DANGEROUS,.args=FLUSHDB_Args},
{"info","Get information and statistics about the server","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,INFO_History,INFO_tips,infoCommand,-1,CMD_LOADING|CMD_STALE|CMD_SENTINEL,ACL_CATEGORY_DANGEROUS,.args=INFO_Args},
{"lastsave","Get the UNIX time stamp of the last successful save to disk","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,LASTSAVE_History,LASTSAVE_tips,lastsaveCommand,1,CMD_LOADING|CMD_STALE|CMD_FAST,ACL_CATEGORY_ADMIN|ACL_CATEGORY_DANGEROUS},
{"latency","A container for latency diagnostics commands","Depends on subcommand.","2.8.13",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,LATENCY_History,LATENCY_tips,NULL,-2,0,0,.subcommands=LATENCY_Subcommands},
{"lolwut","Display some computer art and the Redis version",NULL,"5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,LOLWUT_History,LOLWUT_tips,lolwutCommand,-1,CMD_READONLY|CMD_FAST,0,.args=LOLWUT_Args},
{"memory","A container for memory diagnostics commands","Depends on subcommand.","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MEMORY_History,MEMORY_tips,NULL,-2,0,0,.subcommands=MEMORY_Subcommands},
{"module","A container for module commands","Depends on subcommand.","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MODULE_History,MODULE_tips,NULL,-2,0,0,.subcommands=MODULE_Subcommands},
{"monitor","Listen for all requests received by the server in real time",NULL,"1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,MONITOR_History,MONITOR_tips,monitorCommand,1,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0},
{"psync","Internal command used for replication",NULL,"2.8.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,PSYNC_History,PSYNC_tips,syncCommand,-3,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_NO_MULTI|CMD_NOSCRIPT,0,.args=PSYNC_Args},
{"replconf","An internal command for configuring the replication stream","O(1)","3.0.0",CMD_DOC_SYSCMD,NULL,NULL,COMMAND_GROUP_SERVER,REPLCONF_History,REPLCONF_tips,replconfCommand,-1,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE,0},
{"replicaof","Make the server a replica of another instance, or promote it as master.","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,REPLICAOF_History,REPLICAOF_tips,replicaofCommand,3,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_ALLOW_BUSY|CMD_NOSCRIPT|CMD_STALE,0,.args=REPLICAOF_Args},
{"restore-asking","An internal command for migrating keys in a cluster","O(1) to create the new key and additional O(N*M) to reconstruct the serialized value, where N is the number of Redis objects composing the value and M their average size. For small string values the time complexity is thus O(1)+O(1*M) where M is small, so simply O(1). However for sorted set values the complexity is O(N*M*log(N)) because inserting values into sorted sets is O(log(N)).","3.0.0",CMD_DOC_SYSCMD,NULL,NULL,COMMAND_GROUP_SERVER,RESTORE_ASKING_History,RESTORE_ASKING_tips,restoreCommand,-4,CMD_WRITE|CMD_DENYOOM|CMD_ASKING,ACL_CATEGORY_KEYSPACE|ACL_CATEGORY_DANGEROUS,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}}},
{"role","Return the role of the instance in the context of replication","O(1)","2.8.12",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,ROLE_History,ROLE_tips,roleCommand,1,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_FAST|CMD_SENTINEL,ACL_CATEGORY_ADMIN|ACL_CATEGORY_DANGEROUS},
{"save","Synchronously save the dataset to disk","O(N) where N is the total number of keys in all databases","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,SAVE_History,SAVE_tips,saveCommand,1,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_NOSCRIPT|CMD_NO_MULTI,0},
{"shutdown","Synchronously save the dataset to disk and then shut down the server","O(N) when saving, where N is the total number of keys in all databases when saving data, otherwise O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,SHUTDOWN_History,SHUTDOWN_tips,shutdownCommand,-1,CMD_ADMIN|CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_NO_MULTI|CMD_SENTINEL|CMD_ALLOW_BUSY,0,.args=SHUTDOWN_Args},
{"slaveof","Make the server a replica of another instance, or promote it as master. Deprecated starting with Redis 5. Use REPLICAOF instead.","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,SLAVEOF_History,SLAVEOF_tips,replicaofCommand,3,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_NOSCRIPT|CMD_STALE,0,.args=SLAVEOF_Args},
{"slowlog","A container for slow log commands","Depends on subcommand.","2.2.12",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,SLOWLOG_History,SLOWLOG_tips,NULL,-2,0,0,.subcommands=SLOWLOG_Subcommands},
{"swapdb","Swaps two Redis databases","O(N) where N is the count of clients watching or blocking on keys from both databases.","4.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,SWAPDB_History,SWAPDB_tips,swapdbCommand,3,CMD_WRITE|CMD_FAST,ACL_CATEGORY_KEYSPACE|ACL_CATEGORY_DANGEROUS,.args=SWAPDB_Args},
{"sync","Internal command used for replication",NULL,"1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,SYNC_History,SYNC_tips,syncCommand,1,CMD_NO_ASYNC_LOADING|CMD_ADMIN|CMD_NO_MULTI|CMD_NOSCRIPT,0},
{"time","Return the current server time","O(1)","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SERVER,TIME_History,TIME_tips,timeCommand,1,CMD_LOADING|CMD_STALE|CMD_FAST,0},

{"sadd","Add one or more members to a set","O(1) for each element added, so O(N) to add N elements when the command is called with multiple arguments.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SADD_History,SADD_tips,saddCommand,-3,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_SET,{{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SADD_Args},
{"scard","Get the number of members in a set","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SCARD_History,SCARD_tips,scardCommand,2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_SET,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SCARD_Args},
{"sdiff","Subtract multiple sets","O(N) where N is the total number of elements in all given sets.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SDIFF_History,SDIFF_tips,sdiffCommand,-2,CMD_READONLY,ACL_CATEGORY_SET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=SDIFF_Args},
{"sdiffstore","Subtract multiple sets and store the resulting set in a key","O(N) where N is the total number of elements in all given sets.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SDIFFSTORE_History,SDIFFSTORE_tips,sdiffstoreCommand,-3,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_SET,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=SDIFFSTORE_Args},
{"sinter","Intersect multiple sets","O(N*M) worst case where N is the cardinality of the smallest set and M is the number of sets.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SINTER_History,SINTER_tips,sinterCommand,-2,CMD_READONLY,ACL_CATEGORY_SET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=SINTER_Args},
{"sintercard","Intersect multiple sets and return the cardinality of the result","O(N*M) worst case where N is the cardinality of the smallest set and M is the number of sets.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SINTERCARD_History,SINTERCARD_tips,sinterCardCommand,-3,CMD_READONLY,ACL_CATEGORY_SET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},sintercardGetKeys,.args=SINTERCARD_Args},
{"sinterstore","Intersect multiple sets and store the resulting set in a key","O(N*M) worst case where N is the cardinality of the smallest set and M is the number of sets.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SINTERSTORE_History,SINTERSTORE_tips,sinterstoreCommand,-3,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_SET,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=SINTERSTORE_Args},
{"sismember","Determine if a given value is a member of a set","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SISMEMBER_History,SISMEMBER_tips,sismemberCommand,3,CMD_READONLY|CMD_FAST,ACL_CATEGORY_SET,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SISMEMBER_Args},
{"smembers","Get all the members in a set","O(N) where N is the set cardinality.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SMEMBERS_History,SMEMBERS_tips,sinterCommand,2,CMD_READONLY,ACL_CATEGORY_SET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SMEMBERS_Args},
{"smismember","Returns the membership associated with the given elements for a set","O(N) where N is the number of elements being checked for membership","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SMISMEMBER_History,SMISMEMBER_tips,smismemberCommand,-3,CMD_READONLY|CMD_FAST,ACL_CATEGORY_SET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SMISMEMBER_Args},
{"smove","Move a member from one set to another","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SMOVE_History,SMOVE_tips,smoveCommand,4,CMD_WRITE|CMD_FAST,ACL_CATEGORY_SET,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SMOVE_Args},
{"spop","Remove and return one or multiple random members from a set","Without the count argument O(1), otherwise O(N) where N is the value of the passed count.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SPOP_History,SPOP_tips,spopCommand,-2,CMD_WRITE|CMD_FAST,ACL_CATEGORY_SET,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SPOP_Args},
{"srandmember","Get one or multiple random members from a set","Without the count argument O(1), otherwise O(N) where N is the absolute value of the passed count.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SRANDMEMBER_History,SRANDMEMBER_tips,srandmemberCommand,-2,CMD_READONLY,ACL_CATEGORY_SET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SRANDMEMBER_Args},
{"srem","Remove one or more members from a set","O(N) where N is the number of members to be removed.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SREM_History,SREM_tips,sremCommand,-3,CMD_WRITE|CMD_FAST,ACL_CATEGORY_SET,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SREM_Args},
{"sscan","Incrementally iterate Set elements","O(1) for every call. O(N) for a complete iteration, including enough command calls for the cursor to return back to 0. N is the number of elements inside the collection..","2.8.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SSCAN_History,SSCAN_tips,sscanCommand,-3,CMD_READONLY,ACL_CATEGORY_SET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SSCAN_Args},
{"sunion","Add multiple sets","O(N) where N is the total number of elements in all given sets.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SUNION_History,SUNION_tips,sunionCommand,-2,CMD_READONLY,ACL_CATEGORY_SET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=SUNION_Args},
{"sunionstore","Add multiple sets and store the resulting set in a key","O(N) where N is the total number of elements in all given sets.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SET,SUNIONSTORE_History,SUNIONSTORE_tips,sunionstoreCommand,-3,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_SET,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=SUNIONSTORE_Args},

{"bzmpop","Remove and return members with scores in a sorted set or block until one is available","O(K) + O(N*log(M)) where K is the number of provided keys, N being the number of elements in the sorted set, and M being the number of elements popped.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,BZMPOP_History,BZMPOP_tips,bzmpopCommand,-5,CMD_WRITE|CMD_BLOCKING,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},blmpopGetKeys,.args=BZMPOP_Args},
{"bzpopmax","Remove and return the member with the highest score from one or more sorted sets, or block until one is available","O(log(N)) with N being the number of elements in the sorted set.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,BZPOPMAX_History,BZPOPMAX_tips,bzpopmaxCommand,-3,CMD_WRITE|CMD_NOSCRIPT|CMD_FAST|CMD_BLOCKING,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-2,1,0}}},.args=BZPOPMAX_Args},
{"bzpopmin","Remove and return the member with the lowest score from one or more sorted sets, or block until one is available","O(log(N)) with N being the number of elements in the sorted set.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,BZPOPMIN_History,BZPOPMIN_tips,bzpopminCommand,-3,CMD_WRITE|CMD_NOSCRIPT|CMD_FAST|CMD_BLOCKING,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-2,1,0}}},.args=BZPOPMIN_Args},
{"zadd","Add one or more members to a sorted set, or update its score if it already exists","O(log(N)) for each item added, where N is the number of elements in the sorted set.","1.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZADD_History,ZADD_tips,zaddCommand,-4,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZADD_Args},
{"zcard","Get the number of members in a sorted set","O(1)","1.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZCARD_History,ZCARD_tips,zcardCommand,2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZCARD_Args},
{"zcount","Count the members in a sorted set with scores within the given values","O(log(N)) with N being the number of elements in the sorted set.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZCOUNT_History,ZCOUNT_tips,zcountCommand,4,CMD_READONLY|CMD_FAST,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZCOUNT_Args},
{"zdiff","Subtract multiple sorted sets","O(L + (N-K)log(N)) worst case where L is the total number of elements in all the sets, N is the size of the first set, and K is the size of the result set.","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZDIFF_History,ZDIFF_tips,zdiffCommand,-3,CMD_READONLY,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},zunionInterDiffGetKeys,.args=ZDIFF_Args},
{"zdiffstore","Subtract multiple sorted sets and store the resulting sorted set in a new key","O(L + (N-K)log(N)) worst case where L is the total number of elements in all the sets, N is the size of the first set, and K is the size of the result set.","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZDIFFSTORE_History,ZDIFFSTORE_tips,zdiffstoreCommand,-4,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},zunionInterDiffStoreGetKeys,.args=ZDIFFSTORE_Args},
{"zincrby","Increment the score of a member in a sorted set","O(log(N)) where N is the number of elements in the sorted set.","1.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZINCRBY_History,ZINCRBY_tips,zincrbyCommand,4,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZINCRBY_Args},
{"zinter","Intersect multiple sorted sets","O(N*K)+O(M*log(M)) worst case with N being the smallest input sorted set, K being the number of input sorted sets and M being the number of elements in the resulting sorted set.","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZINTER_History,ZINTER_tips,zinterCommand,-3,CMD_READONLY,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},zunionInterDiffGetKeys,.args=ZINTER_Args},
{"zintercard","Intersect multiple sorted sets and return the cardinality of the result","O(N*K) worst case with N being the smallest input sorted set, K being the number of input sorted sets.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZINTERCARD_History,ZINTERCARD_tips,zinterCardCommand,-3,CMD_READONLY,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},zunionInterDiffGetKeys,.args=ZINTERCARD_Args},
{"zinterstore","Intersect multiple sorted sets and store the resulting sorted set in a new key","O(N*K)+O(M*log(M)) worst case with N being the smallest input sorted set, K being the number of input sorted sets and M being the number of elements in the resulting sorted set.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZINTERSTORE_History,ZINTERSTORE_tips,zinterstoreCommand,-4,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},zunionInterDiffStoreGetKeys,.args=ZINTERSTORE_Args},
{"zlexcount","Count the number of members in a sorted set between a given lexicographical range","O(log(N)) with N being the number of elements in the sorted set.","2.8.9",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZLEXCOUNT_History,ZLEXCOUNT_tips,zlexcountCommand,4,CMD_READONLY|CMD_FAST,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZLEXCOUNT_Args},
{"zmpop","Remove and return members with scores in a sorted set","O(K) + O(N*log(M)) where K is the number of provided keys, N being the number of elements in the sorted set, and M being the number of elements popped.","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZMPOP_History,ZMPOP_tips,zmpopCommand,-4,CMD_WRITE,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},zmpopGetKeys,.args=ZMPOP_Args},
{"zmscore","Get the score associated with the given members in a sorted set","O(N) where N is the number of members being requested.","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZMSCORE_History,ZMSCORE_tips,zmscoreCommand,-3,CMD_READONLY|CMD_FAST,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZMSCORE_Args},
{"zpopmax","Remove and return members with the highest scores in a sorted set","O(log(N)*M) with N being the number of elements in the sorted set, and M being the number of elements popped.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZPOPMAX_History,ZPOPMAX_tips,zpopmaxCommand,-2,CMD_WRITE|CMD_FAST,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZPOPMAX_Args},
{"zpopmin","Remove and return members with the lowest scores in a sorted set","O(log(N)*M) with N being the number of elements in the sorted set, and M being the number of elements popped.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZPOPMIN_History,ZPOPMIN_tips,zpopminCommand,-2,CMD_WRITE|CMD_FAST,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZPOPMIN_Args},
{"zrandmember","Get one or multiple random elements from a sorted set","O(N) where N is the number of elements returned","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZRANDMEMBER_History,ZRANDMEMBER_tips,zrandmemberCommand,-2,CMD_READONLY,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZRANDMEMBER_Args},
{"zrange","Return a range of members in a sorted set","O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements returned.","1.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZRANGE_History,ZRANGE_tips,zrangeCommand,-4,CMD_READONLY,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZRANGE_Args},
{"zrangebylex","Return a range of members in a sorted set, by lexicographical range","O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements being returned. If M is constant (e.g. always asking for the first 10 elements with LIMIT), you can consider it O(log(N)).","2.8.9",CMD_DOC_DEPRECATED,"`ZRANGE` with the `BYSCORE` argument","6.2.0",COMMAND_GROUP_SORTED_SET,ZRANGEBYLEX_History,ZRANGEBYLEX_tips,zrangebylexCommand,-4,CMD_READONLY,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZRANGEBYLEX_Args},
{"zrangebyscore","Return a range of members in a sorted set, by score","O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements being returned. If M is constant (e.g. always asking for the first 10 elements with LIMIT), you can consider it O(log(N)).","1.0.5",CMD_DOC_DEPRECATED,"`ZRANGE` with the `BYSCORE` argument","6.2.0",COMMAND_GROUP_SORTED_SET,ZRANGEBYSCORE_History,ZRANGEBYSCORE_tips,zrangebyscoreCommand,-4,CMD_READONLY,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZRANGEBYSCORE_Args},
{"zrangestore","Store a range of members from sorted set into another key","O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements stored into the destination key.","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZRANGESTORE_History,ZRANGESTORE_tips,zrangestoreCommand,-5,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZRANGESTORE_Args},
{"zrank","Determine the index of a member in a sorted set","O(log(N))","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZRANK_History,ZRANK_tips,zrankCommand,3,CMD_READONLY|CMD_FAST,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZRANK_Args},
{"zrem","Remove one or more members from a sorted set","O(M*log(N)) with N being the number of elements in the sorted set and M the number of elements to be removed.","1.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZREM_History,ZREM_tips,zremCommand,-3,CMD_WRITE|CMD_FAST,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZREM_Args},
{"zremrangebylex","Remove all members in a sorted set between the given lexicographical range","O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements removed by the operation.","2.8.9",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZREMRANGEBYLEX_History,ZREMRANGEBYLEX_tips,zremrangebylexCommand,4,CMD_WRITE,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZREMRANGEBYLEX_Args},
{"zremrangebyrank","Remove all members in a sorted set within the given indexes","O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements removed by the operation.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZREMRANGEBYRANK_History,ZREMRANGEBYRANK_tips,zremrangebyrankCommand,4,CMD_WRITE,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZREMRANGEBYRANK_Args},
{"zremrangebyscore","Remove all members in a sorted set within the given scores","O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements removed by the operation.","1.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZREMRANGEBYSCORE_History,ZREMRANGEBYSCORE_tips,zremrangebyscoreCommand,4,CMD_WRITE,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZREMRANGEBYSCORE_Args},
{"zrevrange","Return a range of members in a sorted set, by index, with scores ordered from high to low","O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements returned.","1.2.0",CMD_DOC_DEPRECATED,"`ZRANGE` with the `REV` argument","6.2.0",COMMAND_GROUP_SORTED_SET,ZREVRANGE_History,ZREVRANGE_tips,zrevrangeCommand,-4,CMD_READONLY,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZREVRANGE_Args},
{"zrevrangebylex","Return a range of members in a sorted set, by lexicographical range, ordered from higher to lower strings.","O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements being returned. If M is constant (e.g. always asking for the first 10 elements with LIMIT), you can consider it O(log(N)).","2.8.9",CMD_DOC_DEPRECATED,"`ZRANGE` with the `REV` and `BYLEX` arguments","6.2.0",COMMAND_GROUP_SORTED_SET,ZREVRANGEBYLEX_History,ZREVRANGEBYLEX_tips,zrevrangebylexCommand,-4,CMD_READONLY,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZREVRANGEBYLEX_Args},
{"zrevrangebyscore","Return a range of members in a sorted set, by score, with scores ordered from high to low","O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements being returned. If M is constant (e.g. always asking for the first 10 elements with LIMIT), you can consider it O(log(N)).","2.2.0",CMD_DOC_DEPRECATED,"`ZRANGE` with the `REV` and `BYSCORE` arguments","6.2.0",COMMAND_GROUP_SORTED_SET,ZREVRANGEBYSCORE_History,ZREVRANGEBYSCORE_tips,zrevrangebyscoreCommand,-4,CMD_READONLY,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZREVRANGEBYSCORE_Args},
{"zrevrank","Determine the index of a member in a sorted set, with scores ordered from high to low","O(log(N))","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZREVRANK_History,ZREVRANK_tips,zrevrankCommand,3,CMD_READONLY|CMD_FAST,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZREVRANK_Args},
{"zscan","Incrementally iterate sorted sets elements and associated scores","O(1) for every call. O(N) for a complete iteration, including enough command calls for the cursor to return back to 0. N is the number of elements inside the collection..","2.8.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZSCAN_History,ZSCAN_tips,zscanCommand,-3,CMD_READONLY,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZSCAN_Args},
{"zscore","Get the score associated with the given member in a sorted set","O(1)","1.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZSCORE_History,ZSCORE_tips,zscoreCommand,3,CMD_READONLY|CMD_FAST,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=ZSCORE_Args},
{"zunion","Add multiple sorted sets","O(N)+O(M*log(M)) with N being the sum of the sizes of the input sorted sets, and M being the number of elements in the resulting sorted set.","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZUNION_History,ZUNION_tips,zunionCommand,-3,CMD_READONLY,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},zunionInterDiffGetKeys,.args=ZUNION_Args},
{"zunionstore","Add multiple sorted sets and store the resulting sorted set in a new key","O(N)+O(M log(M)) with N being the sum of the sizes of the input sorted sets, and M being the number of elements in the resulting sorted set.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_SORTED_SET,ZUNIONSTORE_History,ZUNIONSTORE_tips,zunionstoreCommand,-4,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_SORTEDSET,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}},{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={2},KSPEC_FK_KEYNUM,.fk.keynum={0,1,1}}},zunionInterDiffStoreGetKeys,.args=ZUNIONSTORE_Args},

{"xack","Marks a pending message as correctly processed, effectively removing it from the pending entries list of the consumer group. Return value of the command is the number of messages successfully acknowledged, that is, the IDs we were actually able to resolve in the PEL.","O(1) for each message ID processed.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XACK_History,XACK_tips,xackCommand,-4,CMD_WRITE|CMD_FAST,ACL_CATEGORY_STREAM,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XACK_Args},
{"xadd","Appends a new entry to a stream","O(1) when adding a new entry, O(N) when trimming where N being the number of entries evicted.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XADD_History,XADD_tips,xaddCommand,-5,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_STREAM,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XADD_Args},
{"xautoclaim","Changes (or acquires) ownership of messages in a consumer group, as if the messages were delivered to the specified consumer.","O(1) if COUNT is small.","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XAUTOCLAIM_History,XAUTOCLAIM_tips,xautoclaimCommand,-6,CMD_WRITE|CMD_FAST,ACL_CATEGORY_STREAM,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XAUTOCLAIM_Args},
{"xclaim","Changes (or acquires) ownership of a message in a consumer group, as if the message was delivered to the specified consumer.","O(log N) with N being the number of messages in the PEL of the consumer group.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XCLAIM_History,XCLAIM_tips,xclaimCommand,-6,CMD_WRITE|CMD_FAST,ACL_CATEGORY_STREAM,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XCLAIM_Args},
{"xdel","Removes the specified entries from the stream. Returns the number of items actually deleted, that may be different from the number of IDs passed in case certain IDs do not exist.","O(1) for each single item to delete in the stream, regardless of the stream size.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XDEL_History,XDEL_tips,xdelCommand,-3,CMD_WRITE|CMD_FAST,ACL_CATEGORY_STREAM,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XDEL_Args},
{"xgroup","A container for consumer groups commands","Depends on subcommand.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XGROUP_History,XGROUP_tips,NULL,-2,0,0,.subcommands=XGROUP_Subcommands},
{"xinfo","A container for stream introspection commands","Depends on subcommand.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XINFO_History,XINFO_tips,NULL,-2,0,0,.subcommands=XINFO_Subcommands},
{"xlen","Return the number of entries in a stream","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XLEN_History,XLEN_tips,xlenCommand,2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_STREAM,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XLEN_Args},
{"xpending","Return information and entries from a stream consumer group pending entries list, that are messages fetched but never acknowledged.","O(N) with N being the number of elements returned, so asking for a small fixed number of entries per call is O(1). O(M), where M is the total number of entries scanned when used with the IDLE filter. When the command returns just the summary and the list of consumers is small, it runs in O(1) time; otherwise, an additional O(N) time for iterating every consumer.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XPENDING_History,XPENDING_tips,xpendingCommand,-3,CMD_READONLY,ACL_CATEGORY_STREAM,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XPENDING_Args},
{"xrange","Return a range of elements in a stream, with IDs matching the specified IDs interval","O(N) with N being the number of elements being returned. If N is constant (e.g. always asking for the first 10 elements with COUNT), you can consider it O(1).","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XRANGE_History,XRANGE_tips,xrangeCommand,-4,CMD_READONLY,ACL_CATEGORY_STREAM,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XRANGE_Args},
{"xread","Return never seen elements in multiple streams, with IDs greater than the ones reported by the caller for each stream. Can block.","For each stream mentioned: O(N) with N being the number of elements being returned, it means that XREAD-ing with a fixed COUNT is O(1). Note that when the BLOCK option is used, XADD will pay O(M) time in order to serve the M clients blocked on the stream getting new data.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XREAD_History,XREAD_tips,xreadCommand,-4,CMD_BLOCKING|CMD_READONLY|CMD_BLOCKING,ACL_CATEGORY_STREAM,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_KEYWORD,.bs.keyword={"STREAMS",1},KSPEC_FK_RANGE,.fk.range={-1,1,2}}},xreadGetKeys,.args=XREAD_Args},
{"xreadgroup","Return new entries from a stream using a consumer group, or access the history of the pending entries for a given consumer. Can block.","For each stream mentioned: O(M) with M being the number of elements returned. If M is constant (e.g. always asking for the first 10 elements with COUNT), you can consider it O(1). On the other side when XREADGROUP blocks, XADD will pay the O(N) time in order to serve the N clients blocked on the stream getting new data.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XREADGROUP_History,XREADGROUP_tips,xreadCommand,-7,CMD_BLOCKING|CMD_WRITE,ACL_CATEGORY_STREAM,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_KEYWORD,.bs.keyword={"STREAMS",4},KSPEC_FK_RANGE,.fk.range={-1,1,2}}},xreadGetKeys,.args=XREADGROUP_Args},
{"xrevrange","Return a range of elements in a stream, with IDs matching the specified IDs interval, in reverse order (from greater to smaller IDs) compared to XRANGE","O(N) with N being the number of elements returned. If N is constant (e.g. always asking for the first 10 elements with COUNT), you can consider it O(1).","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XREVRANGE_History,XREVRANGE_tips,xrevrangeCommand,-4,CMD_READONLY,ACL_CATEGORY_STREAM,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XREVRANGE_Args},
{"xsetid","An internal command for replicating stream values","O(1)","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XSETID_History,XSETID_tips,xsetidCommand,3,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_STREAM,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XSETID_Args},
{"xtrim","Trims the stream to (approximately if '~' is passed) a certain size","O(N), with N being the number of evicted entries. Constant times are very small however, since entries are organized in macro nodes containing multiple entries that can be released with a single deallocation.","5.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STREAM,XTRIM_History,XTRIM_tips,xtrimCommand,-4,CMD_WRITE,ACL_CATEGORY_STREAM,{{CMD_KEY_RW|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=XTRIM_Args},

{"append","Append a value to a key","O(1). The amortized time complexity is O(1) assuming the appended value is small and the already present value is of any size, since the dynamic string library used by Redis will double the free space available on every reallocation.","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,APPEND_History,APPEND_tips,appendCommand,3,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_RW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=APPEND_Args},
{"decr","Decrement the integer value of a key by one","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,DECR_History,DECR_tips,decrCommand,2,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=DECR_Args},
{"decrby","Decrement the integer value of a key by the given number","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,DECRBY_History,DECRBY_tips,decrbyCommand,3,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=DECRBY_Args},
{"get","Get the value of a key","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,GET_History,GET_tips,getCommand,2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GET_Args},
{"getdel","Get the value of a key and delete the key","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,GETDEL_History,GETDEL_tips,getdelCommand,2,CMD_WRITE|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_DELETE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GETDEL_Args},
{"getex","Get the value of a key and optionally set its expiration","O(1)","6.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,GETEX_History,GETEX_tips,getexCommand,-2,CMD_WRITE|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GETEX_Args},
{"getrange","Get a substring of the string stored at a key","O(N) where N is the length of the returned string. The complexity is ultimately determined by the returned length, but because creating a substring from an existing string is very cheap, it can be considered O(1) for small strings.","2.4.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,GETRANGE_History,GETRANGE_tips,getrangeCommand,4,CMD_READONLY,ACL_CATEGORY_STRING,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GETRANGE_Args},
{"getset","Set the string value of a key and return its old value","O(1)","1.0.0",CMD_DOC_DEPRECATED,"`SET` with the `!GET` argument","6.2.0",COMMAND_GROUP_STRING,GETSET_History,GETSET_tips,getsetCommand,3,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=GETSET_Args},
{"incr","Increment the integer value of a key by one","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,INCR_History,INCR_tips,incrCommand,2,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=INCR_Args},
{"incrby","Increment the integer value of a key by the given amount","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,INCRBY_History,INCRBY_tips,incrbyCommand,3,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=INCRBY_Args},
{"incrbyfloat","Increment the float value of a key by the given amount","O(1)","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,INCRBYFLOAT_History,INCRBYFLOAT_tips,incrbyfloatCommand,3,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=INCRBYFLOAT_Args},
{"lcs","Find longest common substring","O(N*M) where N and M are the lengths of s1 and s2, respectively","7.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,LCS_History,LCS_tips,lcsCommand,-3,CMD_READONLY,ACL_CATEGORY_STRING,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={1,1,0}}},.args=LCS_Args},
{"mget","Get the values of all the given keys","O(N) where N is the number of keys to retrieve.","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,MGET_History,MGET_tips,mgetCommand,-2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=MGET_Args},
{"mset","Set multiple keys to multiple values","O(N) where N is the number of keys to set.","1.0.1",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,MSET_History,MSET_tips,msetCommand,-3,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_STRING,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,2,0}}},.args=MSET_Args},
{"msetnx","Set multiple keys to multiple values, only if none of the keys exist","O(N) where N is the number of keys to set.","1.0.1",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,MSETNX_History,MSETNX_tips,msetnxCommand,-3,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_STRING,{{CMD_KEY_OW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,2,0}}},.args=MSETNX_Args},
{"psetex","Set the value and expiration in milliseconds of a key","O(1)","2.6.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,PSETEX_History,PSETEX_tips,psetexCommand,4,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_STRING,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=PSETEX_Args},
{"set","Set the string value of a key","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,SET_History,SET_tips,setCommand,-3,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_STRING,{{CMD_KEY_RW|CMD_KEY_ACCESS|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SET_Args},
{"setex","Set the value and expiration of a key","O(1)","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,SETEX_History,SETEX_tips,setexCommand,4,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_STRING,{{CMD_KEY_OW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SETEX_Args},
{"setnx","Set the value of a key, only if the key does not exist","O(1)","1.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,SETNX_History,SETNX_tips,setnxCommand,3,CMD_WRITE|CMD_DENYOOM|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_OW|CMD_KEY_INSERT,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SETNX_Args},
{"setrange","Overwrite part of a string at key starting at the specified offset","O(1), not counting the time taken to copy the new string in place. Usually, this string is very small so the amortized complexity is O(1). Otherwise, complexity is O(M) with M being the length of the value argument.","2.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,SETRANGE_History,SETRANGE_tips,setrangeCommand,4,CMD_WRITE|CMD_DENYOOM,ACL_CATEGORY_STRING,{{CMD_KEY_RW|CMD_KEY_UPDATE,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SETRANGE_Args},
{"strlen","Get the length of the value stored in a key","O(1)","2.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_STRING,STRLEN_History,STRLEN_tips,strlenCommand,2,CMD_READONLY|CMD_FAST,ACL_CATEGORY_STRING,{{CMD_KEY_RO,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=STRLEN_Args},
{"substr","Get a substring of the string stored at a key","O(N) where N is the length of the returned string. The complexity is ultimately determined by the returned length, but because creating a substring from an existing string is very cheap, it can be considered O(1) for small strings.","1.0.0",CMD_DOC_DEPRECATED,"`GETRANGE`","2.0.0",COMMAND_GROUP_STRING,SUBSTR_History,SUBSTR_tips,getrangeCommand,4,CMD_READONLY,ACL_CATEGORY_STRING,{{CMD_KEY_RO|CMD_KEY_ACCESS,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={0,1,0}}},.args=SUBSTR_Args},

{"discard","Discard all commands issued after MULTI","O(N), when N is the number of queued commands","2.0.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_TRANSACTIONS,DISCARD_History,DISCARD_tips,discardCommand,1,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_FAST|CMD_ALLOW_BUSY,ACL_CATEGORY_TRANSACTION},
{"exec","Execute all commands issued after MULTI","Depends on commands in the transaction","1.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_TRANSACTIONS,EXEC_History,EXEC_tips,execCommand,1,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_SKIP_SLOWLOG,ACL_CATEGORY_TRANSACTION},
{"multi","Mark the start of a transaction block","O(1)","1.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_TRANSACTIONS,MULTI_History,MULTI_tips,multiCommand,1,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_FAST|CMD_ALLOW_BUSY,ACL_CATEGORY_TRANSACTION},
{"unwatch","Forget about all watched keys","O(1)","2.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_TRANSACTIONS,UNWATCH_History,UNWATCH_tips,unwatchCommand,1,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_FAST|CMD_ALLOW_BUSY,ACL_CATEGORY_TRANSACTION},
{"watch","Watch the given keys to determine execution of the MULTI/EXEC block","O(1) for every key.","2.2.0",CMD_DOC_NONE,NULL,NULL,COMMAND_GROUP_TRANSACTIONS,WATCH_History,WATCH_tips,watchCommand,-2,CMD_NOSCRIPT|CMD_LOADING|CMD_STALE|CMD_FAST|CMD_ALLOW_BUSY,ACL_CATEGORY_TRANSACTION,{{0,KSPEC_BS_INDEX,.bs.index={1},KSPEC_FK_RANGE,.fk.range={-1,1,0}}},.args=WATCH_Args},
{0}
};
