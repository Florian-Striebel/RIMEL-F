






































#if !defined(GMOCK_INCLUDE_GMOCK_GMOCK_GENERATED_MATCHERS_H_)
#define GMOCK_INCLUDE_GMOCK_GMOCK_GENERATED_MATCHERS_H_

#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include "gmock/gmock-matchers.h"

namespace testing {
namespace internal {


#define GMOCK_FIELD_TYPE_(Tuple, i) typename ::testing::tuple_element<i, Tuple>::type













template <class Tuple, int k0 = -1, int k1 = -1, int k2 = -1, int k3 = -1,
int k4 = -1, int k5 = -1, int k6 = -1, int k7 = -1, int k8 = -1,
int k9 = -1>
class TupleFields;


template <class Tuple, int k0, int k1, int k2, int k3, int k4, int k5, int k6,
int k7, int k8, int k9>
class TupleFields {
public:
typedef ::testing::tuple<GMOCK_FIELD_TYPE_(Tuple, k0),
GMOCK_FIELD_TYPE_(Tuple, k1), GMOCK_FIELD_TYPE_(Tuple, k2),
GMOCK_FIELD_TYPE_(Tuple, k3), GMOCK_FIELD_TYPE_(Tuple, k4),
GMOCK_FIELD_TYPE_(Tuple, k5), GMOCK_FIELD_TYPE_(Tuple, k6),
GMOCK_FIELD_TYPE_(Tuple, k7), GMOCK_FIELD_TYPE_(Tuple, k8),
GMOCK_FIELD_TYPE_(Tuple, k9)> type;
static type GetSelectedFields(const Tuple& t) {
return type(get<k0>(t), get<k1>(t), get<k2>(t), get<k3>(t), get<k4>(t),
get<k5>(t), get<k6>(t), get<k7>(t), get<k8>(t), get<k9>(t));
}
};



template <class Tuple>
class TupleFields<Tuple, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1> {
public:
typedef ::testing::tuple<> type;
static type GetSelectedFields(const Tuple& ) {
return type();
}
};

template <class Tuple, int k0>
class TupleFields<Tuple, k0, -1, -1, -1, -1, -1, -1, -1, -1, -1> {
public:
typedef ::testing::tuple<GMOCK_FIELD_TYPE_(Tuple, k0)> type;
static type GetSelectedFields(const Tuple& t) {
return type(get<k0>(t));
}
};

template <class Tuple, int k0, int k1>
class TupleFields<Tuple, k0, k1, -1, -1, -1, -1, -1, -1, -1, -1> {
public:
typedef ::testing::tuple<GMOCK_FIELD_TYPE_(Tuple, k0),
GMOCK_FIELD_TYPE_(Tuple, k1)> type;
static type GetSelectedFields(const Tuple& t) {
return type(get<k0>(t), get<k1>(t));
}
};

template <class Tuple, int k0, int k1, int k2>
class TupleFields<Tuple, k0, k1, k2, -1, -1, -1, -1, -1, -1, -1> {
public:
typedef ::testing::tuple<GMOCK_FIELD_TYPE_(Tuple, k0),
GMOCK_FIELD_TYPE_(Tuple, k1), GMOCK_FIELD_TYPE_(Tuple, k2)> type;
static type GetSelectedFields(const Tuple& t) {
return type(get<k0>(t), get<k1>(t), get<k2>(t));
}
};

template <class Tuple, int k0, int k1, int k2, int k3>
class TupleFields<Tuple, k0, k1, k2, k3, -1, -1, -1, -1, -1, -1> {
public:
typedef ::testing::tuple<GMOCK_FIELD_TYPE_(Tuple, k0),
GMOCK_FIELD_TYPE_(Tuple, k1), GMOCK_FIELD_TYPE_(Tuple, k2),
GMOCK_FIELD_TYPE_(Tuple, k3)> type;
static type GetSelectedFields(const Tuple& t) {
return type(get<k0>(t), get<k1>(t), get<k2>(t), get<k3>(t));
}
};

template <class Tuple, int k0, int k1, int k2, int k3, int k4>
class TupleFields<Tuple, k0, k1, k2, k3, k4, -1, -1, -1, -1, -1> {
public:
typedef ::testing::tuple<GMOCK_FIELD_TYPE_(Tuple, k0),
GMOCK_FIELD_TYPE_(Tuple, k1), GMOCK_FIELD_TYPE_(Tuple, k2),
GMOCK_FIELD_TYPE_(Tuple, k3), GMOCK_FIELD_TYPE_(Tuple, k4)> type;
static type GetSelectedFields(const Tuple& t) {
return type(get<k0>(t), get<k1>(t), get<k2>(t), get<k3>(t), get<k4>(t));
}
};

template <class Tuple, int k0, int k1, int k2, int k3, int k4, int k5>
class TupleFields<Tuple, k0, k1, k2, k3, k4, k5, -1, -1, -1, -1> {
public:
typedef ::testing::tuple<GMOCK_FIELD_TYPE_(Tuple, k0),
GMOCK_FIELD_TYPE_(Tuple, k1), GMOCK_FIELD_TYPE_(Tuple, k2),
GMOCK_FIELD_TYPE_(Tuple, k3), GMOCK_FIELD_TYPE_(Tuple, k4),
GMOCK_FIELD_TYPE_(Tuple, k5)> type;
static type GetSelectedFields(const Tuple& t) {
return type(get<k0>(t), get<k1>(t), get<k2>(t), get<k3>(t), get<k4>(t),
get<k5>(t));
}
};

template <class Tuple, int k0, int k1, int k2, int k3, int k4, int k5, int k6>
class TupleFields<Tuple, k0, k1, k2, k3, k4, k5, k6, -1, -1, -1> {
public:
typedef ::testing::tuple<GMOCK_FIELD_TYPE_(Tuple, k0),
GMOCK_FIELD_TYPE_(Tuple, k1), GMOCK_FIELD_TYPE_(Tuple, k2),
GMOCK_FIELD_TYPE_(Tuple, k3), GMOCK_FIELD_TYPE_(Tuple, k4),
GMOCK_FIELD_TYPE_(Tuple, k5), GMOCK_FIELD_TYPE_(Tuple, k6)> type;
static type GetSelectedFields(const Tuple& t) {
return type(get<k0>(t), get<k1>(t), get<k2>(t), get<k3>(t), get<k4>(t),
get<k5>(t), get<k6>(t));
}
};

template <class Tuple, int k0, int k1, int k2, int k3, int k4, int k5, int k6,
int k7>
class TupleFields<Tuple, k0, k1, k2, k3, k4, k5, k6, k7, -1, -1> {
public:
typedef ::testing::tuple<GMOCK_FIELD_TYPE_(Tuple, k0),
GMOCK_FIELD_TYPE_(Tuple, k1), GMOCK_FIELD_TYPE_(Tuple, k2),
GMOCK_FIELD_TYPE_(Tuple, k3), GMOCK_FIELD_TYPE_(Tuple, k4),
GMOCK_FIELD_TYPE_(Tuple, k5), GMOCK_FIELD_TYPE_(Tuple, k6),
GMOCK_FIELD_TYPE_(Tuple, k7)> type;
static type GetSelectedFields(const Tuple& t) {
return type(get<k0>(t), get<k1>(t), get<k2>(t), get<k3>(t), get<k4>(t),
get<k5>(t), get<k6>(t), get<k7>(t));
}
};

template <class Tuple, int k0, int k1, int k2, int k3, int k4, int k5, int k6,
int k7, int k8>
class TupleFields<Tuple, k0, k1, k2, k3, k4, k5, k6, k7, k8, -1> {
public:
typedef ::testing::tuple<GMOCK_FIELD_TYPE_(Tuple, k0),
GMOCK_FIELD_TYPE_(Tuple, k1), GMOCK_FIELD_TYPE_(Tuple, k2),
GMOCK_FIELD_TYPE_(Tuple, k3), GMOCK_FIELD_TYPE_(Tuple, k4),
GMOCK_FIELD_TYPE_(Tuple, k5), GMOCK_FIELD_TYPE_(Tuple, k6),
GMOCK_FIELD_TYPE_(Tuple, k7), GMOCK_FIELD_TYPE_(Tuple, k8)> type;
static type GetSelectedFields(const Tuple& t) {
return type(get<k0>(t), get<k1>(t), get<k2>(t), get<k3>(t), get<k4>(t),
get<k5>(t), get<k6>(t), get<k7>(t), get<k8>(t));
}
};

#undef GMOCK_FIELD_TYPE_


template <class ArgsTuple, int k0 = -1, int k1 = -1, int k2 = -1, int k3 = -1,
int k4 = -1, int k5 = -1, int k6 = -1, int k7 = -1, int k8 = -1,
int k9 = -1>
class ArgsMatcherImpl : public MatcherInterface<ArgsTuple> {
public:

typedef GTEST_REMOVE_REFERENCE_AND_CONST_(ArgsTuple) RawArgsTuple;
typedef typename internal::TupleFields<RawArgsTuple, k0, k1, k2, k3, k4, k5,
k6, k7, k8, k9>::type SelectedArgs;
typedef Matcher<const SelectedArgs&> MonomorphicInnerMatcher;

template <typename InnerMatcher>
explicit ArgsMatcherImpl(const InnerMatcher& inner_matcher)
: inner_matcher_(SafeMatcherCast<const SelectedArgs&>(inner_matcher)) {}

virtual bool MatchAndExplain(ArgsTuple args,
MatchResultListener* listener) const {
const SelectedArgs& selected_args = GetSelectedArgs(args);
if (!listener->IsInterested())
return inner_matcher_.Matches(selected_args);

PrintIndices(listener->stream());
*listener << "are " << PrintToString(selected_args);

StringMatchResultListener inner_listener;
const bool match = inner_matcher_.MatchAndExplain(selected_args,
&inner_listener);
PrintIfNotEmpty(inner_listener.str(), listener->stream());
return match;
}

virtual void DescribeTo(::std::ostream* os) const {
*os << "are a tuple ";
PrintIndices(os);
inner_matcher_.DescribeTo(os);
}

virtual void DescribeNegationTo(::std::ostream* os) const {
*os << "are a tuple ";
PrintIndices(os);
inner_matcher_.DescribeNegationTo(os);
}

private:
static SelectedArgs GetSelectedArgs(ArgsTuple args) {
return TupleFields<RawArgsTuple, k0, k1, k2, k3, k4, k5, k6, k7, k8,
k9>::GetSelectedFields(args);
}


static void PrintIndices(::std::ostream* os) {
*os << "whose fields (";
const int indices[10] = { k0, k1, k2, k3, k4, k5, k6, k7, k8, k9 };
for (int i = 0; i < 10; i++) {
if (indices[i] < 0)
break;

if (i >= 1)
*os << ", ";

*os << "#" << indices[i];
}
*os << ") ";
}

const MonomorphicInnerMatcher inner_matcher_;

GTEST_DISALLOW_ASSIGN_(ArgsMatcherImpl);
};

template <class InnerMatcher, int k0 = -1, int k1 = -1, int k2 = -1,
int k3 = -1, int k4 = -1, int k5 = -1, int k6 = -1, int k7 = -1,
int k8 = -1, int k9 = -1>
class ArgsMatcher {
public:
explicit ArgsMatcher(const InnerMatcher& inner_matcher)
: inner_matcher_(inner_matcher) {}

template <typename ArgsTuple>
operator Matcher<ArgsTuple>() const {
return MakeMatcher(new ArgsMatcherImpl<ArgsTuple, k0, k1, k2, k3, k4, k5,
k6, k7, k8, k9>(inner_matcher_));
}

private:
const InnerMatcher inner_matcher_;

GTEST_DISALLOW_ASSIGN_(ArgsMatcher);
};







template <typename M1>
struct AllOfResult1 {
typedef M1 type;
};

template <typename M1, typename M2>
struct AllOfResult2 {
typedef BothOfMatcher<
typename AllOfResult1<M1>::type,
typename AllOfResult1<M2>::type
> type;
};

template <typename M1, typename M2, typename M3>
struct AllOfResult3 {
typedef BothOfMatcher<
typename AllOfResult1<M1>::type,
typename AllOfResult2<M2, M3>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4>
struct AllOfResult4 {
typedef BothOfMatcher<
typename AllOfResult2<M1, M2>::type,
typename AllOfResult2<M3, M4>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4, typename M5>
struct AllOfResult5 {
typedef BothOfMatcher<
typename AllOfResult2<M1, M2>::type,
typename AllOfResult3<M3, M4, M5>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6>
struct AllOfResult6 {
typedef BothOfMatcher<
typename AllOfResult3<M1, M2, M3>::type,
typename AllOfResult3<M4, M5, M6>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7>
struct AllOfResult7 {
typedef BothOfMatcher<
typename AllOfResult3<M1, M2, M3>::type,
typename AllOfResult4<M4, M5, M6, M7>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7, typename M8>
struct AllOfResult8 {
typedef BothOfMatcher<
typename AllOfResult4<M1, M2, M3, M4>::type,
typename AllOfResult4<M5, M6, M7, M8>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7, typename M8, typename M9>
struct AllOfResult9 {
typedef BothOfMatcher<
typename AllOfResult4<M1, M2, M3, M4>::type,
typename AllOfResult5<M5, M6, M7, M8, M9>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7, typename M8, typename M9, typename M10>
struct AllOfResult10 {
typedef BothOfMatcher<
typename AllOfResult5<M1, M2, M3, M4, M5>::type,
typename AllOfResult5<M6, M7, M8, M9, M10>::type
> type;
};







template <typename M1>
struct AnyOfResult1 {
typedef M1 type;
};

template <typename M1, typename M2>
struct AnyOfResult2 {
typedef EitherOfMatcher<
typename AnyOfResult1<M1>::type,
typename AnyOfResult1<M2>::type
> type;
};

template <typename M1, typename M2, typename M3>
struct AnyOfResult3 {
typedef EitherOfMatcher<
typename AnyOfResult1<M1>::type,
typename AnyOfResult2<M2, M3>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4>
struct AnyOfResult4 {
typedef EitherOfMatcher<
typename AnyOfResult2<M1, M2>::type,
typename AnyOfResult2<M3, M4>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4, typename M5>
struct AnyOfResult5 {
typedef EitherOfMatcher<
typename AnyOfResult2<M1, M2>::type,
typename AnyOfResult3<M3, M4, M5>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6>
struct AnyOfResult6 {
typedef EitherOfMatcher<
typename AnyOfResult3<M1, M2, M3>::type,
typename AnyOfResult3<M4, M5, M6>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7>
struct AnyOfResult7 {
typedef EitherOfMatcher<
typename AnyOfResult3<M1, M2, M3>::type,
typename AnyOfResult4<M4, M5, M6, M7>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7, typename M8>
struct AnyOfResult8 {
typedef EitherOfMatcher<
typename AnyOfResult4<M1, M2, M3, M4>::type,
typename AnyOfResult4<M5, M6, M7, M8>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7, typename M8, typename M9>
struct AnyOfResult9 {
typedef EitherOfMatcher<
typename AnyOfResult4<M1, M2, M3, M4>::type,
typename AnyOfResult5<M5, M6, M7, M8, M9>::type
> type;
};

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7, typename M8, typename M9, typename M10>
struct AnyOfResult10 {
typedef EitherOfMatcher<
typename AnyOfResult5<M1, M2, M3, M4, M5>::type,
typename AnyOfResult5<M6, M7, M8, M9, M10>::type
> type;
};

}




template <typename InnerMatcher>
inline internal::ArgsMatcher<InnerMatcher>
Args(const InnerMatcher& matcher) {
return internal::ArgsMatcher<InnerMatcher>(matcher);
}

template <int k1, typename InnerMatcher>
inline internal::ArgsMatcher<InnerMatcher, k1>
Args(const InnerMatcher& matcher) {
return internal::ArgsMatcher<InnerMatcher, k1>(matcher);
}

template <int k1, int k2, typename InnerMatcher>
inline internal::ArgsMatcher<InnerMatcher, k1, k2>
Args(const InnerMatcher& matcher) {
return internal::ArgsMatcher<InnerMatcher, k1, k2>(matcher);
}

template <int k1, int k2, int k3, typename InnerMatcher>
inline internal::ArgsMatcher<InnerMatcher, k1, k2, k3>
Args(const InnerMatcher& matcher) {
return internal::ArgsMatcher<InnerMatcher, k1, k2, k3>(matcher);
}

template <int k1, int k2, int k3, int k4, typename InnerMatcher>
inline internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4>
Args(const InnerMatcher& matcher) {
return internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4>(matcher);
}

template <int k1, int k2, int k3, int k4, int k5, typename InnerMatcher>
inline internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4, k5>
Args(const InnerMatcher& matcher) {
return internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4, k5>(matcher);
}

template <int k1, int k2, int k3, int k4, int k5, int k6, typename InnerMatcher>
inline internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4, k5, k6>
Args(const InnerMatcher& matcher) {
return internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4, k5, k6>(matcher);
}

template <int k1, int k2, int k3, int k4, int k5, int k6, int k7,
typename InnerMatcher>
inline internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4, k5, k6, k7>
Args(const InnerMatcher& matcher) {
return internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4, k5, k6,
k7>(matcher);
}

template <int k1, int k2, int k3, int k4, int k5, int k6, int k7, int k8,
typename InnerMatcher>
inline internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4, k5, k6, k7, k8>
Args(const InnerMatcher& matcher) {
return internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4, k5, k6, k7,
k8>(matcher);
}

template <int k1, int k2, int k3, int k4, int k5, int k6, int k7, int k8,
int k9, typename InnerMatcher>
inline internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4, k5, k6, k7, k8, k9>
Args(const InnerMatcher& matcher) {
return internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4, k5, k6, k7, k8,
k9>(matcher);
}

template <int k1, int k2, int k3, int k4, int k5, int k6, int k7, int k8,
int k9, int k10, typename InnerMatcher>
inline internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4, k5, k6, k7, k8, k9,
k10>
Args(const InnerMatcher& matcher) {
return internal::ArgsMatcher<InnerMatcher, k1, k2, k3, k4, k5, k6, k7, k8,
k9, k10>(matcher);
}















inline internal::ElementsAreMatcher<
::testing::tuple<> >
ElementsAre() {
typedef ::testing::tuple<> Args;
return internal::ElementsAreMatcher<Args>(Args());
}

template <typename T1>
inline internal::ElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type> >
ElementsAre(const T1& e1) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type> Args;
return internal::ElementsAreMatcher<Args>(Args(e1));
}

template <typename T1, typename T2>
inline internal::ElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type> >
ElementsAre(const T1& e1, const T2& e2) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type> Args;
return internal::ElementsAreMatcher<Args>(Args(e1, e2));
}

template <typename T1, typename T2, typename T3>
inline internal::ElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type> >
ElementsAre(const T1& e1, const T2& e2, const T3& e3) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type> Args;
return internal::ElementsAreMatcher<Args>(Args(e1, e2, e3));
}

template <typename T1, typename T2, typename T3, typename T4>
inline internal::ElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type> >
ElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type> Args;
return internal::ElementsAreMatcher<Args>(Args(e1, e2, e3, e4));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
inline internal::ElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type> >
ElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4,
const T5& e5) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type> Args;
return internal::ElementsAreMatcher<Args>(Args(e1, e2, e3, e4, e5));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6>
inline internal::ElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type> >
ElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4,
const T5& e5, const T6& e6) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type> Args;
return internal::ElementsAreMatcher<Args>(Args(e1, e2, e3, e4, e5, e6));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6, typename T7>
inline internal::ElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type> >
ElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4,
const T5& e5, const T6& e6, const T7& e7) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type> Args;
return internal::ElementsAreMatcher<Args>(Args(e1, e2, e3, e4, e5, e6, e7));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6, typename T7, typename T8>
inline internal::ElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type,
typename internal::DecayArray<T8>::type> >
ElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4,
const T5& e5, const T6& e6, const T7& e7, const T8& e8) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type,
typename internal::DecayArray<T8>::type> Args;
return internal::ElementsAreMatcher<Args>(Args(e1, e2, e3, e4, e5, e6, e7,
e8));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6, typename T7, typename T8, typename T9>
inline internal::ElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type,
typename internal::DecayArray<T8>::type,
typename internal::DecayArray<T9>::type> >
ElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4,
const T5& e5, const T6& e6, const T7& e7, const T8& e8, const T9& e9) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type,
typename internal::DecayArray<T8>::type,
typename internal::DecayArray<T9>::type> Args;
return internal::ElementsAreMatcher<Args>(Args(e1, e2, e3, e4, e5, e6, e7,
e8, e9));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6, typename T7, typename T8, typename T9, typename T10>
inline internal::ElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type,
typename internal::DecayArray<T8>::type,
typename internal::DecayArray<T9>::type,
typename internal::DecayArray<T10>::type> >
ElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4,
const T5& e5, const T6& e6, const T7& e7, const T8& e8, const T9& e9,
const T10& e10) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type,
typename internal::DecayArray<T8>::type,
typename internal::DecayArray<T9>::type,
typename internal::DecayArray<T10>::type> Args;
return internal::ElementsAreMatcher<Args>(Args(e1, e2, e3, e4, e5, e6, e7,
e8, e9, e10));
}







inline internal::UnorderedElementsAreMatcher<
::testing::tuple<> >
UnorderedElementsAre() {
typedef ::testing::tuple<> Args;
return internal::UnorderedElementsAreMatcher<Args>(Args());
}

template <typename T1>
inline internal::UnorderedElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type> >
UnorderedElementsAre(const T1& e1) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type> Args;
return internal::UnorderedElementsAreMatcher<Args>(Args(e1));
}

template <typename T1, typename T2>
inline internal::UnorderedElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type> >
UnorderedElementsAre(const T1& e1, const T2& e2) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type> Args;
return internal::UnorderedElementsAreMatcher<Args>(Args(e1, e2));
}

template <typename T1, typename T2, typename T3>
inline internal::UnorderedElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type> >
UnorderedElementsAre(const T1& e1, const T2& e2, const T3& e3) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type> Args;
return internal::UnorderedElementsAreMatcher<Args>(Args(e1, e2, e3));
}

template <typename T1, typename T2, typename T3, typename T4>
inline internal::UnorderedElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type> >
UnorderedElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type> Args;
return internal::UnorderedElementsAreMatcher<Args>(Args(e1, e2, e3, e4));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
inline internal::UnorderedElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type> >
UnorderedElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4,
const T5& e5) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type> Args;
return internal::UnorderedElementsAreMatcher<Args>(Args(e1, e2, e3, e4, e5));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6>
inline internal::UnorderedElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type> >
UnorderedElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4,
const T5& e5, const T6& e6) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type> Args;
return internal::UnorderedElementsAreMatcher<Args>(Args(e1, e2, e3, e4, e5,
e6));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6, typename T7>
inline internal::UnorderedElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type> >
UnorderedElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4,
const T5& e5, const T6& e6, const T7& e7) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type> Args;
return internal::UnorderedElementsAreMatcher<Args>(Args(e1, e2, e3, e4, e5,
e6, e7));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6, typename T7, typename T8>
inline internal::UnorderedElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type,
typename internal::DecayArray<T8>::type> >
UnorderedElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4,
const T5& e5, const T6& e6, const T7& e7, const T8& e8) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type,
typename internal::DecayArray<T8>::type> Args;
return internal::UnorderedElementsAreMatcher<Args>(Args(e1, e2, e3, e4, e5,
e6, e7, e8));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6, typename T7, typename T8, typename T9>
inline internal::UnorderedElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type,
typename internal::DecayArray<T8>::type,
typename internal::DecayArray<T9>::type> >
UnorderedElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4,
const T5& e5, const T6& e6, const T7& e7, const T8& e8, const T9& e9) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type,
typename internal::DecayArray<T8>::type,
typename internal::DecayArray<T9>::type> Args;
return internal::UnorderedElementsAreMatcher<Args>(Args(e1, e2, e3, e4, e5,
e6, e7, e8, e9));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
typename T6, typename T7, typename T8, typename T9, typename T10>
inline internal::UnorderedElementsAreMatcher<
::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type,
typename internal::DecayArray<T8>::type,
typename internal::DecayArray<T9>::type,
typename internal::DecayArray<T10>::type> >
UnorderedElementsAre(const T1& e1, const T2& e2, const T3& e3, const T4& e4,
const T5& e5, const T6& e6, const T7& e7, const T8& e8, const T9& e9,
const T10& e10) {
typedef ::testing::tuple<
typename internal::DecayArray<T1>::type,
typename internal::DecayArray<T2>::type,
typename internal::DecayArray<T3>::type,
typename internal::DecayArray<T4>::type,
typename internal::DecayArray<T5>::type,
typename internal::DecayArray<T6>::type,
typename internal::DecayArray<T7>::type,
typename internal::DecayArray<T8>::type,
typename internal::DecayArray<T9>::type,
typename internal::DecayArray<T10>::type> Args;
return internal::UnorderedElementsAreMatcher<Args>(Args(e1, e2, e3, e4, e5,
e6, e7, e8, e9, e10));
}




