













#if !defined(LLVM_CLANG_C_CXCOMPILATIONDATABASE_H)
#define LLVM_CLANG_C_CXCOMPILATIONDATABASE_H

#include "clang-c/CXString.h"
#include "clang-c/ExternC.h"
#include "clang-c/Platform.h"

LLVM_CLANG_C_EXTERN_C_BEGIN














typedef void * CXCompilationDatabase;










typedef void * CXCompileCommands;




typedef void * CXCompileCommand;




typedef enum {



CXCompilationDatabase_NoError = 0,




CXCompilationDatabase_CanNotLoadDatabase = 1

} CXCompilationDatabase_Error;








CINDEX_LINKAGE CXCompilationDatabase
clang_CompilationDatabase_fromDirectory(const char *BuildDir,
CXCompilationDatabase_Error *ErrorCode);




CINDEX_LINKAGE void
clang_CompilationDatabase_dispose(CXCompilationDatabase);





CINDEX_LINKAGE CXCompileCommands
clang_CompilationDatabase_getCompileCommands(CXCompilationDatabase,
const char *CompleteFileName);




CINDEX_LINKAGE CXCompileCommands
clang_CompilationDatabase_getAllCompileCommands(CXCompilationDatabase);




CINDEX_LINKAGE void clang_CompileCommands_dispose(CXCompileCommands);




CINDEX_LINKAGE unsigned
clang_CompileCommands_getSize(CXCompileCommands);






CINDEX_LINKAGE CXCompileCommand
clang_CompileCommands_getCommand(CXCompileCommands, unsigned I);




CINDEX_LINKAGE CXString
clang_CompileCommand_getDirectory(CXCompileCommand);




CINDEX_LINKAGE CXString
clang_CompileCommand_getFilename(CXCompileCommand);





CINDEX_LINKAGE unsigned
clang_CompileCommand_getNumArgs(CXCompileCommand);







CINDEX_LINKAGE CXString
clang_CompileCommand_getArg(CXCompileCommand, unsigned I);




CINDEX_LINKAGE unsigned
clang_CompileCommand_getNumMappedSources(CXCompileCommand);




CINDEX_LINKAGE CXString
clang_CompileCommand_getMappedSourcePath(CXCompileCommand, unsigned I);




CINDEX_LINKAGE CXString
clang_CompileCommand_getMappedSourceContent(CXCompileCommand, unsigned I);





LLVM_CLANG_C_EXTERN_C_END

#endif

