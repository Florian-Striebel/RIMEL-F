











#if !defined(LLVM_CLANG_LEX_EXTERNALPREPROCESSORSOURCE_H)
#define LLVM_CLANG_LEX_EXTERNALPREPROCESSORSOURCE_H

namespace clang {

class IdentifierInfo;
class Module;






class ExternalPreprocessorSource {
public:
virtual ~ExternalPreprocessorSource();


virtual void ReadDefinedMacros() = 0;


virtual void updateOutOfDateIdentifier(IdentifierInfo &II) = 0;




virtual IdentifierInfo *GetIdentifier(unsigned ID) = 0;


virtual Module *getModule(unsigned ModuleID) = 0;
};

}

#endif
