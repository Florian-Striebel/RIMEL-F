











#if !defined(ORC_RT_WRAPPER_FUNCTION_UTILS_H)
#define ORC_RT_WRAPPER_FUNCTION_UTILS_H

#include "c_api.h"
#include "common.h"
#include "error.h"
#include "simple_packed_serialization.h"
#include <type_traits>

namespace __orc_rt {



class WrapperFunctionResult {
public:

WrapperFunctionResult() { __orc_rt_CWrapperFunctionResultInit(&R); }




WrapperFunctionResult(__orc_rt_CWrapperFunctionResult R) : R(R) {}

WrapperFunctionResult(const WrapperFunctionResult &) = delete;
WrapperFunctionResult &operator=(const WrapperFunctionResult &) = delete;

WrapperFunctionResult(WrapperFunctionResult &&Other) {
__orc_rt_CWrapperFunctionResultInit(&R);
std::swap(R, Other.R);
}

WrapperFunctionResult &operator=(WrapperFunctionResult &&Other) {
__orc_rt_CWrapperFunctionResult Tmp;
__orc_rt_CWrapperFunctionResultInit(&Tmp);
std::swap(Tmp, Other.R);
std::swap(R, Tmp);
return *this;
}

~WrapperFunctionResult() { __orc_rt_DisposeCWrapperFunctionResult(&R); }



__orc_rt_CWrapperFunctionResult release() {
__orc_rt_CWrapperFunctionResult Tmp;
__orc_rt_CWrapperFunctionResultInit(&Tmp);
std::swap(R, Tmp);
return Tmp;
}


const char *data() const { return __orc_rt_CWrapperFunctionResultData(&R); }


size_t size() const { return __orc_rt_CWrapperFunctionResultSize(&R); }



bool empty() const { return __orc_rt_CWrapperFunctionResultEmpty(&R); }



static char *allocate(WrapperFunctionResult &R, size_t Size) {
__orc_rt_DisposeCWrapperFunctionResult(&R.R);
__orc_rt_CWrapperFunctionResultInit(&R.R);
return __orc_rt_CWrapperFunctionResultAllocate(&R.R, Size);
}


static WrapperFunctionResult copyFrom(const char *Source, size_t Size) {
return __orc_rt_CreateCWrapperFunctionResultFromRange(Source, Size);
}


static WrapperFunctionResult copyFrom(const char *Source) {
return __orc_rt_CreateCWrapperFunctionResultFromString(Source);
}


static WrapperFunctionResult copyFrom(const std::string &Source) {
return copyFrom(Source.c_str());
}


static WrapperFunctionResult createOutOfBandError(const char *Msg) {
return __orc_rt_CreateCWrapperFunctionResultFromOutOfBandError(Msg);
}


static WrapperFunctionResult createOutOfBandError(const std::string &Msg) {
return createOutOfBandError(Msg.c_str());
}



const char *getOutOfBandError() const {
return __orc_rt_CWrapperFunctionResultGetOutOfBandError(&R);
}

private:
__orc_rt_CWrapperFunctionResult R;
};

namespace detail {

template <typename SPSArgListT, typename... ArgTs>
Expected<WrapperFunctionResult>
serializeViaSPSToWrapperFunctionResult(const ArgTs &...Args) {
WrapperFunctionResult Result;
char *DataPtr =
WrapperFunctionResult::allocate(Result, SPSArgListT::size(Args...));
SPSOutputBuffer OB(DataPtr, Result.size());
if (!SPSArgListT::serialize(OB, Args...))
return make_error<StringError>(
"Error serializing arguments to blob in call");
return std::move(Result);
}

template <typename RetT> class WrapperFunctionHandlerCaller {
public:
template <typename HandlerT, typename ArgTupleT, std::size_t... I>
static decltype(auto) call(HandlerT &&H, ArgTupleT &Args,
std::index_sequence<I...>) {
return std::forward<HandlerT>(H)(std::get<I>(Args)...);
}
};

template <> class WrapperFunctionHandlerCaller<void> {
public:
template <typename HandlerT, typename ArgTupleT, std::size_t... I>
static SPSEmpty call(HandlerT &&H, ArgTupleT &Args,
std::index_sequence<I...>) {
std::forward<HandlerT>(H)(std::get<I>(Args)...);
return SPSEmpty();
}
};

template <typename WrapperFunctionImplT,
template <typename> class ResultSerializer, typename... SPSTagTs>
class WrapperFunctionHandlerHelper
: public WrapperFunctionHandlerHelper<
decltype(&std::remove_reference_t<WrapperFunctionImplT>::operator()),
ResultSerializer, SPSTagTs...> {};

template <typename RetT, typename... ArgTs,
template <typename> class ResultSerializer, typename... SPSTagTs>
class WrapperFunctionHandlerHelper<RetT(ArgTs...), ResultSerializer,
SPSTagTs...> {
public:
using ArgTuple = std::tuple<std::decay_t<ArgTs>...>;
using ArgIndices = std::make_index_sequence<std::tuple_size<ArgTuple>::value>;

template <typename HandlerT>
static WrapperFunctionResult apply(HandlerT &&H, const char *ArgData,
size_t ArgSize) {
ArgTuple Args;
if (!deserialize(ArgData, ArgSize, Args, ArgIndices{}))
return WrapperFunctionResult::createOutOfBandError(
"Could not deserialize arguments for wrapper function call");

auto HandlerResult = WrapperFunctionHandlerCaller<RetT>::call(
std::forward<HandlerT>(H), Args, ArgIndices{});

if (auto Result = ResultSerializer<decltype(HandlerResult)>::serialize(
std::move(HandlerResult)))
return std::move(*Result);
else
return WrapperFunctionResult::createOutOfBandError(
toString(Result.takeError()));
}

private:
template <std::size_t... I>
static bool deserialize(const char *ArgData, size_t ArgSize, ArgTuple &Args,
std::index_sequence<I...>) {
SPSInputBuffer IB(ArgData, ArgSize);
return SPSArgList<SPSTagTs...>::deserialize(IB, std::get<I>(Args)...);
}

};


template <typename RetT, typename... ArgTs,
template <typename> class ResultSerializer, typename... SPSTagTs>
class WrapperFunctionHandlerHelper<RetT (&)(ArgTs...), ResultSerializer,
SPSTagTs...>
: public WrapperFunctionHandlerHelper<RetT(ArgTs...), ResultSerializer,
SPSTagTs...> {};


template <typename ClassT, typename RetT, typename... ArgTs,
template <typename> class ResultSerializer, typename... SPSTagTs>
class WrapperFunctionHandlerHelper<RetT (ClassT::*)(ArgTs...), ResultSerializer,
SPSTagTs...>
: public WrapperFunctionHandlerHelper<RetT(ArgTs...), ResultSerializer,
SPSTagTs...> {};


template <typename ClassT, typename RetT, typename... ArgTs,
template <typename> class ResultSerializer, typename... SPSTagTs>
class WrapperFunctionHandlerHelper<RetT (ClassT::*)(ArgTs...) const,
ResultSerializer, SPSTagTs...>
: public WrapperFunctionHandlerHelper<RetT(ArgTs...), ResultSerializer,
SPSTagTs...> {};

template <typename SPSRetTagT, typename RetT> class ResultSerializer {
public:
static Expected<WrapperFunctionResult> serialize(RetT Result) {
return serializeViaSPSToWrapperFunctionResult<SPSArgList<SPSRetTagT>>(
Result);
}
};

template <typename SPSRetTagT> class ResultSerializer<SPSRetTagT, Error> {
public:
static Expected<WrapperFunctionResult> serialize(Error Err) {
return serializeViaSPSToWrapperFunctionResult<SPSArgList<SPSRetTagT>>(
toSPSSerializable(std::move(Err)));
}
};

template <typename SPSRetTagT, typename T>
class ResultSerializer<SPSRetTagT, Expected<T>> {
public:
static Expected<WrapperFunctionResult> serialize(Expected<T> E) {
return serializeViaSPSToWrapperFunctionResult<SPSArgList<SPSRetTagT>>(
toSPSSerializable(std::move(E)));
}
};

template <typename SPSRetTagT, typename RetT> class ResultDeserializer {
public:
static void makeSafe(RetT &Result) {}

static Error deserialize(RetT &Result, const char *ArgData, size_t ArgSize) {
SPSInputBuffer IB(ArgData, ArgSize);
if (!SPSArgList<SPSRetTagT>::deserialize(IB, Result))
return make_error<StringError>(
"Error deserializing return value from blob in call");
return Error::success();
}
};

template <> class ResultDeserializer<SPSError, Error> {
public:
static void makeSafe(Error &Err) { cantFail(std::move(Err)); }

static Error deserialize(Error &Err, const char *ArgData, size_t ArgSize) {
SPSInputBuffer IB(ArgData, ArgSize);
SPSSerializableError BSE;
if (!SPSArgList<SPSError>::deserialize(IB, BSE))
return make_error<StringError>(
"Error deserializing return value from blob in call");
Err = fromSPSSerializable(std::move(BSE));
return Error::success();
}
};

template <typename SPSTagT, typename T>
class ResultDeserializer<SPSExpected<SPSTagT>, Expected<T>> {
public:
static void makeSafe(Expected<T> &E) { cantFail(E.takeError()); }

static Error deserialize(Expected<T> &E, const char *ArgData,
size_t ArgSize) {
SPSInputBuffer IB(ArgData, ArgSize);
SPSSerializableExpected<T> BSE;
if (!SPSArgList<SPSExpected<SPSTagT>>::deserialize(IB, BSE))
return make_error<StringError>(
"Error deserializing return value from blob in call");
E = fromSPSSerializable(std::move(BSE));
return Error::success();
}
};

}

template <typename SPSSignature> class WrapperFunction;

template <typename SPSRetTagT, typename... SPSTagTs>
class WrapperFunction<SPSRetTagT(SPSTagTs...)> {
private:
template <typename RetT>
using ResultSerializer = detail::ResultSerializer<SPSRetTagT, RetT>;

public:
template <typename RetT, typename... ArgTs>
static Error call(const void *FnTag, RetT &Result, const ArgTs &...Args) {




detail::ResultDeserializer<SPSRetTagT, RetT>::makeSafe(Result);

if (ORC_RT_UNLIKELY(!&__orc_rt_jit_dispatch_ctx))
return make_error<StringError>("__orc_rt_jit_dispatch_ctx not set");
if (ORC_RT_UNLIKELY(!&__orc_rt_jit_dispatch))
return make_error<StringError>("__orc_rt_jit_dispatch not set");

auto ArgBuffer =
detail::serializeViaSPSToWrapperFunctionResult<SPSArgList<SPSTagTs...>>(
Args...);
if (!ArgBuffer)
return ArgBuffer.takeError();

WrapperFunctionResult ResultBuffer =
__orc_rt_jit_dispatch(&__orc_rt_jit_dispatch_ctx, FnTag,
ArgBuffer->data(), ArgBuffer->size());
if (auto ErrMsg = ResultBuffer.getOutOfBandError())
return make_error<StringError>(ErrMsg);

return detail::ResultDeserializer<SPSRetTagT, RetT>::deserialize(
Result, ResultBuffer.data(), ResultBuffer.size());
}

template <typename HandlerT>
static WrapperFunctionResult handle(const char *ArgData, size_t ArgSize,
HandlerT &&Handler) {
using WFHH =
detail::WrapperFunctionHandlerHelper<HandlerT, ResultSerializer,
SPSTagTs...>;
return WFHH::apply(std::forward<HandlerT>(Handler), ArgData, ArgSize);
}

private:
template <typename T> static const T &makeSerializable(const T &Value) {
return Value;
}

static detail::SPSSerializableError makeSerializable(Error Err) {
return detail::toSPSSerializable(std::move(Err));
}

template <typename T>
static detail::SPSSerializableExpected<T> makeSerializable(Expected<T> E) {
return detail::toSPSSerializable(std::move(E));
}
};

template <typename... SPSTagTs>
class WrapperFunction<void(SPSTagTs...)>
: private WrapperFunction<SPSEmpty(SPSTagTs...)> {
public:
template <typename... ArgTs>
static Error call(const void *FnTag, const ArgTs &...Args) {
SPSEmpty BE;
return WrapperFunction<SPSEmpty(SPSTagTs...)>::call(FnTag, BE, Args...);
}

using WrapperFunction<SPSEmpty(SPSTagTs...)>::handle;
};

}

#endif