template <typename M1, typename M2>
inline typename internal::AllOfResult2<M1, M2>::type
AllOf(M1 m1, M2 m2) {
return typename internal::AllOfResult2<M1, M2>::type(
m1,
m2);
}

template <typename M1, typename M2, typename M3>
inline typename internal::AllOfResult3<M1, M2, M3>::type
AllOf(M1 m1, M2 m2, M3 m3) {
return typename internal::AllOfResult3<M1, M2, M3>::type(
m1,
::testing::AllOf(m2, m3));
}

template <typename M1, typename M2, typename M3, typename M4>
inline typename internal::AllOfResult4<M1, M2, M3, M4>::type
AllOf(M1 m1, M2 m2, M3 m3, M4 m4) {
return typename internal::AllOfResult4<M1, M2, M3, M4>::type(
::testing::AllOf(m1, m2),
::testing::AllOf(m3, m4));
}

template <typename M1, typename M2, typename M3, typename M4, typename M5>
inline typename internal::AllOfResult5<M1, M2, M3, M4, M5>::type
AllOf(M1 m1, M2 m2, M3 m3, M4 m4, M5 m5) {
return typename internal::AllOfResult5<M1, M2, M3, M4, M5>::type(
::testing::AllOf(m1, m2),
::testing::AllOf(m3, m4, m5));
}

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6>
inline typename internal::AllOfResult6<M1, M2, M3, M4, M5, M6>::type
AllOf(M1 m1, M2 m2, M3 m3, M4 m4, M5 m5, M6 m6) {
return typename internal::AllOfResult6<M1, M2, M3, M4, M5, M6>::type(
::testing::AllOf(m1, m2, m3),
::testing::AllOf(m4, m5, m6));
}

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7>
inline typename internal::AllOfResult7<M1, M2, M3, M4, M5, M6, M7>::type
AllOf(M1 m1, M2 m2, M3 m3, M4 m4, M5 m5, M6 m6, M7 m7) {
return typename internal::AllOfResult7<M1, M2, M3, M4, M5, M6, M7>::type(
::testing::AllOf(m1, m2, m3),
::testing::AllOf(m4, m5, m6, m7));
}

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7, typename M8>
inline typename internal::AllOfResult8<M1, M2, M3, M4, M5, M6, M7, M8>::type
AllOf(M1 m1, M2 m2, M3 m3, M4 m4, M5 m5, M6 m6, M7 m7, M8 m8) {
return typename internal::AllOfResult8<M1, M2, M3, M4, M5, M6, M7, M8>::type(
::testing::AllOf(m1, m2, m3, m4),
::testing::AllOf(m5, m6, m7, m8));
}

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7, typename M8, typename M9>
inline typename internal::AllOfResult9<M1, M2, M3, M4, M5, M6, M7, M8, M9>::type
AllOf(M1 m1, M2 m2, M3 m3, M4 m4, M5 m5, M6 m6, M7 m7, M8 m8, M9 m9) {
return typename internal::AllOfResult9<M1, M2, M3, M4, M5, M6, M7, M8,
M9>::type(
::testing::AllOf(m1, m2, m3, m4),
::testing::AllOf(m5, m6, m7, m8, m9));
}

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7, typename M8, typename M9, typename M10>
inline typename internal::AllOfResult10<M1, M2, M3, M4, M5, M6, M7, M8, M9,
M10>::type
AllOf(M1 m1, M2 m2, M3 m3, M4 m4, M5 m5, M6 m6, M7 m7, M8 m8, M9 m9, M10 m10) {
return typename internal::AllOfResult10<M1, M2, M3, M4, M5, M6, M7, M8, M9,
M10>::type(
::testing::AllOf(m1, m2, m3, m4, m5),
::testing::AllOf(m6, m7, m8, m9, m10));
}




