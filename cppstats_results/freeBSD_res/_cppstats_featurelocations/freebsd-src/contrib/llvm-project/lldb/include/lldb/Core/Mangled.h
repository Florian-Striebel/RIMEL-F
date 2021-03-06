







#if !defined(LLDB_CORE_MANGLED_H)
#define LLDB_CORE_MANGLED_H
#if defined(__cplusplus)

#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-forward.h"

#include "lldb/Utility/ConstString.h"

#include "llvm/ADT/StringRef.h"

#include <cstddef>
#include <memory>

namespace lldb_private {











class Mangled {
public:
enum NamePreference {
ePreferMangled,
ePreferDemangled,
ePreferDemangledWithoutArguments
};

enum ManglingScheme {
eManglingSchemeNone = 0,
eManglingSchemeMSVC,
eManglingSchemeItanium,
eManglingSchemeRustV0
};




Mangled() = default;








explicit Mangled(ConstString name);

explicit Mangled(llvm::StringRef name);















operator void *() const;















bool operator!() const;


void Clear();















static int Compare(const Mangled &lhs, const Mangled &rhs);








void Dump(Stream *s) const;





void DumpDebug(Stream *s) const;





ConstString GetDemangledName() const;





ConstString GetDisplayDemangledName() const;

void SetDemangledName(ConstString name) { m_demangled = name; }

void SetMangledName(ConstString name) { m_mangled = name; }





ConstString &GetMangledName() { return m_mangled; }





ConstString GetMangledName() const { return m_mangled; }










ConstString GetName(NamePreference preference = ePreferDemangled) const;








bool NameMatches(ConstString name) const {
if (m_mangled == name)
return true;
return GetDemangledName() == name;
}
bool NameMatches(const RegularExpression &regex) const;











size_t MemorySize() const;












void SetValue(ConstString name, bool is_mangled);








void SetValue(ConstString name);















lldb::LanguageType GuessLanguage() const;


using SkipMangledNameFn = bool(llvm::StringRef, ManglingScheme);























bool DemangleWithRichManglingInfo(RichManglingContext &context,
SkipMangledNameFn *skip_mangled_name);








static Mangled::ManglingScheme GetManglingScheme(llvm::StringRef const name);

private:

ConstString m_mangled;
mutable ConstString m_demangled;

};

Stream &operator<<(Stream &s, const Mangled &obj);

}

#endif
#endif
