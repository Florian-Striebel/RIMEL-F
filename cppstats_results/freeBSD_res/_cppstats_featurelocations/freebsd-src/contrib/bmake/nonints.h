










































































void Arch_Init(void);
void Arch_End(void);

bool Arch_ParseArchive(char **, GNodeList *, GNode *);
void Arch_Touch(GNode *);
void Arch_TouchLib(GNode *);
void Arch_UpdateMTime(GNode *gn);
void Arch_UpdateMemberMTime(GNode *gn);
void Arch_FindLib(GNode *, SearchPath *);
bool Arch_LibOODate(GNode *);
bool Arch_IsLib(GNode *);


int Compat_RunCommand(const char *, GNode *, StringListNode *);
void Compat_Run(GNodeList *);
void Compat_Make(GNode *, GNode *);


CondEvalResult Cond_EvalCondition(const char *, bool *);
CondEvalResult Cond_EvalLine(const char *);
void Cond_restore_depth(unsigned int);
unsigned int Cond_save_depth(void);



MAKE_INLINE const char *
str_basename(const char *pathname)
{
const char *lastSlash = strrchr(pathname, '/');
return lastSlash != NULL ? lastSlash + 1 : pathname;
}

MAKE_INLINE SearchPath *
SearchPath_New(void)
{
SearchPath *path = bmake_malloc(sizeof *path);
Lst_Init(&path->dirs);
return path;
}

void SearchPath_Free(SearchPath *);


int For_Eval(const char *);
bool For_Accum(const char *);
void For_Run(int);


#if defined(WAIT_T)
void JobReapChild(pid_t, WAIT_T, bool);
#endif


bool GetBooleanExpr(const char *, bool);
void Main_ParseArgLine(const char *);
char *Cmd_Exec(const char *, const char **);
void Error(const char *, ...) MAKE_ATTR_PRINTFLIKE(1, 2);
void Fatal(const char *, ...) MAKE_ATTR_PRINTFLIKE(1, 2) MAKE_ATTR_DEAD;
void Punt(const char *, ...) MAKE_ATTR_PRINTFLIKE(1, 2) MAKE_ATTR_DEAD;
void DieHorribly(void) MAKE_ATTR_DEAD;
void Finish(int) MAKE_ATTR_DEAD;
int eunlink(const char *);
void execDie(const char *, const char *);
char *getTmpdir(void);
bool ParseBoolean(const char *, bool);
char *cached_realpath(const char *, char *);


void Parse_Init(void);
void Parse_End(void);

typedef enum VarAssignOp {
VAR_NORMAL,
VAR_SUBST,
VAR_SHELL,
VAR_APPEND,
VAR_DEFAULT
} VarAssignOp;

typedef struct VarAssign {
char *varname;
VarAssignOp op;
const char *value;
} VarAssign;

typedef char *(*ReadMoreProc)(void *, size_t *);

void Parse_Error(ParseErrorLevel, const char *, ...) MAKE_ATTR_PRINTFLIKE(2, 3);
bool Parse_IsVar(const char *, VarAssign *out_var);
void Parse_Var(VarAssign *, GNode *);
void Parse_AddIncludeDir(const char *);
void Parse_File(const char *, int);
void Parse_PushInput(const char *, int, int, ReadMoreProc, void *);
void Parse_MainName(GNodeList *);
int Parse_NumErrors(void);


#if !defined(HAVE_STRLCPY)

size_t strlcpy(char *, const char *, size_t);
#endif


void Suff_Init(void);
void Suff_End(void);

void Suff_ClearSuffixes(void);
bool Suff_IsTransform(const char *);
GNode *Suff_AddTransform(const char *);
void Suff_EndTransform(GNode *);
void Suff_AddSuffix(const char *, GNode **);
SearchPath *Suff_GetPath(const char *);
void Suff_ExtendPaths(void);
void Suff_AddInclude(const char *);
void Suff_AddLib(const char *);
void Suff_FindDeps(GNode *);
SearchPath *Suff_FindPath(GNode *);
void Suff_SetNull(const char *);
void Suff_PrintAll(void);
const char *Suff_NamesStr(void);


void Targ_Init(void);
void Targ_End(void);

void Targ_Stats(void);
GNodeList *Targ_List(void);
GNode *GNode_New(const char *);
GNode *Targ_FindNode(const char *);
GNode *Targ_GetNode(const char *);
GNode *Targ_NewInternalNode(const char *);
GNode *Targ_GetEndNode(void);
void Targ_FindList(GNodeList *, StringList *);
bool Targ_Precious(const GNode *);
void Targ_SetMain(GNode *);
void Targ_PrintCmds(GNode *);
void Targ_PrintNode(GNode *, int);
void Targ_PrintNodes(GNodeList *, int);
const char *Targ_FmtTime(time_t);
void Targ_PrintType(GNodeType);
void Targ_PrintGraph(int);
void Targ_Propagate(void);
const char *GNodeMade_Name(GNodeMade);


void Var_Init(void);
void Var_End(void);

typedef enum VarEvalMode {







VARE_PARSE_ONLY,


VARE_WANTRES,





VARE_UNDEFERR,











VARE_EVAL_KEEP_DOLLAR,












VARE_EVAL_KEEP_UNDEF,





VARE_KEEP_DOLLAR_UNDEF
} VarEvalMode;

typedef enum VarSetFlags {
VAR_SET_NONE = 0,


VAR_SET_NO_EXPORT = 1 << 0,



VAR_SET_READONLY = 1 << 1
} VarSetFlags;


typedef enum VarParseResult {


VPR_OK,


VPR_ERR,












VPR_UNDEF

} VarParseResult;

typedef enum VarExportMode {

VEM_ENV,

VEM_PLAIN,

VEM_LITERAL
} VarExportMode;

void Var_Delete(GNode *, const char *);
void Var_DeleteExpand(GNode *, const char *);
void Var_Undef(const char *);
void Var_Set(GNode *, const char *, const char *);
void Var_SetExpand(GNode *, const char *, const char *);
void Var_SetWithFlags(GNode *, const char *, const char *, VarSetFlags);
void Var_SetExpandWithFlags(GNode *, const char *, const char *, VarSetFlags);
void Var_Append(GNode *, const char *, const char *);
void Var_AppendExpand(GNode *, const char *, const char *);
bool Var_Exists(GNode *, const char *);
bool Var_ExistsExpand(GNode *, const char *);
FStr Var_Value(GNode *, const char *);
const char *GNode_ValueDirect(GNode *, const char *);
VarParseResult Var_Parse(const char **, GNode *, VarEvalMode, FStr *);
VarParseResult Var_Subst(const char *, GNode *, VarEvalMode, char **);
void Var_Stats(void);
void Var_Dump(GNode *);
void Var_ReexportVars(void);
void Var_Export(VarExportMode, const char *);
void Var_ExportVars(const char *);
void Var_UnExport(bool, const char *);

void Global_Set(const char *, const char *);
void Global_SetExpand(const char *, const char *);
void Global_Append(const char *, const char *);
void Global_Delete(const char *);


typedef void (*SignalProc)(int);
SignalProc bmake_signal(int, SignalProc);
