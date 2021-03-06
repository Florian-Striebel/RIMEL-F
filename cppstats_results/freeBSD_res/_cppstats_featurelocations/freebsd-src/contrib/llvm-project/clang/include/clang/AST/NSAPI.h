







#if !defined(LLVM_CLANG_AST_NSAPI_H)
#define LLVM_CLANG_AST_NSAPI_H

#include "clang/Basic/IdentifierTable.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"

namespace clang {
class ASTContext;
class ObjCInterfaceDecl;
class QualType;
class Expr;


class NSAPI {
public:
explicit NSAPI(ASTContext &Ctx);

ASTContext &getASTContext() const { return Ctx; }

enum NSClassIdKindKind {
ClassId_NSObject,
ClassId_NSString,
ClassId_NSArray,
ClassId_NSMutableArray,
ClassId_NSDictionary,
ClassId_NSMutableDictionary,
ClassId_NSNumber,
ClassId_NSMutableSet,
ClassId_NSMutableOrderedSet,
ClassId_NSValue
};
static const unsigned NumClassIds = 10;

enum NSStringMethodKind {
NSStr_stringWithString,
NSStr_stringWithUTF8String,
NSStr_stringWithCStringEncoding,
NSStr_stringWithCString,
NSStr_initWithString,
NSStr_initWithUTF8String
};
static const unsigned NumNSStringMethods = 6;

IdentifierInfo *getNSClassId(NSClassIdKindKind K) const;


Selector getNSStringSelector(NSStringMethodKind MK) const;



bool isNSUTF8StringEncodingConstant(const Expr *E) const {
return isObjCEnumerator(E, "NSUTF8StringEncoding", NSUTF8StringEncodingId);
}



bool isNSASCIIStringEncodingConstant(const Expr *E) const {
return isObjCEnumerator(E, "NSASCIIStringEncoding",NSASCIIStringEncodingId);
}



enum NSArrayMethodKind {
NSArr_array,
NSArr_arrayWithArray,
NSArr_arrayWithObject,
NSArr_arrayWithObjects,
NSArr_arrayWithObjectsCount,
NSArr_initWithArray,
NSArr_initWithObjects,
NSArr_objectAtIndex,
NSMutableArr_replaceObjectAtIndex,
NSMutableArr_addObject,
NSMutableArr_insertObjectAtIndex,
NSMutableArr_setObjectAtIndexedSubscript
};
static const unsigned NumNSArrayMethods = 12;


Selector getNSArraySelector(NSArrayMethodKind MK) const;


Optional<NSArrayMethodKind> getNSArrayMethodKind(Selector Sel);



enum NSDictionaryMethodKind {
NSDict_dictionary,
NSDict_dictionaryWithDictionary,
NSDict_dictionaryWithObjectForKey,
NSDict_dictionaryWithObjectsForKeys,
NSDict_dictionaryWithObjectsForKeysCount,
NSDict_dictionaryWithObjectsAndKeys,
NSDict_initWithDictionary,
NSDict_initWithObjectsAndKeys,
NSDict_initWithObjectsForKeys,
NSDict_objectForKey,
NSMutableDict_setObjectForKey,
NSMutableDict_setObjectForKeyedSubscript,
NSMutableDict_setValueForKey
};
static const unsigned NumNSDictionaryMethods = 13;


Selector getNSDictionarySelector(NSDictionaryMethodKind MK) const;


Optional<NSDictionaryMethodKind> getNSDictionaryMethodKind(Selector Sel);



enum NSSetMethodKind {
NSMutableSet_addObject,
NSOrderedSet_insertObjectAtIndex,
NSOrderedSet_setObjectAtIndex,
NSOrderedSet_setObjectAtIndexedSubscript,
NSOrderedSet_replaceObjectAtIndexWithObject
};
static const unsigned NumNSSetMethods = 5;


Selector getNSSetSelector(NSSetMethodKind MK) const;


Optional<NSSetMethodKind> getNSSetMethodKind(Selector Sel);


Selector getObjectForKeyedSubscriptSelector() const {
return getOrInitSelector(StringRef("objectForKeyedSubscript"),
objectForKeyedSubscriptSel);
}


Selector getObjectAtIndexedSubscriptSelector() const {
return getOrInitSelector(StringRef("objectAtIndexedSubscript"),
objectAtIndexedSubscriptSel);
}


Selector getSetObjectForKeyedSubscriptSelector() const {
StringRef Ids[] = { "setObject", "forKeyedSubscript" };
return getOrInitSelector(Ids, setObjectForKeyedSubscriptSel);
}


Selector getSetObjectAtIndexedSubscriptSelector() const {
StringRef Ids[] = { "setObject", "atIndexedSubscript" };
return getOrInitSelector(Ids, setObjectAtIndexedSubscriptSel);
}


Selector getIsEqualSelector() const {
return getOrInitSelector(StringRef("isEqual"), isEqualSel);
}

Selector getNewSelector() const {
return getOrInitNullarySelector("new", NewSel);
}

Selector getInitSelector() const {
return getOrInitNullarySelector("init", InitSel);
}


enum NSNumberLiteralMethodKind {
NSNumberWithChar,
NSNumberWithUnsignedChar,
NSNumberWithShort,
NSNumberWithUnsignedShort,
NSNumberWithInt,
NSNumberWithUnsignedInt,
NSNumberWithLong,
NSNumberWithUnsignedLong,
NSNumberWithLongLong,
NSNumberWithUnsignedLongLong,
NSNumberWithFloat,
NSNumberWithDouble,
NSNumberWithBool,
NSNumberWithInteger,
NSNumberWithUnsignedInteger
};
static const unsigned NumNSNumberLiteralMethods = 15;




Selector getNSNumberLiteralSelector(NSNumberLiteralMethodKind MK,
bool Instance) const;

bool isNSNumberLiteralSelector(NSNumberLiteralMethodKind MK,
Selector Sel) const {
return Sel == getNSNumberLiteralSelector(MK, false) ||
Sel == getNSNumberLiteralSelector(MK, true);
}


Optional<NSNumberLiteralMethodKind>
getNSNumberLiteralMethodKind(Selector Sel) const;



Optional<NSNumberLiteralMethodKind>
getNSNumberFactoryMethodKind(QualType T) const;


bool isObjCBOOLType(QualType T) const;

bool isObjCNSIntegerType(QualType T) const;

bool isObjCNSUIntegerType(QualType T) const;


StringRef GetNSIntegralKind(QualType T) const;


bool isMacroDefined(StringRef Id) const;


bool isSubclassOfNSClass(ObjCInterfaceDecl *InterfaceDecl,
NSClassIdKindKind NSClassKind) const;

private:
bool isObjCTypedef(QualType T, StringRef name, IdentifierInfo *&II) const;
bool isObjCEnumerator(const Expr *E,
StringRef name, IdentifierInfo *&II) const;
Selector getOrInitSelector(ArrayRef<StringRef> Ids, Selector &Sel) const;
Selector getOrInitNullarySelector(StringRef Id, Selector &Sel) const;

ASTContext &Ctx;

mutable IdentifierInfo *ClassIds[NumClassIds];

mutable Selector NSStringSelectors[NumNSStringMethods];


mutable Selector NSArraySelectors[NumNSArrayMethods];


mutable Selector NSDictionarySelectors[NumNSDictionaryMethods];


mutable Selector NSSetSelectors[NumNSSetMethods];


mutable Selector NSNumberClassSelectors[NumNSNumberLiteralMethods];
mutable Selector NSNumberInstanceSelectors[NumNSNumberLiteralMethods];

mutable Selector objectForKeyedSubscriptSel, objectAtIndexedSubscriptSel,
setObjectForKeyedSubscriptSel,setObjectAtIndexedSubscriptSel,
isEqualSel, InitSel, NewSel;

mutable IdentifierInfo *BOOLId, *NSIntegerId, *NSUIntegerId;
mutable IdentifierInfo *NSASCIIStringEncodingId, *NSUTF8StringEncodingId;
};

}

#endif
