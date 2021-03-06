












#if !defined(LLVM_CLANG_AST_DECLOBJC_COMMON_H)
#define LLVM_CLANG_AST_DECLOBJC_COMMON_H

namespace clang {



namespace ObjCPropertyAttribute {
enum Kind {
kind_noattr = 0x00,
kind_readonly = 0x01,
kind_getter = 0x02,
kind_assign = 0x04,
kind_readwrite = 0x08,
kind_retain = 0x10,
kind_copy = 0x20,
kind_nonatomic = 0x40,
kind_setter = 0x80,
kind_atomic = 0x100,
kind_weak = 0x200,
kind_strong = 0x400,
kind_unsafe_unretained = 0x800,


kind_nullability = 0x1000,
kind_null_resettable = 0x2000,
kind_class = 0x4000,
kind_direct = 0x8000,



};
}

enum {

NumObjCPropertyAttrsBits = 16
};

}

#endif