template <typename M1, typename M2>
inline typename internal::AnyOfResult2<M1, M2>::type
AnyOf(M1 m1, M2 m2) {
return typename internal::AnyOfResult2<M1, M2>::type(
m1,
m2);
}

template <typename M1, typename M2, typename M3>
inline typename internal::AnyOfResult3<M1, M2, M3>::type
AnyOf(M1 m1, M2 m2, M3 m3) {
return typename internal::AnyOfResult3<M1, M2, M3>::type(
m1,
::testing::AnyOf(m2, m3));
}

template <typename M1, typename M2, typename M3, typename M4>
inline typename internal::AnyOfResult4<M1, M2, M3, M4>::type
AnyOf(M1 m1, M2 m2, M3 m3, M4 m4) {
return typename internal::AnyOfResult4<M1, M2, M3, M4>::type(
::testing::AnyOf(m1, m2),
::testing::AnyOf(m3, m4));
}

template <typename M1, typename M2, typename M3, typename M4, typename M5>
inline typename internal::AnyOfResult5<M1, M2, M3, M4, M5>::type
AnyOf(M1 m1, M2 m2, M3 m3, M4 m4, M5 m5) {
return typename internal::AnyOfResult5<M1, M2, M3, M4, M5>::type(
::testing::AnyOf(m1, m2),
::testing::AnyOf(m3, m4, m5));
}

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6>
inline typename internal::AnyOfResult6<M1, M2, M3, M4, M5, M6>::type
AnyOf(M1 m1, M2 m2, M3 m3, M4 m4, M5 m5, M6 m6) {
return typename internal::AnyOfResult6<M1, M2, M3, M4, M5, M6>::type(
::testing::AnyOf(m1, m2, m3),
::testing::AnyOf(m4, m5, m6));
}

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7>
inline typename internal::AnyOfResult7<M1, M2, M3, M4, M5, M6, M7>::type
AnyOf(M1 m1, M2 m2, M3 m3, M4 m4, M5 m5, M6 m6, M7 m7) {
return typename internal::AnyOfResult7<M1, M2, M3, M4, M5, M6, M7>::type(
::testing::AnyOf(m1, m2, m3),
::testing::AnyOf(m4, m5, m6, m7));
}

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7, typename M8>
inline typename internal::AnyOfResult8<M1, M2, M3, M4, M5, M6, M7, M8>::type
AnyOf(M1 m1, M2 m2, M3 m3, M4 m4, M5 m5, M6 m6, M7 m7, M8 m8) {
return typename internal::AnyOfResult8<M1, M2, M3, M4, M5, M6, M7, M8>::type(
::testing::AnyOf(m1, m2, m3, m4),
::testing::AnyOf(m5, m6, m7, m8));
}

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7, typename M8, typename M9>
inline typename internal::AnyOfResult9<M1, M2, M3, M4, M5, M6, M7, M8, M9>::type
AnyOf(M1 m1, M2 m2, M3 m3, M4 m4, M5 m5, M6 m6, M7 m7, M8 m8, M9 m9) {
return typename internal::AnyOfResult9<M1, M2, M3, M4, M5, M6, M7, M8,
M9>::type(
::testing::AnyOf(m1, m2, m3, m4),
::testing::AnyOf(m5, m6, m7, m8, m9));
}

