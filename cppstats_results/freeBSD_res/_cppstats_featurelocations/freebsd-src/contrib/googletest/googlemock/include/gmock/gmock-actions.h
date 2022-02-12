



































#if !defined(GMOCK_INCLUDE_GMOCK_GMOCK_ACTIONS_H_)
#define GMOCK_INCLUDE_GMOCK_GMOCK_ACTIONS_H_

#if !defined(_WIN32_WCE)
#include <errno.h>
#endif

#include <algorithm>
#include <string>

#include "gmock/internal/gmock-internal-utils.h"
#include "gmock/internal/gmock-port.h"

#if GTEST_LANG_CXX11
#include <functional>
#include <type_traits>
#endif

namespace testing {










namespace internal {

template <typename F1, typename F2>
class ActionAdaptor;






template <typename T, bool kDefaultConstructible>
struct BuiltInDefaultValueGetter {
static T Get() { return T(); }
};
template <typename T>
struct BuiltInDefaultValueGetter<T, false> {
static T Get() {
Assert(false, __FILE__, __LINE__,
"Default action undefined for the function return type.");
return internal::Invalid<T>();


}
};








template <typename T>
class BuiltInDefaultValue {
public:
#if GTEST_LANG_CXX11

static bool Exists() {
return ::std::is_default_constructible<T>::value;
}

static T Get() {
return BuiltInDefaultValueGetter<
T, ::std::is_default_constructible<T>::value>::Get();
}

#else

static bool Exists() {
return false;
}

static T Get() {
return BuiltInDefaultValueGetter<T, false>::Get();
}

#endif
};



template <typename T>
class BuiltInDefaultValue<const T> {
public:
static bool Exists() { return BuiltInDefaultValue<T>::Exists(); }
static T Get() { return BuiltInDefaultValue<T>::Get(); }
};



template <typename T>
class BuiltInDefaultValue<T*> {
public:
static bool Exists() { return true; }
static T* Get() { return NULL; }
};



#define GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(type, value) template <> class BuiltInDefaultValue<type> { public: static bool Exists() { return true; } static type Get() { return value; } }







GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(void, );
#if GTEST_HAS_GLOBAL_STRING
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(::string, "");
#endif
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(::std::string, "");
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(bool, false);
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(unsigned char, '\0');
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(signed char, '\0');
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(char, '\0');







#if GMOCK_WCHAR_T_IS_NATIVE_
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(wchar_t, 0U);
#endif

GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(unsigned short, 0U);
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(signed short, 0);
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(unsigned int, 0U);
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(signed int, 0);
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(unsigned long, 0UL);
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(signed long, 0L);
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(UInt64, 0);
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(Int64, 0);
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(float, 0);
GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_(double, 0);

#undef GMOCK_DEFINE_DEFAULT_ACTION_FOR_RETURN_TYPE_

}














template <typename T>
class DefaultValue {
public:


static void Set(T x) {
delete producer_;
producer_ = new FixedValueProducer(x);
}




typedef T (*FactoryFunction)();
static void SetFactory(FactoryFunction factory) {
delete producer_;
producer_ = new FactoryValueProducer(factory);
}


static void Clear() {
delete producer_;
producer_ = NULL;
}


static bool IsSet() { return producer_ != NULL; }



static bool Exists() {
return IsSet() || internal::BuiltInDefaultValue<T>::Exists();
}




static T Get() {
return producer_ == NULL ?
internal::BuiltInDefaultValue<T>::Get() : producer_->Produce();
}

private:
class ValueProducer {
public:
virtual ~ValueProducer() {}
virtual T Produce() = 0;
};

class FixedValueProducer : public ValueProducer {
public:
explicit FixedValueProducer(T value) : value_(value) {}
virtual T Produce() { return value_; }

private:
const T value_;
GTEST_DISALLOW_COPY_AND_ASSIGN_(FixedValueProducer);
};

class FactoryValueProducer : public ValueProducer {
public:
explicit FactoryValueProducer(FactoryFunction factory)
: factory_(factory) {}
virtual T Produce() { return factory_(); }

private:
const FactoryFunction factory_;
GTEST_DISALLOW_COPY_AND_ASSIGN_(FactoryValueProducer);
};

static ValueProducer* producer_;
};



template <typename T>
class DefaultValue<T&> {
public:

static void Set(T& x) {
address_ = &x;
}


static void Clear() {
address_ = NULL;
}


static bool IsSet() { return address_ != NULL; }



static bool Exists() {
return IsSet() || internal::BuiltInDefaultValue<T&>::Exists();
}




static T& Get() {
return address_ == NULL ?
internal::BuiltInDefaultValue<T&>::Get() : *address_;
}

private:
static T* address_;
};



template <>
class DefaultValue<void> {
public:
static bool Exists() { return true; }
static void Get() {}
};


template <typename T>
typename DefaultValue<T>::ValueProducer* DefaultValue<T>::producer_ = NULL;


template <typename T>
T* DefaultValue<T&>::address_ = NULL;


template <typename F>
class ActionInterface {
public:
typedef typename internal::Function<F>::Result Result;
typedef typename internal::Function<F>::ArgumentTuple ArgumentTuple;

ActionInterface() {}
virtual ~ActionInterface() {}





virtual Result Perform(const ArgumentTuple& args) = 0;

private:
GTEST_DISALLOW_COPY_AND_ASSIGN_(ActionInterface);
};










template <typename F>
class Action {
public:
typedef typename internal::Function<F>::Result Result;
typedef typename internal::Function<F>::ArgumentTuple ArgumentTuple;



Action() {}

#if GTEST_LANG_CXX11



template <typename G,
typename = typename ::std::enable_if<
::std::is_constructible<::std::function<F>, G>::value>::type>
Action(G&& fun) : fun_(::std::forward<G>(fun)) {}
#endif


explicit Action(ActionInterface<F>* impl) : impl_(impl) {}





template <typename Func>
explicit Action(const Action<Func>& action);


bool IsDoDefault() const {
#if GTEST_LANG_CXX11
return impl_ == nullptr && fun_ == nullptr;
#else
return impl_ == NULL;
#endif
}







Result Perform(ArgumentTuple args) const {
if (IsDoDefault()) {
internal::IllegalDoDefault(__FILE__, __LINE__);
}
#if GTEST_LANG_CXX11
if (fun_ != nullptr) {
return internal::Apply(fun_, ::std::move(args));
}
#endif
return impl_->Perform(args);
}

private:
template <typename F1, typename F2>
friend class internal::ActionAdaptor;

template <typename G>
friend class Action;







#if GTEST_LANG_CXX11
::std::function<F> fun_;
#endif
internal::linked_ptr<ActionInterface<F> > impl_;
};






















template <typename Impl>
class PolymorphicAction {
public:
explicit PolymorphicAction(const Impl& impl) : impl_(impl) {}

template <typename F>
operator Action<F>() const {
return Action<F>(new MonomorphicImpl<F>(impl_));
}

private:
template <typename F>
class MonomorphicImpl : public ActionInterface<F> {
public:
typedef typename internal::Function<F>::Result Result;
typedef typename internal::Function<F>::ArgumentTuple ArgumentTuple;

explicit MonomorphicImpl(const Impl& impl) : impl_(impl) {}

virtual Result Perform(const ArgumentTuple& args) {
return impl_.template Perform<Result>(args);
}

private:
Impl impl_;

GTEST_DISALLOW_ASSIGN_(MonomorphicImpl);
};

Impl impl_;

GTEST_DISALLOW_ASSIGN_(PolymorphicAction);
};



template <typename F>
Action<F> MakeAction(ActionInterface<F>* impl) {
return Action<F>(impl);
}








template <typename Impl>
inline PolymorphicAction<Impl> MakePolymorphicAction(const Impl& impl) {
return PolymorphicAction<Impl>(impl);
}

namespace internal {



template <typename F1, typename F2>
class ActionAdaptor : public ActionInterface<F1> {
public:
typedef typename internal::Function<F1>::Result Result;
typedef typename internal::Function<F1>::ArgumentTuple ArgumentTuple;

explicit ActionAdaptor(const Action<F2>& from) : impl_(from.impl_) {}

virtual Result Perform(const ArgumentTuple& args) {
return impl_->Perform(args);
}

private:
const internal::linked_ptr<ActionInterface<F2> > impl_;

GTEST_DISALLOW_ASSIGN_(ActionAdaptor);
};



template <typename T>
struct ByMoveWrapper {
explicit ByMoveWrapper(T value) : payload(internal::move(value)) {}
T payload;
};




























template <typename R>
class ReturnAction {
public:



explicit ReturnAction(R value) : value_(new R(internal::move(value))) {}



template <typename F>
operator Action<F>() const {








typedef typename Function<F>::Result Result;
GTEST_COMPILE_ASSERT_(
!is_reference<Result>::value,
use_ReturnRef_instead_of_Return_to_return_a_reference);
return Action<F>(new Impl<R, F>(value_));
}

private:

template <typename R_, typename F>
class Impl : public ActionInterface<F> {
public:
typedef typename Function<F>::Result Result;
typedef typename Function<F>::ArgumentTuple ArgumentTuple;








explicit Impl(const linked_ptr<R>& value)
: value_before_cast_(*value),
value_(ImplicitCast_<Result>(value_before_cast_)) {}

virtual Result Perform(const ArgumentTuple&) { return value_; }

private:
GTEST_COMPILE_ASSERT_(!is_reference<Result>::value,
Result_cannot_be_a_reference_type);


R value_before_cast_;
Result value_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(Impl);
};



template <typename R_, typename F>
class Impl<ByMoveWrapper<R_>, F> : public ActionInterface<F> {
public:
typedef typename Function<F>::Result Result;
typedef typename Function<F>::ArgumentTuple ArgumentTuple;

explicit Impl(const linked_ptr<R>& wrapper)
: performed_(false), wrapper_(wrapper) {}

virtual Result Perform(const ArgumentTuple&) {
GTEST_CHECK_(!performed_)
<< "A ByMove() action should only be performed once.";
performed_ = true;
return internal::move(wrapper_->payload);
}

private:
bool performed_;
const linked_ptr<R> wrapper_;

GTEST_DISALLOW_ASSIGN_(Impl);
};

const linked_ptr<R> value_;

GTEST_DISALLOW_ASSIGN_(ReturnAction);
};


class ReturnNullAction {
public:



template <typename Result, typename ArgumentTuple>
static Result Perform(const ArgumentTuple&) {
#if GTEST_LANG_CXX11
return nullptr;
#else
GTEST_COMPILE_ASSERT_(internal::is_pointer<Result>::value,
ReturnNull_can_be_used_to_return_a_pointer_only);
return NULL;
#endif
}
};


class ReturnVoidAction {
public:

template <typename Result, typename ArgumentTuple>
static void Perform(const ArgumentTuple&) {
CompileAssertTypesEqual<void, Result>();
}
};




template <typename T>
class ReturnRefAction {
public:

explicit ReturnRefAction(T& ref) : ref_(ref) {}



template <typename F>
operator Action<F>() const {
typedef typename Function<F>::Result Result;



GTEST_COMPILE_ASSERT_(internal::is_reference<Result>::value,
use_Return_instead_of_ReturnRef_to_return_a_value);
return Action<F>(new Impl<F>(ref_));
}

private:

template <typename F>
class Impl : public ActionInterface<F> {
public:
typedef typename Function<F>::Result Result;
typedef typename Function<F>::ArgumentTuple ArgumentTuple;

explicit Impl(T& ref) : ref_(ref) {}

virtual Result Perform(const ArgumentTuple&) {
return ref_;
}

private:
T& ref_;

GTEST_DISALLOW_ASSIGN_(Impl);
};

T& ref_;

GTEST_DISALLOW_ASSIGN_(ReturnRefAction);
};




template <typename T>
class ReturnRefOfCopyAction {
public:


explicit ReturnRefOfCopyAction(const T& value) : value_(value) {}



template <typename F>
operator Action<F>() const {
typedef typename Function<F>::Result Result;



GTEST_COMPILE_ASSERT_(
internal::is_reference<Result>::value,
use_Return_instead_of_ReturnRefOfCopy_to_return_a_value);
return Action<F>(new Impl<F>(value_));
}

private:

template <typename F>
class Impl : public ActionInterface<F> {
public:
typedef typename Function<F>::Result Result;
typedef typename Function<F>::ArgumentTuple ArgumentTuple;

explicit Impl(const T& value) : value_(value) {}

virtual Result Perform(const ArgumentTuple&) {
return value_;
}

private:
T value_;

GTEST_DISALLOW_ASSIGN_(Impl);
};

const T value_;

GTEST_DISALLOW_ASSIGN_(ReturnRefOfCopyAction);
};


class DoDefaultAction {
public:


template <typename F>
operator Action<F>() const { return Action<F>(); }
};



template <typename T1, typename T2>
class AssignAction {
public:
AssignAction(T1* ptr, T2 value) : ptr_(ptr), value_(value) {}

template <typename Result, typename ArgumentTuple>
void Perform(const ArgumentTuple& ) const {
*ptr_ = value_;
}

private:
T1* const ptr_;
const T2 value_;

GTEST_DISALLOW_ASSIGN_(AssignAction);
};

#if !GTEST_OS_WINDOWS_MOBILE



template <typename T>
class SetErrnoAndReturnAction {
public:
SetErrnoAndReturnAction(int errno_value, T result)
: errno_(errno_value),
result_(result) {}
template <typename Result, typename ArgumentTuple>
Result Perform(const ArgumentTuple& ) const {
errno = errno_;
return result_;
}

private:
const int errno_;
const T result_;

GTEST_DISALLOW_ASSIGN_(SetErrnoAndReturnAction);
};

#endif





template <size_t N, typename A, bool kIsProto>
class SetArgumentPointeeAction {
public:


explicit SetArgumentPointeeAction(const A& value) : value_(value) {}

template <typename Result, typename ArgumentTuple>
void Perform(const ArgumentTuple& args) const {
CompileAssertTypesEqual<void, Result>();
*::testing::get<N>(args) = value_;
}

private:
const A value_;

GTEST_DISALLOW_ASSIGN_(SetArgumentPointeeAction);
};

template <size_t N, typename Proto>
class SetArgumentPointeeAction<N, Proto, true> {
public:




explicit SetArgumentPointeeAction(const Proto& proto) : proto_(new Proto) {
proto_->CopyFrom(proto);
}

template <typename Result, typename ArgumentTuple>
void Perform(const ArgumentTuple& args) const {
CompileAssertTypesEqual<void, Result>();
::testing::get<N>(args)->CopyFrom(*proto_);
}

private:
const internal::linked_ptr<Proto> proto_;

GTEST_DISALLOW_ASSIGN_(SetArgumentPointeeAction);
};






template <typename FunctionImpl>
class InvokeWithoutArgsAction {
public:


explicit InvokeWithoutArgsAction(FunctionImpl function_impl)
: function_impl_(function_impl) {}



template <typename Result, typename ArgumentTuple>
Result Perform(const ArgumentTuple&) { return function_impl_(); }

private:
FunctionImpl function_impl_;

GTEST_DISALLOW_ASSIGN_(InvokeWithoutArgsAction);
};


template <class Class, typename MethodPtr>
class InvokeMethodWithoutArgsAction {
public:
InvokeMethodWithoutArgsAction(Class* obj_ptr, MethodPtr method_ptr)
: obj_ptr_(obj_ptr), method_ptr_(method_ptr) {}

template <typename Result, typename ArgumentTuple>
Result Perform(const ArgumentTuple&) const {
return (obj_ptr_->*method_ptr_)();
}

private:
Class* const obj_ptr_;
const MethodPtr method_ptr_;

GTEST_DISALLOW_ASSIGN_(InvokeMethodWithoutArgsAction);
};


template <typename CallbackType>
class InvokeCallbackWithoutArgsAction {
public:

explicit InvokeCallbackWithoutArgsAction(CallbackType* callback)
: callback_(callback) {
callback->CheckIsRepeatable();
}




template <typename Result, typename ArgumentTuple>
Result Perform(const ArgumentTuple&) const { return callback_->Run(); }

private:
const internal::linked_ptr<CallbackType> callback_;

GTEST_DISALLOW_ASSIGN_(InvokeCallbackWithoutArgsAction);
};


template <typename A>
class IgnoreResultAction {
public:
explicit IgnoreResultAction(const A& action) : action_(action) {}

template <typename F>
operator Action<F>() const {








typedef typename internal::Function<F>::Result Result;


CompileAssertTypesEqual<void, Result>();

return Action<F>(new Impl<F>(action_));
}

private:
template <typename F>
class Impl : public ActionInterface<F> {
public:
typedef typename internal::Function<F>::Result Result;
typedef typename internal::Function<F>::ArgumentTuple ArgumentTuple;

explicit Impl(const A& action) : action_(action) {}

virtual void Perform(const ArgumentTuple& args) {

action_.Perform(args);
}

private:


typedef typename internal::Function<F>::MakeResultIgnoredValue
OriginalFunction;

const Action<OriginalFunction> action_;

GTEST_DISALLOW_ASSIGN_(Impl);
};

const A action_;

GTEST_DISALLOW_ASSIGN_(IgnoreResultAction);
};








template <typename T>
class ReferenceWrapper {
public:

explicit ReferenceWrapper(T& l_value) : pointer_(&l_value) {}



operator T&() const { return *pointer_; }
private:
T* pointer_;
};


template <typename T>
void PrintTo(const ReferenceWrapper<T>& ref, ::std::ostream* os) {
T& value = ref;
UniversalPrinter<T&>::Print(value, os);
}



template <typename Action1, typename Action2>
class DoBothAction {
public:
DoBothAction(Action1 action1, Action2 action2)
: action1_(action1), action2_(action2) {}



template <typename F>
operator Action<F>() const {
return Action<F>(new Impl<F>(action1_, action2_));
}

private:

template <typename F>
class Impl : public ActionInterface<F> {
public:
typedef typename Function<F>::Result Result;
typedef typename Function<F>::ArgumentTuple ArgumentTuple;
typedef typename Function<F>::MakeResultVoid VoidResult;

Impl(const Action<VoidResult>& action1, const Action<F>& action2)
: action1_(action1), action2_(action2) {}

virtual Result Perform(const ArgumentTuple& args) {
action1_.Perform(args);
return action2_.Perform(args);
}

private:
const Action<VoidResult> action1_;
const Action<F> action2_;

GTEST_DISALLOW_ASSIGN_(Impl);
};

Action1 action1_;
Action2 action2_;

GTEST_DISALLOW_ASSIGN_(DoBothAction);
};

}































typedef internal::IgnoredValue Unused;





template <typename To>
template <typename From>
Action<To>::Action(const Action<From>& from)
:
#if GTEST_LANG_CXX11
fun_(from.fun_),
#endif
impl_(from.impl_ == NULL ? NULL
: new internal::ActionAdaptor<To, From>(from)) {
}




template <typename R>
internal::ReturnAction<R> Return(R value) {
return internal::ReturnAction<R>(internal::move(value));
}


inline PolymorphicAction<internal::ReturnNullAction> ReturnNull() {
return MakePolymorphicAction(internal::ReturnNullAction());
}


inline PolymorphicAction<internal::ReturnVoidAction> Return() {
return MakePolymorphicAction(internal::ReturnVoidAction());
}


template <typename R>
inline internal::ReturnRefAction<R> ReturnRef(R& x) {
return internal::ReturnRefAction<R>(x);
}




template <typename R>
inline internal::ReturnRefOfCopyAction<R> ReturnRefOfCopy(const R& x) {
return internal::ReturnRefOfCopyAction<R>(x);
}





template <typename R>
internal::ByMoveWrapper<R> ByMove(R x) {
return internal::ByMoveWrapper<R>(internal::move(x));
}


inline internal::DoDefaultAction DoDefault() {
return internal::DoDefaultAction();
}



template <size_t N, typename T>
PolymorphicAction<
internal::SetArgumentPointeeAction<
N, T, internal::IsAProtocolMessage<T>::value> >
SetArgPointee(const T& x) {
return MakePolymorphicAction(internal::SetArgumentPointeeAction<
N, T, internal::IsAProtocolMessage<T>::value>(x));
}

#if !((GTEST_GCC_VER_ && GTEST_GCC_VER_ < 40000) || GTEST_OS_SYMBIAN)



template <size_t N>
PolymorphicAction<
internal::SetArgumentPointeeAction<N, const char*, false> >
SetArgPointee(const char* p) {
return MakePolymorphicAction(internal::SetArgumentPointeeAction<
N, const char*, false>(p));
}

template <size_t N>
PolymorphicAction<
internal::SetArgumentPointeeAction<N, const wchar_t*, false> >
SetArgPointee(const wchar_t* p) {
return MakePolymorphicAction(internal::SetArgumentPointeeAction<
N, const wchar_t*, false>(p));
}
#endif


template <size_t N, typename T>
PolymorphicAction<
internal::SetArgumentPointeeAction<
N, T, internal::IsAProtocolMessage<T>::value> >
SetArgumentPointee(const T& x) {
return MakePolymorphicAction(internal::SetArgumentPointeeAction<
N, T, internal::IsAProtocolMessage<T>::value>(x));
}


template <typename T1, typename T2>
PolymorphicAction<internal::AssignAction<T1, T2> > Assign(T1* ptr, T2 val) {
return MakePolymorphicAction(internal::AssignAction<T1, T2>(ptr, val));
}

#if !GTEST_OS_WINDOWS_MOBILE


template <typename T>
PolymorphicAction<internal::SetErrnoAndReturnAction<T> >
SetErrnoAndReturn(int errval, T result) {
return MakePolymorphicAction(
internal::SetErrnoAndReturnAction<T>(errval, result));
}

#endif




template <typename FunctionImpl>
PolymorphicAction<internal::InvokeWithoutArgsAction<FunctionImpl> >
InvokeWithoutArgs(FunctionImpl function_impl) {
return MakePolymorphicAction(
internal::InvokeWithoutArgsAction<FunctionImpl>(function_impl));
}



template <class Class, typename MethodPtr>
PolymorphicAction<internal::InvokeMethodWithoutArgsAction<Class, MethodPtr> >
InvokeWithoutArgs(Class* obj_ptr, MethodPtr method_ptr) {
return MakePolymorphicAction(
internal::InvokeMethodWithoutArgsAction<Class, MethodPtr>(
obj_ptr, method_ptr));
}




template <typename A>
inline internal::IgnoreResultAction<A> IgnoreResult(const A& an_action) {
return internal::IgnoreResultAction<A>(an_action);
}








template <typename T>
inline internal::ReferenceWrapper<T> ByRef(T& l_value) {
return internal::ReferenceWrapper<T>(l_value);
}

}

#endif
