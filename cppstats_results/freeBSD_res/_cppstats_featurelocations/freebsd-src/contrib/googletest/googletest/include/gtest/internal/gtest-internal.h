



































#if !defined(GTEST_INCLUDE_GTEST_INTERNAL_GTEST_INTERNAL_H_)
#define GTEST_INCLUDE_GTEST_INTERNAL_GTEST_INTERNAL_H_

#include "gtest/internal/gtest-port.h"

#if GTEST_OS_LINUX
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#if GTEST_HAS_EXCEPTIONS
#include <stdexcept>
#endif

#include <ctype.h>
#include <float.h>
#include <string.h>
#include <iomanip>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "gtest/gtest-message.h"
#include "gtest/internal/gtest-filepath.h"
#include "gtest/internal/gtest-string.h"
#include "gtest/internal/gtest-type-util.h"









#define GTEST_CONCAT_TOKEN_(foo, bar) GTEST_CONCAT_TOKEN_IMPL_(foo, bar)
#define GTEST_CONCAT_TOKEN_IMPL_(foo, bar) foo ##bar


#define GTEST_STRINGIFY_(name) #name

class ProtocolMessage;
namespace proto2 { class Message; }

namespace testing {



class AssertionResult;
class Message;
class Test;
class TestInfo;
class TestPartResult;
class UnitTest;

template <typename T>
::std::string PrintToString(const T& value);

namespace internal {

struct TraceInfo;
class TestInfoImpl;
class UnitTestImpl;



GTEST_API_ extern const char kStackTraceMarker[];















char IsNullLiteralHelper(Secret* p);
char (&IsNullLiteralHelper(...))[2];




#if defined(GTEST_ELLIPSIS_NEEDS_POD_)


#define GTEST_IS_NULL_LITERAL_(x) false
#else
#define GTEST_IS_NULL_LITERAL_(x) (sizeof(::testing::internal::IsNullLiteralHelper(x)) == 1)

#endif


GTEST_API_ std::string AppendUserMessage(
const std::string& gtest_msg, const Message& user_msg);

#if GTEST_HAS_EXCEPTIONS

GTEST_DISABLE_MSC_WARNINGS_PUSH_(4275 \
)







class GTEST_API_ GoogleTestFailureException : public ::std::runtime_error {
public:
explicit GoogleTestFailureException(const TestPartResult& failure);
};

GTEST_DISABLE_MSC_WARNINGS_POP_()

#endif

namespace edit_distance {





enum EditType { kMatch, kAdd, kRemove, kReplace };
GTEST_API_ std::vector<EditType> CalculateOptimalEdits(
const std::vector<size_t>& left, const std::vector<size_t>& right);


GTEST_API_ std::vector<EditType> CalculateOptimalEdits(
const std::vector<std::string>& left,
const std::vector<std::string>& right);


GTEST_API_ std::string CreateUnifiedDiff(const std::vector<std::string>& left,
const std::vector<std::string>& right,
size_t context = 2);

}





GTEST_API_ std::string DiffStrings(const std::string& left,
const std::string& right,
size_t* total_line_count);
















GTEST_API_ AssertionResult EqFailure(const char* expected_expression,
const char* actual_expression,
const std::string& expected_value,
const std::string& actual_value,
bool ignoring_case);


GTEST_API_ std::string GetBoolAssertionFailureMessage(
const AssertionResult& assertion_result,
const char* expression_text,
const char* actual_predicate_value,
const char* expected_predicate_value);






























template <typename RawType>
class FloatingPoint {
public:


typedef typename TypeWithSize<sizeof(RawType)>::UInt Bits;




static const size_t kBitCount = 8*sizeof(RawType);


static const size_t kFractionBitCount =
std::numeric_limits<RawType>::digits - 1;


static const size_t kExponentBitCount = kBitCount - 1 - kFractionBitCount;


static const Bits kSignBitMask = static_cast<Bits>(1) << (kBitCount - 1);


static const Bits kFractionBitMask =
~static_cast<Bits>(0) >> (kExponentBitCount + 1);


static const Bits kExponentBitMask = ~(kSignBitMask | kFractionBitMask);













static const size_t kMaxUlps = 4;







explicit FloatingPoint(const RawType& x) { u_.value_ = x; }






static RawType ReinterpretBits(const Bits bits) {
FloatingPoint fp(0);
fp.u_.bits_ = bits;
return fp.u_.value_;
}


static RawType Infinity() {
return ReinterpretBits(kExponentBitMask);
}


static RawType Max();




const Bits &bits() const { return u_.bits_; }


Bits exponent_bits() const { return kExponentBitMask & u_.bits_; }


Bits fraction_bits() const { return kFractionBitMask & u_.bits_; }


Bits sign_bit() const { return kSignBitMask & u_.bits_; }


bool is_nan() const {


return (exponent_bits() == kExponentBitMask) && (fraction_bits() != 0);
}







bool AlmostEquals(const FloatingPoint& rhs) const {


if (is_nan() || rhs.is_nan()) return false;

return DistanceBetweenSignAndMagnitudeNumbers(u_.bits_, rhs.u_.bits_)
<= kMaxUlps;
}

private:

union FloatingPointUnion {
RawType value_;
Bits bits_;
};
















static Bits SignAndMagnitudeToBiased(const Bits &sam) {
if (kSignBitMask & sam) {

return ~sam + 1;
} else {

return kSignBitMask | sam;
}
}



static Bits DistanceBetweenSignAndMagnitudeNumbers(const Bits &sam1,
const Bits &sam2) {
const Bits biased1 = SignAndMagnitudeToBiased(sam1);
const Bits biased2 = SignAndMagnitudeToBiased(sam2);
return (biased1 >= biased2) ? (biased1 - biased2) : (biased2 - biased1);
}

FloatingPointUnion u_;
};



template <>
inline float FloatingPoint<float>::Max() { return FLT_MAX; }
template <>
inline double FloatingPoint<double>::Max() { return DBL_MAX; }



typedef FloatingPoint<float> Float;
typedef FloatingPoint<double> Double;







typedef const void* TypeId;

template <typename T>
class TypeIdHelper {
public:



static bool dummy_;
};

template <typename T>
bool TypeIdHelper<T>::dummy_ = false;




template <typename T>
TypeId GetTypeId() {




return &(TypeIdHelper<T>::dummy_);
}






GTEST_API_ TypeId GetTestTypeId();



class TestFactoryBase {
public:
virtual ~TestFactoryBase() {}



virtual Test* CreateTest() = 0;

protected:
TestFactoryBase() {}

private:
GTEST_DISALLOW_COPY_AND_ASSIGN_(TestFactoryBase);
};



template <class TestClass>
class TestFactoryImpl : public TestFactoryBase {
public:
virtual Test* CreateTest() { return new TestClass; }
};

#if GTEST_OS_WINDOWS





GTEST_API_ AssertionResult IsHRESULTSuccess(const char* expr,
long hr);
GTEST_API_ AssertionResult IsHRESULTFailure(const char* expr,
long hr);

#endif


typedef void (*SetUpTestCaseFunc)();
typedef void (*TearDownTestCaseFunc)();

struct CodeLocation {
CodeLocation(const std::string& a_file, int a_line)
: file(a_file), line(a_line) {}

std::string file;
int line;
};



















GTEST_API_ TestInfo* MakeAndRegisterTestInfo(
const char* test_case_name,
const char* name,
const char* type_param,
const char* value_param,
CodeLocation code_location,
TypeId fixture_class_id,
SetUpTestCaseFunc set_up_tc,
TearDownTestCaseFunc tear_down_tc,
TestFactoryBase* factory);




GTEST_API_ bool SkipPrefix(const char* prefix, const char** pstr);

#if GTEST_HAS_TYPED_TEST || GTEST_HAS_TYPED_TEST_P

GTEST_DISABLE_MSC_WARNINGS_PUSH_(4251 \
)


class GTEST_API_ TypedTestCasePState {
public:
TypedTestCasePState() : registered_(false) {}




bool AddTestName(const char* file, int line, const char* case_name,
const char* test_name) {
if (registered_) {
fprintf(stderr, "%s Test %s must be defined before "
"REGISTER_TYPED_TEST_CASE_P(%s, ...).\n",
FormatFileLocation(file, line).c_str(), test_name, case_name);
fflush(stderr);
posix::Abort();
}
registered_tests_.insert(
::std::make_pair(test_name, CodeLocation(file, line)));
return true;
}

bool TestExists(const std::string& test_name) const {
return registered_tests_.count(test_name) > 0;
}

const CodeLocation& GetCodeLocation(const std::string& test_name) const {
RegisteredTestsMap::const_iterator it = registered_tests_.find(test_name);
GTEST_CHECK_(it != registered_tests_.end());
return it->second;
}




const char* VerifyRegisteredTestNames(
const char* file, int line, const char* registered_tests);

private:
typedef ::std::map<std::string, CodeLocation> RegisteredTestsMap;

bool registered_;
RegisteredTestsMap registered_tests_;
};

GTEST_DISABLE_MSC_WARNINGS_POP_()



inline const char* SkipComma(const char* str) {
const char* comma = strchr(str, ',');
if (comma == NULL) {
return NULL;
}
while (IsSpace(*(++comma))) {}
return comma;
}



inline std::string GetPrefixUntilComma(const char* str) {
const char* comma = strchr(str, ',');
return comma == NULL ? str : std::string(str, comma);
}



void SplitString(const ::std::string& str, char delimiter,
::std::vector< ::std::string>* dest);



struct DefaultNameGenerator {
template <typename T>
static std::string GetName(int i) {
return StreamableToString(i);
}
};

template <typename Provided = DefaultNameGenerator>
struct NameGeneratorSelector {
typedef Provided type;
};

template <typename NameGenerator>
void GenerateNamesRecursively(Types0, std::vector<std::string>*, int) {}

template <typename NameGenerator, typename Types>
void GenerateNamesRecursively(Types, std::vector<std::string>* result, int i) {
result->push_back(NameGenerator::template GetName<typename Types::Head>(i));
GenerateNamesRecursively<NameGenerator>(typename Types::Tail(), result,
i + 1);
}

template <typename NameGenerator, typename Types>
std::vector<std::string> GenerateNames() {
std::vector<std::string> result;
GenerateNamesRecursively<NameGenerator>(Types(), &result, 0);
return result;
}








template <GTEST_TEMPLATE_ Fixture, class TestSel, typename Types>
class TypeParameterizedTest {
public:




static bool Register(const char* prefix, const CodeLocation& code_location,
const char* case_name, const char* test_names, int index,
const std::vector<std::string>& type_names =
GenerateNames<DefaultNameGenerator, Types>()) {
typedef typename Types::Head Type;
typedef Fixture<Type> FixtureClass;
typedef typename GTEST_BIND_(TestSel, Type) TestClass;



MakeAndRegisterTestInfo(
(std::string(prefix) + (prefix[0] == '\0' ? "" : "/") + case_name +
"/" + type_names[index])
.c_str(),
StripTrailingSpaces(GetPrefixUntilComma(test_names)).c_str(),
GetTypeName<Type>().c_str(),
NULL,
code_location, GetTypeId<FixtureClass>(), TestClass::SetUpTestCase,
TestClass::TearDownTestCase, new TestFactoryImpl<TestClass>);


return TypeParameterizedTest<Fixture, TestSel,
typename Types::Tail>::Register(prefix,
code_location,
case_name,
test_names,
index + 1,
type_names);
}
};


template <GTEST_TEMPLATE_ Fixture, class TestSel>
class TypeParameterizedTest<Fixture, TestSel, Types0> {
public:
static bool Register(const char* , const CodeLocation&,
const char* , const char* ,
int ,
const std::vector<std::string>& =
std::vector<std::string>() ) {
return true;
}
};





template <GTEST_TEMPLATE_ Fixture, typename Tests, typename Types>
class TypeParameterizedTestCase {
public:
static bool Register(const char* prefix, CodeLocation code_location,
const TypedTestCasePState* state, const char* case_name,
const char* test_names,
const std::vector<std::string>& type_names =
GenerateNames<DefaultNameGenerator, Types>()) {
std::string test_name = StripTrailingSpaces(
GetPrefixUntilComma(test_names));
if (!state->TestExists(test_name)) {
fprintf(stderr, "Failed to get code location for test %s.%s at %s.",
case_name, test_name.c_str(),
FormatFileLocation(code_location.file.c_str(),
code_location.line).c_str());
fflush(stderr);
posix::Abort();
}
const CodeLocation& test_location = state->GetCodeLocation(test_name);

typedef typename Tests::Head Head;


TypeParameterizedTest<Fixture, Head, Types>::Register(
prefix, test_location, case_name, test_names, 0, type_names);


return TypeParameterizedTestCase<Fixture, typename Tests::Tail,
Types>::Register(prefix, code_location,
state, case_name,
SkipComma(test_names),
type_names);
}
};


template <GTEST_TEMPLATE_ Fixture, typename Types>
class TypeParameterizedTestCase<Fixture, Templates0, Types> {
public:
static bool Register(const char* , const CodeLocation&,
const TypedTestCasePState* ,
const char* , const char* ,
const std::vector<std::string>& =
std::vector<std::string>() ) {
return true;
}
};

#endif











GTEST_API_ std::string GetCurrentOsStackTraceExceptTop(
UnitTest* unit_test, int skip_count);





GTEST_API_ bool AlwaysTrue();


inline bool AlwaysFalse() { return !AlwaysTrue(); }




struct GTEST_API_ ConstCharPtr {
ConstCharPtr(const char* str) : value(str) {}
operator bool() const { return true; }
const char* value;
};






class GTEST_API_ Random {
public:
static const UInt32 kMaxRange = 1u << 31;

explicit Random(UInt32 seed) : state_(seed) {}

void Reseed(UInt32 seed) { state_ = seed; }



UInt32 Generate(UInt32 range);

private:
UInt32 state_;
GTEST_DISALLOW_COPY_AND_ASSIGN_(Random);
};



template <typename T1, typename T2>
struct CompileAssertTypesEqual;

template <typename T>
struct CompileAssertTypesEqual<T, T> {
};




template <typename T>
struct RemoveReference { typedef T type; };
template <typename T>
struct RemoveReference<T&> { typedef T type; };



#define GTEST_REMOVE_REFERENCE_(T) typename ::testing::internal::RemoveReference<T>::type





template <typename T>
struct RemoveConst { typedef T type; };
template <typename T>
struct RemoveConst<const T> { typedef T type; };




template <typename T, size_t N>
struct RemoveConst<const T[N]> {
typedef typename RemoveConst<T>::type type[N];
};

#if defined(_MSC_VER) && _MSC_VER < 1400



template <typename T, size_t N>
struct RemoveConst<T[N]> {
typedef typename RemoveConst<T>::type type[N];
};
#endif



#define GTEST_REMOVE_CONST_(T) typename ::testing::internal::RemoveConst<T>::type



#define GTEST_REMOVE_REFERENCE_AND_CONST_(T) GTEST_REMOVE_CONST_(GTEST_REMOVE_REFERENCE_(T))





template <typename From, typename To>
class ImplicitlyConvertible {
private:






static typename AddReference<From>::type MakeFrom();











static char Helper(To);
static char (&Helper(...))[2];



public:
#if defined(__BORLANDC__)



static const bool value = __is_convertible(From, To);
#else



GTEST_DISABLE_MSC_WARNINGS_PUSH_(4244)
static const bool value =
sizeof(Helper(ImplicitlyConvertible::MakeFrom())) == 1;
GTEST_DISABLE_MSC_WARNINGS_POP_()
#endif
};
template <typename From, typename To>
const bool ImplicitlyConvertible<From, To>::value;




template <typename T>
struct IsAProtocolMessage
: public bool_constant<
ImplicitlyConvertible<const T*, const ::ProtocolMessage*>::value ||
ImplicitlyConvertible<const T*, const ::proto2::Message*>::value> {
};

























typedef int IsContainer;
#if GTEST_LANG_CXX11
template <class C,
class Iterator = decltype(::std::declval<const C&>().begin()),
class = decltype(::std::declval<const C&>().end()),
class = decltype(++::std::declval<Iterator&>()),
class = decltype(*::std::declval<Iterator>()),
class = typename C::const_iterator>
IsContainer IsContainerTest(int ) {
return 0;
}
#else
template <class C>
IsContainer IsContainerTest(int ,
typename C::iterator* = NULL,
typename C::const_iterator* = NULL) {
return 0;
}
#endif

typedef char IsNotContainer;
template <class C>
IsNotContainer IsContainerTest(long ) { return '\0'; }





template <typename T>
struct IsHashTable {
private:
template <typename U>
static char test(typename U::hasher*, typename U::reverse_iterator*);
template <typename U>
static int test(typename U::hasher*, ...);
template <typename U>
static char test(...);

public:
static const bool value = sizeof(test<T>(0, 0)) == sizeof(int);
};

template <typename T>
const bool IsHashTable<T>::value;

template<typename T>
struct VoidT {
typedef void value_type;
};

template <typename T, typename = void>
struct HasValueType : false_type {};
template <typename T>
struct HasValueType<T, VoidT<typename T::value_type> > : true_type {
};

template <typename C,
bool = sizeof(IsContainerTest<C>(0)) == sizeof(IsContainer),
bool = HasValueType<C>::value>
struct IsRecursiveContainerImpl;

template <typename C, bool HV>
struct IsRecursiveContainerImpl<C, false, HV> : public false_type {};





template <typename C>
struct IsRecursiveContainerImpl<C, true, false> : public false_type {};

template <typename C>
struct IsRecursiveContainerImpl<C, true, true> {
#if GTEST_LANG_CXX11
typedef typename IteratorTraits<typename C::const_iterator>::value_type
value_type;
#else
typedef typename IteratorTraits<typename C::iterator>::value_type value_type;
#endif
typedef is_same<value_type, C> type;
};







template <typename C>
struct IsRecursiveContainer : public IsRecursiveContainerImpl<C>::type {};





template<bool> struct EnableIf;
template<> struct EnableIf<true> { typedef void type; };







template <typename T, typename U>
bool ArrayEq(const T* lhs, size_t size, const U* rhs);


template <typename T, typename U>
inline bool ArrayEq(const T& lhs, const U& rhs) { return lhs == rhs; }


template <typename T, typename U, size_t N>
inline bool ArrayEq(const T(&lhs)[N], const U(&rhs)[N]) {
return internal::ArrayEq(lhs, N, rhs);
}




template <typename T, typename U>
bool ArrayEq(const T* lhs, size_t size, const U* rhs) {
for (size_t i = 0; i != size; i++) {
if (!internal::ArrayEq(lhs[i], rhs[i]))
return false;
}
return true;
}



template <typename Iter, typename Element>
Iter ArrayAwareFind(Iter begin, Iter end, const Element& elem) {
for (Iter it = begin; it != end; ++it) {
if (internal::ArrayEq(*it, elem))
return it;
}
return end;
}





template <typename T, typename U>
void CopyArray(const T* from, size_t size, U* to);


template <typename T, typename U>
inline void CopyArray(const T& from, U* to) { *to = from; }


template <typename T, typename U, size_t N>
inline void CopyArray(const T(&from)[N], U(*to)[N]) {
internal::CopyArray(from, N, *to);
}




template <typename T, typename U>
void CopyArray(const T* from, size_t size, U* to) {
for (size_t i = 0; i != size; i++) {
internal::CopyArray(from[i], to + i);
}
}





struct RelationToSourceReference {};
struct RelationToSourceCopy {};









template <typename Element>
class NativeArray {
public:

typedef Element value_type;
typedef Element* iterator;
typedef const Element* const_iterator;


NativeArray(const Element* array, size_t count, RelationToSourceReference) {
InitRef(array, count);
}


NativeArray(const Element* array, size_t count, RelationToSourceCopy) {
InitCopy(array, count);
}


NativeArray(const NativeArray& rhs) {
(this->*rhs.clone_)(rhs.array_, rhs.size_);
}

~NativeArray() {
if (clone_ != &NativeArray::InitRef)
delete[] array_;
}


size_t size() const { return size_; }
const_iterator begin() const { return array_; }
const_iterator end() const { return array_ + size_; }
bool operator==(const NativeArray& rhs) const {
return size() == rhs.size() &&
ArrayEq(begin(), size(), rhs.begin());
}

private:
enum {
kCheckTypeIsNotConstOrAReference = StaticAssertTypeEqHelper<
Element, GTEST_REMOVE_REFERENCE_AND_CONST_(Element)>::value
};


void InitCopy(const Element* array, size_t a_size) {
Element* const copy = new Element[a_size];
CopyArray(array, a_size, copy);
array_ = copy;
size_ = a_size;
clone_ = &NativeArray::InitCopy;
}


void InitRef(const Element* array, size_t a_size) {
array_ = array;
size_ = a_size;
clone_ = &NativeArray::InitRef;
}

const Element* array_;
size_t size_;
void (NativeArray::*clone_)(const Element*, size_t);

GTEST_DISALLOW_ASSIGN_(NativeArray);
};

}
}