template <typename M1, typename M2, typename M3, typename M4, typename M5,
typename M6, typename M7, typename M8, typename M9, typename M10>
inline typename internal::AnyOfResult10<M1, M2, M3, M4, M5, M6, M7, M8, M9,
M10>::type
AnyOf(M1 m1, M2 m2, M3 m3, M4 m4, M5 m5, M6 m6, M7 m7, M8 m8, M9 m9, M10 m10) {
return typename internal::AnyOfResult10<M1, M2, M3, M4, M5, M6, M7, M8, M9,
M10>::type(
::testing::AnyOf(m1, m2, m3, m4, m5),
::testing::AnyOf(m6, m7, m8, m9, m10));
}

}


























































































































































































































#define MATCHER(name, description)class name##Matcher {public:template <typename arg_type>class gmock_Impl : public ::testing::MatcherInterface<GTEST_REFERENCE_TO_CONST_(arg_type)> {public:gmock_Impl(){}virtual bool MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener) const;virtual void DescribeTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(false);}virtual void DescribeNegationTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(true);}private:::std::string FormatDescription(bool negation) const {::std::string gmock_description = (description);if (!gmock_description.empty())return gmock_description;return ::testing::internal::FormatMatcherDescription(negation, #name, ::testing::internal::UniversalTersePrintTupleFieldsToStrings(::testing::tuple<>()));}};template <typename arg_type>operator ::testing::Matcher<arg_type>() const {return ::testing::Matcher<arg_type>(new gmock_Impl<arg_type>());}name##Matcher() {}private:};inline name##Matcher name() {return name##Matcher();}template <typename arg_type>bool name##Matcher::gmock_Impl<arg_type>::MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener GTEST_ATTRIBUTE_UNUSED_)const














































