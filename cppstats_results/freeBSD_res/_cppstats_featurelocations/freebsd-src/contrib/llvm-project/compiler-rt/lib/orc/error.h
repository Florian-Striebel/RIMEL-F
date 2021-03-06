







#if !defined(ORC_RT_ERROR_H)
#define ORC_RT_ERROR_H

#include "compiler.h"
#include "extensible_rtti.h"
#include "stl_extras.h"

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>

namespace __orc_rt {


class ErrorInfoBase : public RTTIExtends<ErrorInfoBase, RTTIRoot> {
public:
virtual std::string toString() const = 0;
};


class ORC_RT_NODISCARD Error {

template <typename ErrT, typename... ArgTs>
friend Error make_error(ArgTs &&...Args);

friend Error repackage_error(std::unique_ptr<ErrorInfoBase>);

template <typename ErrT> friend std::unique_ptr<ErrT> error_cast(Error &);

template <typename T> friend class Expected;

public:


~Error() { assertIsChecked(); }

Error(const Error &) = delete;
Error &operator=(const Error &) = delete;




Error(Error &&Other) {
setChecked(true);
*this = std::move(Other);
}





Error &operator=(Error &&Other) {

assertIsChecked();
setPtr(Other.getPtr());


setChecked(false);


Other.setPtr(nullptr);
Other.setChecked(true);

return *this;
}


static Error success() { return Error(); }


explicit operator bool() {
setChecked(getPtr() == nullptr);
return getPtr() != nullptr;
}


template <typename ErrT> bool isA() const {
return getPtr() && getPtr()->isA<ErrT>();
}

private:
Error() = default;

Error(std::unique_ptr<ErrorInfoBase> ErrInfo) {
auto RawErrPtr = reinterpret_cast<uintptr_t>(ErrInfo.release());
assert((RawErrPtr & 0x1) == 0 && "ErrorInfo is insufficiently aligned");
ErrPtr = RawErrPtr | 0x1;
}

void assertIsChecked() {
if (ORC_RT_UNLIKELY(!isChecked() || getPtr())) {
fprintf(stderr, "Error must be checked prior to destruction.\n");
abort();
}
}

template <typename ErrT = ErrorInfoBase> ErrT *getPtr() const {
return reinterpret_cast<ErrT *>(ErrPtr & ~uintptr_t(1));
}

void setPtr(ErrorInfoBase *Ptr) {
ErrPtr = (reinterpret_cast<uintptr_t>(Ptr) & ~uintptr_t(1)) | (ErrPtr & 1);
}

bool isChecked() const { return ErrPtr & 0x1; }

void setChecked(bool Checked) {
ErrPtr = (reinterpret_cast<uintptr_t>(ErrPtr) & ~uintptr_t(1)) | Checked;
}

template <typename ErrT = ErrorInfoBase> std::unique_ptr<ErrT> takePayload() {
static_assert(std::is_base_of<ErrorInfoBase, ErrT>::value,
"ErrT is not an ErrorInfoBase subclass");
std::unique_ptr<ErrT> Tmp(getPtr<ErrT>());
setPtr(nullptr);
setChecked(true);
return Tmp;
}

uintptr_t ErrPtr = 0;
};


template <typename ErrT, typename... ArgTs> Error make_error(ArgTs &&...Args) {
static_assert(std::is_base_of<ErrorInfoBase, ErrT>::value,
"ErrT is not an ErrorInfoBase subclass");
return Error(std::make_unique<ErrT>(std::forward<ArgTs>(Args)...));
}




inline Error repackage_error(std::unique_ptr<ErrorInfoBase> EIB) {
return Error(std::move(EIB));
}














template <typename ErrT> std::unique_ptr<ErrT> error_cast(Error &Err) {
static_assert(std::is_base_of<ErrorInfoBase, ErrT>::value,
"ErrT is not an ErrorInfoBase subclass");
if (Err.isA<ErrT>())
return Err.takePayload<ErrT>();
return nullptr;
}



class ErrorAsOutParameter {
public:
ErrorAsOutParameter(Error *Err) : Err(Err) {

if (Err)
(void)!!*Err;
}

~ErrorAsOutParameter() {

if (Err && !*Err)
*Err = Error::success();
}

private:
Error *Err;
};

template <typename T> class ORC_RT_NODISCARD Expected {

template <class OtherT> friend class Expected;

static constexpr bool IsRef = std::is_reference<T>::value;
using wrap = std::reference_wrapper<std::remove_reference_t<T>>;
using error_type = std::unique_ptr<ErrorInfoBase>;
using storage_type = std::conditional_t<IsRef, wrap, T>;
using value_type = T;

using reference = std::remove_reference_t<T> &;
using const_reference = const std::remove_reference_t<T> &;
using pointer = std::remove_reference_t<T> *;
using const_pointer = const std::remove_reference_t<T> *;

public:

Expected(Error Err) : HasError(true), Unchecked(true) {
assert(Err && "Cannot create Expected<T> from Error success value");
new (getErrorStorage()) error_type(Err.takePayload());
}


template <typename OtherT>
Expected(OtherT &&Val,
std::enable_if_t<std::is_convertible<OtherT, T>::value> * = nullptr)
: HasError(false), Unchecked(true) {
new (getStorage()) storage_type(std::forward<OtherT>(Val));
}


Expected(Expected &&Other) { moveConstruct(std::move(Other)); }



template <class OtherT>
Expected(
Expected<OtherT> &&Other,
std::enable_if_t<std::is_convertible<OtherT, T>::value> * = nullptr) {
moveConstruct(std::move(Other));
}



template <class OtherT>
explicit Expected(
Expected<OtherT> &&Other,
std::enable_if_t<!std::is_convertible<OtherT, T>::value> * = nullptr) {
moveConstruct(std::move(Other));
}


Expected &operator=(Expected &&Other) {
moveAssign(std::move(Other));
return *this;
}


~Expected() {
assertIsChecked();
if (!HasError)
getStorage()->~storage_type();
else
getErrorStorage()->~error_type();
}



explicit operator bool() {
Unchecked = HasError;
return !HasError;
}


template <typename ErrT> bool isFailureOfType() const {
return HasError && (*getErrorStorage())->template isFailureOfType<ErrT>();
}










Error takeError() {
Unchecked = false;
return HasError ? Error(std::move(*getErrorStorage())) : Error::success();
}


pointer operator->() {
assertIsChecked();
return toPointer(getStorage());
}


const_pointer operator->() const {
assertIsChecked();
return toPointer(getStorage());
}


reference operator*() {
assertIsChecked();
return *getStorage();
}


const_reference operator*() const {
assertIsChecked();
return *getStorage();
}

private:
template <class T1>
static bool compareThisIfSameType(const T1 &a, const T1 &b) {
return &a == &b;
}

template <class T1, class T2>
static bool compareThisIfSameType(const T1 &a, const T2 &b) {
return false;
}

template <class OtherT> void moveConstruct(Expected<OtherT> &&Other) {
HasError = Other.HasError;
Unchecked = true;
Other.Unchecked = false;

if (!HasError)
new (getStorage()) storage_type(std::move(*Other.getStorage()));
else
new (getErrorStorage()) error_type(std::move(*Other.getErrorStorage()));
}

template <class OtherT> void moveAssign(Expected<OtherT> &&Other) {
assertIsChecked();

if (compareThisIfSameType(*this, Other))
return;

this->~Expected();
new (this) Expected(std::move(Other));
}

pointer toPointer(pointer Val) { return Val; }

const_pointer toPointer(const_pointer Val) const { return Val; }

pointer toPointer(wrap *Val) { return &Val->get(); }

const_pointer toPointer(const wrap *Val) const { return &Val->get(); }

storage_type *getStorage() {
assert(!HasError && "Cannot get value when an error exists!");
return reinterpret_cast<storage_type *>(&TStorage);
}

const storage_type *getStorage() const {
assert(!HasError && "Cannot get value when an error exists!");
return reinterpret_cast<const storage_type *>(&TStorage);
}

error_type *getErrorStorage() {
assert(HasError && "Cannot get error when a value exists!");
return reinterpret_cast<error_type *>(&ErrorStorage);
}

const error_type *getErrorStorage() const {
assert(HasError && "Cannot get error when a value exists!");
return reinterpret_cast<const error_type *>(&ErrorStorage);
}

void assertIsChecked() {
if (ORC_RT_UNLIKELY(Unchecked)) {
fprintf(stderr,
"Expected<T> must be checked before access or destruction.\n");
abort();
}
}

union {
std::aligned_union_t<1, storage_type> TStorage;
std::aligned_union_t<1, error_type> ErrorStorage;
};

bool HasError : 1;
bool Unchecked : 1;
};


inline void consumeError(Error Err) {
if (Err)
(void)error_cast<ErrorInfoBase>(Err);
}



inline void cantFail(Error Err) {
assert(!Err && "cantFail called on failure value");
consumeError(std::move(Err));
}



template <typename T> T cantFail(Expected<T> E) {
assert(E && "cantFail called on failure value");
consumeError(E.takeError());
return std::move(*E);
}



template <typename T> T &cantFail(Expected<T &> E) {
assert(E && "cantFail called on failure value");
consumeError(E.takeError());
return *E;
}



inline std::string toString(Error Err) {
if (auto EIB = error_cast<ErrorInfoBase>(Err))
return EIB->toString();
return {};
}

class StringError : public RTTIExtends<StringError, ErrorInfoBase> {
public:
StringError(std::string ErrMsg) : ErrMsg(std::move(ErrMsg)) {}
std::string toString() const override { return ErrMsg; }

private:
std::string ErrMsg;
};

}

#endif