#define GTEST_MESSAGE_AT_(file, line, message, result_type) ::testing::internal::AssertHelper(result_type, file, line, message) = ::testing::Message()



#define GTEST_MESSAGE_(message, result_type) GTEST_MESSAGE_AT_(__FILE__, __LINE__, message, result_type)


#define GTEST_FATAL_FAILURE_(message) return GTEST_MESSAGE_(message, ::testing::TestPartResult::kFatalFailure)


#define GTEST_NONFATAL_FAILURE_(message) GTEST_MESSAGE_(message, ::testing::TestPartResult::kNonFatalFailure)


#define GTEST_SUCCESS_(message) GTEST_MESSAGE_(message, ::testing::TestPartResult::kSuccess)


#define GTEST_SKIP_(message) return GTEST_MESSAGE_(message, ::testing::TestPartResult::kSkip)





#define GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement) if (::testing::internal::AlwaysTrue()) { statement; }


#define GTEST_TEST_THROW_(statement, expected_exception, fail) GTEST_AMBIGUOUS_ELSE_BLOCKER_ if (::testing::internal::ConstCharPtr gtest_msg = "") { bool gtest_caught_expected = false; try { GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); } catch (expected_exception const&) { gtest_caught_expected = true; } catch (...) { gtest_msg.value = "Expected: " #statement " throws an exception of type " #expected_exception ".\n Actual: it throws a different type."; goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); } if (!gtest_caught_expected) { gtest_msg.value = "Expected: " #statement " throws an exception of type " #expected_exception ".\n Actual: it throws nothing."; goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); } } else GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__): fail(gtest_msg.value)

