#define MATCHER_P(name, p0, description)template <typename p0##_type>class name##MatcherP {public:template <typename arg_type>class gmock_Impl : public ::testing::MatcherInterface<GTEST_REFERENCE_TO_CONST_(arg_type)> {public:explicit gmock_Impl(p0##_type gmock_p0): p0(::testing::internal::move(gmock_p0)) {}virtual bool MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener) const;virtual void DescribeTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(false);}virtual void DescribeNegationTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(true);}p0##_type const p0;private:::std::string FormatDescription(bool negation) const {::std::string gmock_description = (description);if (!gmock_description.empty())return gmock_description;return ::testing::internal::FormatMatcherDescription(negation, #name, ::testing::internal::UniversalTersePrintTupleFieldsToStrings(::testing::tuple<p0##_type>(p0)));}};template <typename arg_type>operator ::testing::Matcher<arg_type>() const {return ::testing::Matcher<arg_type>(new gmock_Impl<arg_type>(p0));}explicit name##MatcherP(p0##_type gmock_p0) : p0(::testing::internal::move(gmock_p0)) {}p0##_type const p0;private:};template <typename p0##_type>inline name##MatcherP<p0##_type> name(p0##_type p0) {return name##MatcherP<p0##_type>(p0);}template <typename p0##_type>template <typename arg_type>bool name##MatcherP<p0##_type>::gmock_Impl<arg_type>::MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener GTEST_ATTRIBUTE_UNUSED_)const




















































