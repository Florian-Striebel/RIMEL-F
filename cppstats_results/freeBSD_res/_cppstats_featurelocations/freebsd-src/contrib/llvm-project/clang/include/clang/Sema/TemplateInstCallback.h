












#if !defined(LLVM_CLANG_TEMPLATE_INST_CALLBACK_H)
#define LLVM_CLANG_TEMPLATE_INST_CALLBACK_H

#include "clang/Sema/Sema.h"

namespace clang {



class TemplateInstantiationCallback {
public:
virtual ~TemplateInstantiationCallback() = default;


virtual void initialize(const Sema &TheSema) = 0;


virtual void finalize(const Sema &TheSema) = 0;


virtual void atTemplateBegin(const Sema &TheSema,
const Sema::CodeSynthesisContext &Inst) = 0;


virtual void atTemplateEnd(const Sema &TheSema,
const Sema::CodeSynthesisContext &Inst) = 0;
};

template <class TemplateInstantiationCallbackPtrs>
void initialize(TemplateInstantiationCallbackPtrs &Callbacks,
const Sema &TheSema) {
for (auto &C : Callbacks) {
if (C)
C->initialize(TheSema);
}
}

template <class TemplateInstantiationCallbackPtrs>
void finalize(TemplateInstantiationCallbackPtrs &Callbacks,
const Sema &TheSema) {
for (auto &C : Callbacks) {
if (C)
C->finalize(TheSema);
}
}

template <class TemplateInstantiationCallbackPtrs>
void atTemplateBegin(TemplateInstantiationCallbackPtrs &Callbacks,
const Sema &TheSema,
const Sema::CodeSynthesisContext &Inst) {
for (auto &C : Callbacks) {
if (C)
C->atTemplateBegin(TheSema, Inst);
}
}

template <class TemplateInstantiationCallbackPtrs>
void atTemplateEnd(TemplateInstantiationCallbackPtrs &Callbacks,
const Sema &TheSema,
const Sema::CodeSynthesisContext &Inst) {
for (auto &C : Callbacks) {
if (C)
C->atTemplateEnd(TheSema, Inst);
}
}

}

#endif