#define GTEST_TEST_NO_THROW_(statement, fail) GTEST_AMBIGUOUS_ELSE_BLOCKER_ if (::testing::internal::AlwaysTrue()) { try { GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); } catch (...) { goto GTEST_CONCAT_TOKEN_(gtest_label_testnothrow_, __LINE__); } } else GTEST_CONCAT_TOKEN_(gtest_label_testnothrow_, __LINE__): fail("Expected: " #statement " doesn't throw an exception.\n" " Actual: it throws.")













#define GTEST_TEST_ANY_THROW_(statement, fail) GTEST_AMBIGUOUS_ELSE_BLOCKER_ if (::testing::internal::AlwaysTrue()) { bool gtest_caught_any = false; try { GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); } catch (...) { gtest_caught_any = true; } if (!gtest_caught_any) { goto GTEST_CONCAT_TOKEN_(gtest_label_testanythrow_, __LINE__); } } else GTEST_CONCAT_TOKEN_(gtest_label_testanythrow_, __LINE__): fail("Expected: " #statement " throws an exception.\n" " Actual: it doesn't.")





















#define GTEST_TEST_BOOLEAN_(expression, text, actual, expected, fail) GTEST_AMBIGUOUS_ELSE_BLOCKER_ if (const ::testing::AssertionResult gtest_ar_ = ::testing::AssertionResult(expression)) ; else fail(::testing::internal::GetBoolAssertionFailureMessage(gtest_ar_, text, #actual, #expected).c_str())