#define MATCHER_P2(name, p0, p1, description)template <typename p0##_type, typename p1##_type>class name##MatcherP2 {public:template <typename arg_type>class gmock_Impl : public ::testing::MatcherInterface<GTEST_REFERENCE_TO_CONST_(arg_type)> {public:gmock_Impl(p0##_type gmock_p0, p1##_type gmock_p1): p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)) {}virtual bool MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener) const;virtual void DescribeTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(false);}virtual void DescribeNegationTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(true);}p0##_type const p0;p1##_type const p1;private:::std::string FormatDescription(bool negation) const {::std::string gmock_description = (description);if (!gmock_description.empty())return gmock_description;return ::testing::internal::FormatMatcherDescription(negation, #name, ::testing::internal::UniversalTersePrintTupleFieldsToStrings(::testing::tuple<p0##_type, p1##_type>(p0, p1)));}};template <typename arg_type>operator ::testing::Matcher<arg_type>() const {return ::testing::Matcher<arg_type>(new gmock_Impl<arg_type>(p0, p1));}name##MatcherP2(p0##_type gmock_p0, p1##_type gmock_p1) : p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)) {}p0##_type const p0;p1##_type const p1;private:};template <typename p0##_type, typename p1##_type>inline name##MatcherP2<p0##_type, p1##_type> name(p0##_type p0, p1##_type p1) {return name##MatcherP2<p0##_type, p1##_type>(p0, p1);}template <typename p0##_type, typename p1##_type>template <typename arg_type>bool name##MatcherP2<p0##_type, p1##_type>::gmock_Impl<arg_type>::MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener GTEST_ATTRIBUTE_UNUSED_)const


























































#define MATCHER_P3(name, p0, p1, p2, description)template <typename p0##_type, typename p1##_type, typename p2##_type>class name##MatcherP3 {public:template <typename arg_type>class gmock_Impl : public ::testing::MatcherInterface<GTEST_REFERENCE_TO_CONST_(arg_type)> {public:gmock_Impl(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2): p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)) {}virtual bool MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener) const;virtual void DescribeTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(false);}virtual void DescribeNegationTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(true);}p0##_type const p0;p1##_type const p1;p2##_type const p2;private:::std::string FormatDescription(bool negation) const {::std::string gmock_description = (description);if (!gmock_description.empty())return gmock_description;return ::testing::internal::FormatMatcherDescription(negation, #name, ::testing::internal::UniversalTersePrintTupleFieldsToStrings(::testing::tuple<p0##_type, p1##_type, p2##_type>(p0, p1, p2)));}};template <typename arg_type>operator ::testing::Matcher<arg_type>() const {return ::testing::Matcher<arg_type>(new gmock_Impl<arg_type>(p0, p1, p2));}name##MatcherP3(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2) : p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)) {}p0##_type const p0;p1##_type const p1;p2##_type const p2;private:};template <typename p0##_type, typename p1##_type, typename p2##_type>inline name##MatcherP3<p0##_type, p1##_type, p2##_type> name(p0##_type p0, p1##_type p1, p2##_type p2) {return name##MatcherP3<p0##_type, p1##_type, p2##_type>(p0, p1, p2);}template <typename p0##_type, typename p1##_type, typename p2##_type>template <typename arg_type>bool name##MatcherP3<p0##_type, p1##_type, p2##_type>::gmock_Impl<arg_type>::MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener GTEST_ATTRIBUTE_UNUSED_)const































































#define MATCHER_P4(name, p0, p1, p2, p3, description)template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type>class name##MatcherP4 {public:template <typename arg_type>class gmock_Impl : public ::testing::MatcherInterface<GTEST_REFERENCE_TO_CONST_(arg_type)> {public:gmock_Impl(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3): p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)) {}virtual bool MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener) const;virtual void DescribeTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(false);}virtual void DescribeNegationTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(true);}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;private:::std::string FormatDescription(bool negation) const {::std::string gmock_description = (description);if (!gmock_description.empty())return gmock_description;return ::testing::internal::FormatMatcherDescription(negation, #name, ::testing::internal::UniversalTersePrintTupleFieldsToStrings(::testing::tuple<p0##_type, p1##_type, p2##_type, p3##_type>(p0, p1, p2, p3)));}};template <typename arg_type>operator ::testing::Matcher<arg_type>() const {return ::testing::Matcher<arg_type>(new gmock_Impl<arg_type>(p0, p1, p2, p3));}name##MatcherP4(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3) : p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)) {}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;private:};template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type>inline name##MatcherP4<p0##_type, p1##_type, p2##_type, p3##_type> name(p0##_type p0, p1##_type p1, p2##_type p2, p3##_type p3) {return name##MatcherP4<p0##_type, p1##_type, p2##_type, p3##_type>(p0, p1, p2, p3);}template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type>template <typename arg_type>bool name##MatcherP4<p0##_type, p1##_type, p2##_type, p3##_type>::gmock_Impl<arg_type>::MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener GTEST_ATTRIBUTE_UNUSED_)const










































































#define MATCHER_P5(name, p0, p1, p2, p3, p4, description)template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type>class name##MatcherP5 {public:template <typename arg_type>class gmock_Impl : public ::testing::MatcherInterface<GTEST_REFERENCE_TO_CONST_(arg_type)> {public:gmock_Impl(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3, p4##_type gmock_p4): p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)), p4(::testing::internal::move(gmock_p4)) {}virtual bool MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener) const;virtual void DescribeTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(false);}virtual void DescribeNegationTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(true);}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;p4##_type const p4;private:::std::string FormatDescription(bool negation) const {::std::string gmock_description = (description);if (!gmock_description.empty())return gmock_description;return ::testing::internal::FormatMatcherDescription(negation, #name, ::testing::internal::UniversalTersePrintTupleFieldsToStrings(::testing::tuple<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type>(p0, p1, p2, p3, p4)));}};template <typename arg_type>operator ::testing::Matcher<arg_type>() const {return ::testing::Matcher<arg_type>(new gmock_Impl<arg_type>(p0, p1, p2, p3, p4));}name##MatcherP5(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3, p4##_type gmock_p4) : p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)), p4(::testing::internal::move(gmock_p4)) {}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;p4##_type const p4;private:};template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type>inline name##MatcherP5<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type> name(p0##_type p0, p1##_type p1, p2##_type p2, p3##_type p3, p4##_type p4) {return name##MatcherP5<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type>(p0, p1, p2, p3, p4);}template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type>template <typename arg_type>bool name##MatcherP5<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type>::gmock_Impl<arg_type>::MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener GTEST_ATTRIBUTE_UNUSED_)const














































































