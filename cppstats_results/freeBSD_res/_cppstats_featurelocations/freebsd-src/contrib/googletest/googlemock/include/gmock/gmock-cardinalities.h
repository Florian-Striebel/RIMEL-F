





































#if !defined(GMOCK_INCLUDE_GMOCK_GMOCK_CARDINALITIES_H_)
#define GMOCK_INCLUDE_GMOCK_GMOCK_CARDINALITIES_H_

#include <limits.h>
#include <ostream>
#include "gmock/internal/gmock-port.h"
#include "gtest/gtest.h"

GTEST_DISABLE_MSC_WARNINGS_PUSH_(4251 \
)

namespace testing {












class CardinalityInterface {
public:
virtual ~CardinalityInterface() {}



virtual int ConservativeLowerBound() const { return 0; }
virtual int ConservativeUpperBound() const { return INT_MAX; }


virtual bool IsSatisfiedByCallCount(int call_count) const = 0;


virtual bool IsSaturatedByCallCount(int call_count) const = 0;


virtual void DescribeTo(::std::ostream* os) const = 0;
};






class GTEST_API_ Cardinality {
public:


Cardinality() {}


explicit Cardinality(const CardinalityInterface* impl) : impl_(impl) {}



int ConservativeLowerBound() const { return impl_->ConservativeLowerBound(); }
int ConservativeUpperBound() const { return impl_->ConservativeUpperBound(); }


bool IsSatisfiedByCallCount(int call_count) const {
return impl_->IsSatisfiedByCallCount(call_count);
}


bool IsSaturatedByCallCount(int call_count) const {
return impl_->IsSaturatedByCallCount(call_count);
}



bool IsOverSaturatedByCallCount(int call_count) const {
return impl_->IsSaturatedByCallCount(call_count) &&
!impl_->IsSatisfiedByCallCount(call_count);
}


void DescribeTo(::std::ostream* os) const { impl_->DescribeTo(os); }


static void DescribeActualCallCountTo(int actual_call_count,
::std::ostream* os);

private:
internal::linked_ptr<const CardinalityInterface> impl_;
};


GTEST_API_ Cardinality AtLeast(int n);


GTEST_API_ Cardinality AtMost(int n);


GTEST_API_ Cardinality AnyNumber();


GTEST_API_ Cardinality Between(int min, int max);


GTEST_API_ Cardinality Exactly(int n);


inline Cardinality MakeCardinality(const CardinalityInterface* c) {
return Cardinality(c);
}

}

GTEST_DISABLE_MSC_WARNINGS_POP_()

#endif