#define GTEST_TEST_NO_FATAL_FAILURE_(statement, fail) GTEST_AMBIGUOUS_ELSE_BLOCKER_ if (::testing::internal::AlwaysTrue()) { ::testing::internal::HasNewFatalFailureHelper gtest_fatal_failure_checker; GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); if (gtest_fatal_failure_checker.has_new_fatal_failure()) { goto GTEST_CONCAT_TOKEN_(gtest_label_testnofatal_, __LINE__); } } else GTEST_CONCAT_TOKEN_(gtest_label_testnofatal_, __LINE__): fail("Expected: " #statement " doesn't generate new fatal " "failures in the current thread.\n" " Actual: it does.")














#define GTEST_TEST_CLASS_NAME_(test_case_name, test_name) test_case_name##_##test_name##_Test



#define GTEST_TEST_(test_case_name, test_name, parent_class, parent_id)class GTEST_TEST_CLASS_NAME_(test_case_name, test_name) : public parent_class {public:GTEST_TEST_CLASS_NAME_(test_case_name, test_name)() {}private:virtual void TestBody();static ::testing::TestInfo* const test_info_ GTEST_ATTRIBUTE_UNUSED_;GTEST_DISALLOW_COPY_AND_ASSIGN_(GTEST_TEST_CLASS_NAME_(test_case_name, test_name));};::testing::TestInfo* const GTEST_TEST_CLASS_NAME_(test_case_name, test_name)::test_info_ =::testing::internal::MakeAndRegisterTestInfo(#test_case_name, #test_name, NULL, NULL, ::testing::internal::CodeLocation(__FILE__, __LINE__), (parent_id), parent_class::SetUpTestCase, parent_class::TearDownTestCase, new ::testing::internal::TestFactoryImpl<GTEST_TEST_CLASS_NAME_(test_case_name, test_name)>);void GTEST_TEST_CLASS_NAME_(test_case_name, test_name)::TestBody()






















#endif