#define MATCHER_P6(name, p0, p1, p2, p3, p4, p5, description)template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type>class name##MatcherP6 {public:template <typename arg_type>class gmock_Impl : public ::testing::MatcherInterface<GTEST_REFERENCE_TO_CONST_(arg_type)> {public:gmock_Impl(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3, p4##_type gmock_p4, p5##_type gmock_p5): p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)), p4(::testing::internal::move(gmock_p4)), p5(::testing::internal::move(gmock_p5)) {}virtual bool MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener) const;virtual void DescribeTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(false);}virtual void DescribeNegationTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(true);}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;p4##_type const p4;p5##_type const p5;private:::std::string FormatDescription(bool negation) const {::std::string gmock_description = (description);if (!gmock_description.empty())return gmock_description;return ::testing::internal::FormatMatcherDescription(negation, #name, ::testing::internal::UniversalTersePrintTupleFieldsToStrings(::testing::tuple<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type>(p0, p1, p2, p3, p4, p5)));}};template <typename arg_type>operator ::testing::Matcher<arg_type>() const {return ::testing::Matcher<arg_type>(new gmock_Impl<arg_type>(p0, p1, p2, p3, p4, p5));}name##MatcherP6(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3, p4##_type gmock_p4, p5##_type gmock_p5) : p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)), p4(::testing::internal::move(gmock_p4)), p5(::testing::internal::move(gmock_p5)) {}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;p4##_type const p4;p5##_type const p5;private:};template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type>inline name##MatcherP6<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type> name(p0##_type p0, p1##_type p1, p2##_type p2, p3##_type p3, p4##_type p4, p5##_type p5) {return name##MatcherP6<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type>(p0, p1, p2, p3, p4, p5);}template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type>template <typename arg_type>bool name##MatcherP6<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type>::gmock_Impl<arg_type>::MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener GTEST_ATTRIBUTE_UNUSED_)const


















































































#define MATCHER_P7(name, p0, p1, p2, p3, p4, p5, p6, description)template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type, typename p6##_type>class name##MatcherP7 {public:template <typename arg_type>class gmock_Impl : public ::testing::MatcherInterface<GTEST_REFERENCE_TO_CONST_(arg_type)> {public:gmock_Impl(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3, p4##_type gmock_p4, p5##_type gmock_p5, p6##_type gmock_p6): p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)), p4(::testing::internal::move(gmock_p4)), p5(::testing::internal::move(gmock_p5)), p6(::testing::internal::move(gmock_p6)) {}virtual bool MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener) const;virtual void DescribeTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(false);}virtual void DescribeNegationTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(true);}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;p4##_type const p4;p5##_type const p5;p6##_type const p6;private:::std::string FormatDescription(bool negation) const {::std::string gmock_description = (description);if (!gmock_description.empty())return gmock_description;return ::testing::internal::FormatMatcherDescription(negation, #name, ::testing::internal::UniversalTersePrintTupleFieldsToStrings(::testing::tuple<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type>(p0, p1, p2, p3, p4, p5, p6)));}};template <typename arg_type>operator ::testing::Matcher<arg_type>() const {return ::testing::Matcher<arg_type>(new gmock_Impl<arg_type>(p0, p1, p2, p3, p4, p5, p6));}name##MatcherP7(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3, p4##_type gmock_p4, p5##_type gmock_p5, p6##_type gmock_p6) : p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)), p4(::testing::internal::move(gmock_p4)), p5(::testing::internal::move(gmock_p5)), p6(::testing::internal::move(gmock_p6)) {}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;p4##_type const p4;p5##_type const p5;p6##_type const p6;private:};template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type, typename p6##_type>inline name##MatcherP7<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type> name(p0##_type p0, p1##_type p1, p2##_type p2, p3##_type p3, p4##_type p4, p5##_type p5, p6##_type p6) {return name##MatcherP7<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type>(p0, p1, p2, p3, p4, p5, p6);}template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type, typename p6##_type>template <typename arg_type>bool name##MatcherP7<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type>::gmock_Impl<arg_type>::MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener GTEST_ATTRIBUTE_UNUSED_)const





























































































#define MATCHER_P8(name, p0, p1, p2, p3, p4, p5, p6, p7, description)template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type, typename p6##_type, typename p7##_type>class name##MatcherP8 {public:template <typename arg_type>class gmock_Impl : public ::testing::MatcherInterface<GTEST_REFERENCE_TO_CONST_(arg_type)> {public:gmock_Impl(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3, p4##_type gmock_p4, p5##_type gmock_p5, p6##_type gmock_p6, p7##_type gmock_p7): p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)), p4(::testing::internal::move(gmock_p4)), p5(::testing::internal::move(gmock_p5)), p6(::testing::internal::move(gmock_p6)), p7(::testing::internal::move(gmock_p7)) {}virtual bool MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener) const;virtual void DescribeTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(false);}virtual void DescribeNegationTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(true);}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;p4##_type const p4;p5##_type const p5;p6##_type const p6;p7##_type const p7;private:::std::string FormatDescription(bool negation) const {::std::string gmock_description = (description);if (!gmock_description.empty())return gmock_description;return ::testing::internal::FormatMatcherDescription(negation, #name, ::testing::internal::UniversalTersePrintTupleFieldsToStrings(::testing::tuple<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type, p7##_type>(p0, p1, p2, p3, p4, p5, p6, p7)));}};template <typename arg_type>operator ::testing::Matcher<arg_type>() const {return ::testing::Matcher<arg_type>(new gmock_Impl<arg_type>(p0, p1, p2, p3, p4, p5, p6, p7));}name##MatcherP8(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3, p4##_type gmock_p4, p5##_type gmock_p5, p6##_type gmock_p6, p7##_type gmock_p7) : p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)), p4(::testing::internal::move(gmock_p4)), p5(::testing::internal::move(gmock_p5)), p6(::testing::internal::move(gmock_p6)), p7(::testing::internal::move(gmock_p7)) {}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;p4##_type const p4;p5##_type const p5;p6##_type const p6;p7##_type const p7;private:};template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type, typename p6##_type, typename p7##_type>inline name##MatcherP8<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type, p7##_type> name(p0##_type p0, p1##_type p1, p2##_type p2, p3##_type p3, p4##_type p4, p5##_type p5, p6##_type p6, p7##_type p7) {return name##MatcherP8<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type, p7##_type>(p0, p1, p2, p3, p4, p5, p6, p7);}template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type, typename p6##_type, typename p7##_type>template <typename arg_type>bool name##MatcherP8<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type, p7##_type>::gmock_Impl<arg_type>::MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener GTEST_ATTRIBUTE_UNUSED_)const



































































































