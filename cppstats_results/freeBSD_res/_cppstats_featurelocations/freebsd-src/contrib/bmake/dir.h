









































































#if !defined(MAKE_DIR_H)
#define MAKE_DIR_H

typedef struct CachedDir CachedDir;

void Dir_Init(void);
void Dir_InitCur(const char *);
void Dir_InitDot(void);
void Dir_End(void);
void Dir_SetPATH(void);
bool Dir_HasWildcards(const char *);
void SearchPath_Expand(SearchPath *, const char *, StringList *);
char *Dir_FindFile(const char *, SearchPath *);
char *Dir_FindHereOrAbove(const char *, const char *);
void Dir_UpdateMTime(GNode *, bool);
CachedDir *SearchPath_Add(SearchPath *, const char *);
char *SearchPath_ToFlags(SearchPath *, const char *);
void SearchPath_Clear(SearchPath *);
void SearchPath_AddAll(SearchPath *, SearchPath *);
void Dir_PrintDirectories(void);
void SearchPath_Print(const SearchPath *);
SearchPath *Dir_CopyDirSearchPath(void);


struct cached_stat {
time_t cst_mtime;
mode_t cst_mode;
};

int cached_lstat(const char *, struct cached_stat *);
int cached_stat(const char *, struct cached_stat *);

#endif
