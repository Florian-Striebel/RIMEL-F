


































































































#if !defined(GTEST_INCLUDE_GTEST_GTEST_PRINTERS_H_)
#define GTEST_INCLUDE_GTEST_GTEST_PRINTERS_H_

#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "gtest/internal/gtest-port.h"
#include "gtest/internal/gtest-internal.h"

#if GTEST_HAS_STD_TUPLE_
#include <tuple>
#endif

#if GTEST_HAS_ABSL
#include "absl/strings/string_view.h"
#include "absl/types/optional.h"
#include "absl/types/variant.h"
#endif

namespace testing {



namespace internal2 {



GTEST_API_ void PrintBytesInObjectTo(const unsigned char* obj_bytes,
size_t count,
::std::ostream* os);



enum TypeKind {
kProtobuf,
kConvertibleToInteger,

#if GTEST_HAS_ABSL
kConvertibleToStringView,

#endif
kOtherType
};





template <typename T, TypeKind kTypeKind>
class TypeWithoutFormatter {
public:

static void PrintValue(const T& value, ::std::ostream* os) {
PrintBytesInObjectTo(static_cast<const unsigned char*>(
reinterpret_cast<const void*>(&value)),
sizeof(value), os);
}
};




const size_t kProtobufOneLinerMaxLength = 50;

template <typename T>
class TypeWithoutFormatter<T, kProtobuf> {
public:
static void PrintValue(const T& value, ::std::ostream* os) {
std::string pretty_str = value.ShortDebugString();
if (pretty_str.length() > kProtobufOneLinerMaxLength) {
pretty_str = "\n" + value.DebugString();
}
*os << ("<" + pretty_str + ">");
}
};

template <typename T>
class TypeWithoutFormatter<T, kConvertibleToInteger> {
public:







static void PrintValue(const T& value, ::std::ostream* os) {
const internal::BiggestInt kBigInt = value;
*os << kBigInt;
}
};

#if GTEST_HAS_ABSL
template <typename T>
class TypeWithoutFormatter<T, kConvertibleToStringView> {
public:





static void PrintValue(const T& value, ::std::ostream* os);
};
#endif

























template <typename Char, typename CharTraits, typename T>
::std::basic_ostream<Char, CharTraits>& operator<<(
::std::basic_ostream<Char, CharTraits>& os, const T& x) {
TypeWithoutFormatter<T, (internal::IsAProtocolMessage<T>::value
? kProtobuf
: internal::ImplicitlyConvertible<
const T&, internal::BiggestInt>::value
? kConvertibleToInteger
:
#if GTEST_HAS_ABSL
internal::ImplicitlyConvertible<
const T&, absl::string_view>::value
? kConvertibleToStringView
:
#endif
kOtherType)>::PrintValue(x, &os);
return os;
}

}
}



namespace testing_internal {



template <typename T>
void DefaultPrintNonContainerTo(const T& value, ::std::ostream* os) {











using namespace ::testing::internal2;














*os << value;
}

}