#define MATCHER_P9(name, p0, p1, p2, p3, p4, p5, p6, p7, p8, description)template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type, typename p6##_type, typename p7##_type, typename p8##_type>class name##MatcherP9 {public:template <typename arg_type>class gmock_Impl : public ::testing::MatcherInterface<GTEST_REFERENCE_TO_CONST_(arg_type)> {public:gmock_Impl(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3, p4##_type gmock_p4, p5##_type gmock_p5, p6##_type gmock_p6, p7##_type gmock_p7, p8##_type gmock_p8): p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)), p4(::testing::internal::move(gmock_p4)), p5(::testing::internal::move(gmock_p5)), p6(::testing::internal::move(gmock_p6)), p7(::testing::internal::move(gmock_p7)), p8(::testing::internal::move(gmock_p8)) {}virtual bool MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener) const;virtual void DescribeTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(false);}virtual void DescribeNegationTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(true);}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;p4##_type const p4;p5##_type const p5;p6##_type const p6;p7##_type const p7;p8##_type const p8;private:::std::string FormatDescription(bool negation) const {::std::string gmock_description = (description);if (!gmock_description.empty())return gmock_description;return ::testing::internal::FormatMatcherDescription(negation, #name, ::testing::internal::UniversalTersePrintTupleFieldsToStrings(::testing::tuple<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type, p7##_type, p8##_type>(p0, p1, p2, p3, p4, p5, p6, p7, p8)));}};template <typename arg_type>operator ::testing::Matcher<arg_type>() const {return ::testing::Matcher<arg_type>(new gmock_Impl<arg_type>(p0, p1, p2, p3, p4, p5, p6, p7, p8));}name##MatcherP9(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3, p4##_type gmock_p4, p5##_type gmock_p5, p6##_type gmock_p6, p7##_type gmock_p7, p8##_type gmock_p8) : p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)), p4(::testing::internal::move(gmock_p4)), p5(::testing::internal::move(gmock_p5)), p6(::testing::internal::move(gmock_p6)), p7(::testing::internal::move(gmock_p7)), p8(::testing::internal::move(gmock_p8)) {}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;p4##_type const p4;p5##_type const p5;p6##_type const p6;p7##_type const p7;p8##_type const p8;private:};template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type, typename p6##_type, typename p7##_type, typename p8##_type>inline name##MatcherP9<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type, p7##_type, p8##_type> name(p0##_type p0, p1##_type p1, p2##_type p2, p3##_type p3, p4##_type p4, p5##_type p5, p6##_type p6, p7##_type p7, p8##_type p8) {return name##MatcherP9<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type, p7##_type, p8##_type>(p0, p1, p2, p3, p4, p5, p6, p7, p8);}template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type, typename p6##_type, typename p7##_type, typename p8##_type>template <typename arg_type>bool name##MatcherP9<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type, p7##_type, p8##_type>::gmock_Impl<arg_type>::MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener GTEST_ATTRIBUTE_UNUSED_)const








































































































#define MATCHER_P10(name, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, description)template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type, typename p6##_type, typename p7##_type, typename p8##_type, typename p9##_type>class name##MatcherP10 {public:template <typename arg_type>class gmock_Impl : public ::testing::MatcherInterface<GTEST_REFERENCE_TO_CONST_(arg_type)> {public:gmock_Impl(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3, p4##_type gmock_p4, p5##_type gmock_p5, p6##_type gmock_p6, p7##_type gmock_p7, p8##_type gmock_p8, p9##_type gmock_p9): p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)), p4(::testing::internal::move(gmock_p4)), p5(::testing::internal::move(gmock_p5)), p6(::testing::internal::move(gmock_p6)), p7(::testing::internal::move(gmock_p7)), p8(::testing::internal::move(gmock_p8)), p9(::testing::internal::move(gmock_p9)) {}virtual bool MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener) const;virtual void DescribeTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(false);}virtual void DescribeNegationTo(::std::ostream* gmock_os) const {*gmock_os << FormatDescription(true);}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;p4##_type const p4;p5##_type const p5;p6##_type const p6;p7##_type const p7;p8##_type const p8;p9##_type const p9;private:::std::string FormatDescription(bool negation) const {::std::string gmock_description = (description);if (!gmock_description.empty())return gmock_description;return ::testing::internal::FormatMatcherDescription(negation, #name, ::testing::internal::UniversalTersePrintTupleFieldsToStrings(::testing::tuple<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type, p7##_type, p8##_type, p9##_type>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)));}};template <typename arg_type>operator ::testing::Matcher<arg_type>() const {return ::testing::Matcher<arg_type>(new gmock_Impl<arg_type>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9));}name##MatcherP10(p0##_type gmock_p0, p1##_type gmock_p1, p2##_type gmock_p2, p3##_type gmock_p3, p4##_type gmock_p4, p5##_type gmock_p5, p6##_type gmock_p6, p7##_type gmock_p7, p8##_type gmock_p8, p9##_type gmock_p9) : p0(::testing::internal::move(gmock_p0)), p1(::testing::internal::move(gmock_p1)), p2(::testing::internal::move(gmock_p2)), p3(::testing::internal::move(gmock_p3)), p4(::testing::internal::move(gmock_p4)), p5(::testing::internal::move(gmock_p5)), p6(::testing::internal::move(gmock_p6)), p7(::testing::internal::move(gmock_p7)), p8(::testing::internal::move(gmock_p8)), p9(::testing::internal::move(gmock_p9)) {}p0##_type const p0;p1##_type const p1;p2##_type const p2;p3##_type const p3;p4##_type const p4;p5##_type const p5;p6##_type const p6;p7##_type const p7;p8##_type const p8;p9##_type const p9;private:};template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type, typename p6##_type, typename p7##_type, typename p8##_type, typename p9##_type>inline name##MatcherP10<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type, p7##_type, p8##_type, p9##_type> name(p0##_type p0, p1##_type p1, p2##_type p2, p3##_type p3, p4##_type p4, p5##_type p5, p6##_type p6, p7##_type p7, p8##_type p8, p9##_type p9) {return name##MatcherP10<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type, p7##_type, p8##_type, p9##_type>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);}template <typename p0##_type, typename p1##_type, typename p2##_type, typename p3##_type, typename p4##_type, typename p5##_type, typename p6##_type, typename p7##_type, typename p8##_type, typename p9##_type>template <typename arg_type>bool name##MatcherP10<p0##_type, p1##_type, p2##_type, p3##_type, p4##_type, p5##_type, p6##_type, p7##_type, p8##_type, p9##_type>::gmock_Impl<arg_type>::MatchAndExplain(GTEST_REFERENCE_TO_CONST_(arg_type) arg,::testing::MatchResultListener* result_listener GTEST_ATTRIBUTE_UNUSED_)const

















































































































#endif
