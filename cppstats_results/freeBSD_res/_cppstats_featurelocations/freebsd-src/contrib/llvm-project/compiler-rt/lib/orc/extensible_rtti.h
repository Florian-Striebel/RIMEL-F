

























































#if !defined(ORC_RT_EXTENSIBLE_RTTI_H)
#define ORC_RT_EXTENSIBLE_RTTI_H

namespace __orc_rt {

template <typename ThisT, typename ParentT> class RTTIExtends;





class RTTIRoot {
public:
virtual ~RTTIRoot() = default;


static const void *classID() { return &ID; }


virtual const void *dynamicClassID() const = 0;


virtual bool isA(const void *const ClassID) const {
return ClassID == classID();
}


template <typename QueryT> bool isA() const { return isA(QueryT::classID()); }

static bool classof(const RTTIRoot *R) { return R->isA<RTTIRoot>(); }

private:
virtual void anchor();

static char ID;
};




















template <typename ThisT, typename ParentT> class RTTIExtends : public ParentT {
public:

using ParentT::isA;
using ParentT::ParentT;

static char ID;

static const void *classID() { return &ThisT::ID; }

const void *dynamicClassID() const override { return &ThisT::ID; }

bool isA(const void *const ClassID) const override {
return ClassID == classID() || ParentT::isA(ClassID);
}

static bool classof(const RTTIRoot *R) { return R->isA<ThisT>(); }
};

template <typename ThisT, typename ParentT>
char RTTIExtends<ThisT, ParentT>::ID = 0;



template <typename To, typename From> bool isa(const From &Value) {
return To::classof(&Value);
}

}

#endif