namespace testing {
namespace internal {
















template <typename ToPrint, typename OtherOperand>
class FormatForComparison {
public:
static ::std::string Format(const ToPrint& value) {
return ::testing::PrintToString(value);
}
};


template <typename ToPrint, size_t N, typename OtherOperand>
class FormatForComparison<ToPrint[N], OtherOperand> {
public:
static ::std::string Format(const ToPrint* value) {
return FormatForComparison<const ToPrint*, OtherOperand>::Format(value);
}
};




#define GTEST_IMPL_FORMAT_C_STRING_AS_POINTER_(CharType) template <typename OtherOperand> class FormatForComparison<CharType*, OtherOperand> { public: static ::std::string Format(CharType* value) { return ::testing::PrintToString(static_cast<const void*>(value)); } }








GTEST_IMPL_FORMAT_C_STRING_AS_POINTER_(char);
GTEST_IMPL_FORMAT_C_STRING_AS_POINTER_(const char);
GTEST_IMPL_FORMAT_C_STRING_AS_POINTER_(wchar_t);
GTEST_IMPL_FORMAT_C_STRING_AS_POINTER_(const wchar_t);

#undef GTEST_IMPL_FORMAT_C_STRING_AS_POINTER_




#define GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(CharType, OtherStringType) template <> class FormatForComparison<CharType*, OtherStringType> { public: static ::std::string Format(CharType* value) { return ::testing::PrintToString(value); } }








GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(char, ::std::string);
GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(const char, ::std::string);

#if GTEST_HAS_GLOBAL_STRING
GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(char, ::string);
GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(const char, ::string);
#endif

#if GTEST_HAS_GLOBAL_WSTRING
GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(wchar_t, ::wstring);
GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(const wchar_t, ::wstring);
#endif

#if GTEST_HAS_STD_WSTRING
GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(wchar_t, ::std::wstring);
GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(const wchar_t, ::std::wstring);
#endif

#undef GTEST_IMPL_FORMAT_C_STRING_AS_STRING_









template <typename T1, typename T2>
std::string FormatForComparisonFailureMessage(
const T1& value, const T2& ) {
return FormatForComparison<T1, T2>::Format(value);
}








template <typename T>
class UniversalPrinter;

template <typename T>
void UniversalPrint(const T& value, ::std::ostream* os);

enum DefaultPrinterType {
kPrintContainer,
kPrintPointer,
kPrintFunctionPointer,
kPrintOther,
};
template <DefaultPrinterType type> struct WrapPrinterType {};



template <typename C>
void DefaultPrintTo(WrapPrinterType<kPrintContainer> ,
const C& container, ::std::ostream* os) {
const size_t kMaxCount = 32;
*os << '{';
size_t count = 0;
for (typename C::const_iterator it = container.begin();
it != container.end(); ++it, ++count) {
if (count > 0) {
*os << ',';
if (count == kMaxCount) {
*os << " ...";
break;
}
}
*os << ' ';


internal::UniversalPrint(*it, os);
}

if (count > 0) {
*os << ' ';
}
*os << '}';
}







template <typename T>
void DefaultPrintTo(WrapPrinterType<kPrintPointer> ,
T* p, ::std::ostream* os) {
if (p == NULL) {
*os << "NULL";
} else {



*os << p;
}
}
template <typename T>
void DefaultPrintTo(WrapPrinterType<kPrintFunctionPointer> ,
T* p, ::std::ostream* os) {
if (p == NULL) {
*os << "NULL";
} else {



*os << reinterpret_cast<const void*>(p);
}
}



template <typename T>
void DefaultPrintTo(WrapPrinterType<kPrintOther> ,
const T& value, ::std::ostream* os) {
::testing_internal::DefaultPrintNonContainerTo(value, os);
}












template <typename T>
void PrintTo(const T& value, ::std::ostream* os) {



















DefaultPrintTo(
WrapPrinterType <
(sizeof(IsContainerTest<T>(0)) == sizeof(IsContainer)) &&
!IsRecursiveContainer<T>::value
? kPrintContainer
: !is_pointer<T>::value
? kPrintOther
#if GTEST_LANG_CXX11
: std::is_function<typename std::remove_pointer<T>::type>::value
#else
: !internal::ImplicitlyConvertible<T, const void*>::value
#endif
? kPrintFunctionPointer
: kPrintPointer > (),
value, os);
}






GTEST_API_ void PrintTo(unsigned char c, ::std::ostream* os);
GTEST_API_ void PrintTo(signed char c, ::std::ostream* os);
inline void PrintTo(char c, ::std::ostream* os) {



PrintTo(static_cast<unsigned char>(c), os);
}


inline void PrintTo(bool x, ::std::ostream* os) {
*os << (x ? "true" : "false");
}








GTEST_API_ void PrintTo(wchar_t wc, ::std::ostream* os);


GTEST_API_ void PrintTo(const char* s, ::std::ostream* os);
inline void PrintTo(char* s, ::std::ostream* os) {
PrintTo(ImplicitCast_<const char*>(s), os);
}



inline void PrintTo(const signed char* s, ::std::ostream* os) {
PrintTo(ImplicitCast_<const void*>(s), os);
}
inline void PrintTo(signed char* s, ::std::ostream* os) {
PrintTo(ImplicitCast_<const void*>(s), os);
}
inline void PrintTo(const unsigned char* s, ::std::ostream* os) {
PrintTo(ImplicitCast_<const void*>(s), os);
}
inline void PrintTo(unsigned char* s, ::std::ostream* os) {
PrintTo(ImplicitCast_<const void*>(s), os);
}






#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)

GTEST_API_ void PrintTo(const wchar_t* s, ::std::ostream* os);
inline void PrintTo(wchar_t* s, ::std::ostream* os) {
PrintTo(ImplicitCast_<const wchar_t*>(s), os);
}
#endif






template <typename T>
void PrintRawArrayTo(const T a[], size_t count, ::std::ostream* os) {
UniversalPrint(a[0], os);
for (size_t i = 1; i != count; i++) {
*os << ", ";
UniversalPrint(a[i], os);
}
}


#if GTEST_HAS_GLOBAL_STRING
GTEST_API_ void PrintStringTo(const ::string&s, ::std::ostream* os);
inline void PrintTo(const ::string& s, ::std::ostream* os) {
PrintStringTo(s, os);
}
#endif

GTEST_API_ void PrintStringTo(const ::std::string&s, ::std::ostream* os);
inline void PrintTo(const ::std::string& s, ::std::ostream* os) {
PrintStringTo(s, os);
}


#if GTEST_HAS_GLOBAL_WSTRING
GTEST_API_ void PrintWideStringTo(const ::wstring&s, ::std::ostream* os);
inline void PrintTo(const ::wstring& s, ::std::ostream* os) {
PrintWideStringTo(s, os);
}
#endif

#if GTEST_HAS_STD_WSTRING
GTEST_API_ void PrintWideStringTo(const ::std::wstring&s, ::std::ostream* os);
inline void PrintTo(const ::std::wstring& s, ::std::ostream* os) {
PrintWideStringTo(s, os);
}
#endif

#if GTEST_HAS_ABSL

inline void PrintTo(absl::string_view sp, ::std::ostream* os) {
PrintTo(::std::string(sp), os);
}
#endif

#if GTEST_LANG_CXX11
inline void PrintTo(std::nullptr_t, ::std::ostream* os) { *os << "(nullptr)"; }
#endif

#if GTEST_HAS_TR1_TUPLE || GTEST_HAS_STD_TUPLE_


template <typename T>
void PrintTupleTo(const T& t, ::std::ostream* os);
#endif

#if GTEST_HAS_TR1_TUPLE








inline void PrintTo(const ::std::tr1::tuple<>& t, ::std::ostream* os) {
PrintTupleTo(t, os);
}

template <typename T1>
void PrintTo(const ::std::tr1::tuple<T1>& t, ::std::ostream* os) {
PrintTupleTo(t, os);
}

template <typename T1, typename T2>
void PrintTo(const ::std::tr1::tuple<T1, T2>& t, ::std::ostream* os) {
PrintTupleTo(t, os);
}

template <typename T1, typename T2, typename T3>
void PrintTo(const ::std::tr1::tuple<T1, T2, T3>& t, ::std::ostream* os) {
PrintTupleTo(t, os);
}

template <typename T1, typename T2, typename T3, typename T4>
void PrintTo(const ::std::tr1::tuple<T1, T2, T3, T4>& t, ::std::ostream* os) {
PrintTupleTo(t, os);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
void PrintTo(const ::std::tr1::tuple<T1, T2, T3, T4, T5>& t,
::std::ostream* os) {
PrintTupleTo(t, os);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6>
void PrintTo(const ::std::tr1::tuple<T1, T2, T3, T4, T5, T6>& t,
::std::ostream* os) {
PrintTupleTo(t, os);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6, typename T7>
void PrintTo(const ::std::tr1::tuple<T1, T2, T3, T4, T5, T6, T7>& t,
::std::ostream* os) {
PrintTupleTo(t, os);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6, typename T7, typename T8>
void PrintTo(const ::std::tr1::tuple<T1, T2, T3, T4, T5, T6, T7, T8>& t,
::std::ostream* os) {
PrintTupleTo(t, os);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6, typename T7, typename T8, typename T9>
void PrintTo(const ::std::tr1::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9>& t,
::std::ostream* os) {
PrintTupleTo(t, os);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6, typename T7, typename T8, typename T9, typename T10>
void PrintTo(
const ::std::tr1::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>& t,
::std::ostream* os) {
PrintTupleTo(t, os);
}
#endif

#if GTEST_HAS_STD_TUPLE_
template <typename... Types>
void PrintTo(const ::std::tuple<Types...>& t, ::std::ostream* os) {
PrintTupleTo(t, os);
}
#endif


template <typename T1, typename T2>
void PrintTo(const ::std::pair<T1, T2>& value, ::std::ostream* os) {
*os << '(';


UniversalPrinter<T1>::Print(value.first, os);
*os << ", ";
UniversalPrinter<T2>::Print(value.second, os);
*os << ')';
}



template <typename T>
class UniversalPrinter {
public:


GTEST_DISABLE_MSC_WARNINGS_PUSH_(4180)




static void Print(const T& value, ::std::ostream* os) {








PrintTo(value, os);
}

GTEST_DISABLE_MSC_WARNINGS_POP_()
};

#if GTEST_HAS_ABSL



template <typename T>
class UniversalPrinter<::absl::optional<T>> {
public:
static void Print(const ::absl::optional<T>& value, ::std::ostream* os) {
*os << '(';
if (!value) {
*os << "nullopt";
} else {
UniversalPrint(*value, os);
}
*os << ')';
}
};



template <typename... T>
class UniversalPrinter<::absl::variant<T...>> {
public:
static void Print(const ::absl::variant<T...>& value, ::std::ostream* os) {
*os << '(';
absl::visit(Visitor{os}, value);
*os << ')';
}

private:
struct Visitor {
template <typename U>
void operator()(const U& u) const {
*os << "'" << GetTypeName<U>() << "' with value ";
UniversalPrint(u, os);
}
::std::ostream* os;
};
};

#endif



template <typename T>
void UniversalPrintArray(const T* begin, size_t len, ::std::ostream* os) {
if (len == 0) {
*os << "{}";
} else {
*os << "{ ";
const size_t kThreshold = 18;
const size_t kChunkSize = 8;




if (len <= kThreshold) {
PrintRawArrayTo(begin, len, os);
} else {
PrintRawArrayTo(begin, kChunkSize, os);
*os << ", ..., ";
PrintRawArrayTo(begin + len - kChunkSize, kChunkSize, os);
}
*os << " }";
}
}

GTEST_API_ void UniversalPrintArray(
const char* begin, size_t len, ::std::ostream* os);


GTEST_API_ void UniversalPrintArray(
const wchar_t* begin, size_t len, ::std::ostream* os);


template <typename T, size_t N>
class UniversalPrinter<T[N]> {
public:


static void Print(const T (&a)[N], ::std::ostream* os) {
UniversalPrintArray(a, N, os);
}
};


template <typename T>
class UniversalPrinter<T&> {
public:


GTEST_DISABLE_MSC_WARNINGS_PUSH_(4180)

static void Print(const T& value, ::std::ostream* os) {


*os << "@" << reinterpret_cast<const void*>(&value) << " ";


UniversalPrint(value, os);
}

GTEST_DISABLE_MSC_WARNINGS_POP_()
};





template <typename T>
class UniversalTersePrinter {
public:
static void Print(const T& value, ::std::ostream* os) {
UniversalPrint(value, os);
}
};
template <typename T>
class UniversalTersePrinter<T&> {
public:
static void Print(const T& value, ::std::ostream* os) {
UniversalPrint(value, os);
}
};
template <typename T, size_t N>
class UniversalTersePrinter<T[N]> {
public:
static void Print(const T (&value)[N], ::std::ostream* os) {
UniversalPrinter<T[N]>::Print(value, os);
}
};
template <>
class UniversalTersePrinter<const char*> {
public:
static void Print(const char* str, ::std::ostream* os) {
if (str == NULL) {
*os << "NULL";
} else {
UniversalPrint(std::string(str), os);
}
}
};
template <>
class UniversalTersePrinter<char*> {
public:
static void Print(char* str, ::std::ostream* os) {
UniversalTersePrinter<const char*>::Print(str, os);
}
};

#if GTEST_HAS_STD_WSTRING
template <>
class UniversalTersePrinter<const wchar_t*> {
public:
static void Print(const wchar_t* str, ::std::ostream* os) {
if (str == NULL) {
*os << "NULL";
} else {
UniversalPrint(::std::wstring(str), os);
}
}
};
#endif

template <>
class UniversalTersePrinter<wchar_t*> {
public:
static void Print(wchar_t* str, ::std::ostream* os) {
UniversalTersePrinter<const wchar_t*>::Print(str, os);
}
};

template <typename T>
void UniversalTersePrint(const T& value, ::std::ostream* os) {
UniversalTersePrinter<T>::Print(value, os);
}





template <typename T>
void UniversalPrint(const T& value, ::std::ostream* os) {


typedef T T1;
UniversalPrinter<T1>::Print(value, os);
}

typedef ::std::vector< ::std::string> Strings;








template <typename TupleT>
struct TuplePolicy;

#if GTEST_HAS_TR1_TUPLE
template <typename TupleT>
struct TuplePolicy {
typedef TupleT Tuple;
static const size_t tuple_size = ::std::tr1::tuple_size<Tuple>::value;

template <size_t I>
struct tuple_element : ::std::tr1::tuple_element<static_cast<int>(I), Tuple> {
};

template <size_t I>
static typename AddReference<const typename ::std::tr1::tuple_element<
static_cast<int>(I), Tuple>::type>::type
get(const Tuple& tuple) {
return ::std::tr1::get<I>(tuple);
}
};
template <typename TupleT>
const size_t TuplePolicy<TupleT>::tuple_size;
#endif

#if GTEST_HAS_STD_TUPLE_
template <typename... Types>
struct TuplePolicy< ::std::tuple<Types...> > {
typedef ::std::tuple<Types...> Tuple;
static const size_t tuple_size = ::std::tuple_size<Tuple>::value;

template <size_t I>
struct tuple_element : ::std::tuple_element<I, Tuple> {};

template <size_t I>
static const typename ::std::tuple_element<I, Tuple>::type& get(
const Tuple& tuple) {
return ::std::get<I>(tuple);
}
};
template <typename... Types>
const size_t TuplePolicy< ::std::tuple<Types...> >::tuple_size;
#endif

#if GTEST_HAS_TR1_TUPLE || GTEST_HAS_STD_TUPLE_








template <size_t N>
struct TuplePrefixPrinter {

template <typename Tuple>
static void PrintPrefixTo(const Tuple& t, ::std::ostream* os) {
TuplePrefixPrinter<N - 1>::PrintPrefixTo(t, os);
GTEST_INTENTIONAL_CONST_COND_PUSH_()
if (N > 1) {
GTEST_INTENTIONAL_CONST_COND_POP_()
*os << ", ";
}
UniversalPrinter<
typename TuplePolicy<Tuple>::template tuple_element<N - 1>::type>
::Print(TuplePolicy<Tuple>::template get<N - 1>(t), os);
}



template <typename Tuple>
static void TersePrintPrefixToStrings(const Tuple& t, Strings* strings) {
TuplePrefixPrinter<N - 1>::TersePrintPrefixToStrings(t, strings);
::std::stringstream ss;
UniversalTersePrint(TuplePolicy<Tuple>::template get<N - 1>(t), &ss);
strings->push_back(ss.str());
}
};


template <>
struct TuplePrefixPrinter<0> {
template <typename Tuple>
static void PrintPrefixTo(const Tuple&, ::std::ostream*) {}

template <typename Tuple>
static void TersePrintPrefixToStrings(const Tuple&, Strings*) {}
};



template <typename Tuple>
void PrintTupleTo(const Tuple& t, ::std::ostream* os) {
*os << "(";
TuplePrefixPrinter<TuplePolicy<Tuple>::tuple_size>::PrintPrefixTo(t, os);
*os << ")";
}




template <typename Tuple>
Strings UniversalTersePrintTupleFieldsToStrings(const Tuple& value) {
Strings result;
TuplePrefixPrinter<TuplePolicy<Tuple>::tuple_size>::
TersePrintPrefixToStrings(value, &result);
return result;
}
#endif

}

#if GTEST_HAS_ABSL
namespace internal2 {
template <typename T>
void TypeWithoutFormatter<T, kConvertibleToStringView>::PrintValue(
const T& value, ::std::ostream* os) {
internal::PrintTo(absl::string_view(value), os);
}
}
#endif

template <typename T>
::std::string PrintToString(const T& value) {
::std::stringstream ss;
internal::UniversalTersePrinter<T>::Print(value, &ss);
return ss.str();
}

}




#include "gtest/internal/custom/gtest-printers.h"

#endif
