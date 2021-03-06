











#if !defined(SANITIZER_BITVECTOR_H)
#define SANITIZER_BITVECTOR_H

#include "sanitizer_common.h"

namespace __sanitizer {


template <class basic_int_t = uptr>
class BasicBitVector {
public:
enum SizeEnum : uptr { kSize = sizeof(basic_int_t) * 8 };

uptr size() const { return kSize; }

void clear() { bits_ = 0; }
void setAll() { bits_ = ~(basic_int_t)0; }
bool empty() const { return bits_ == 0; }


bool setBit(uptr idx) {
basic_int_t old = bits_;
bits_ |= mask(idx);
return bits_ != old;
}


bool clearBit(uptr idx) {
basic_int_t old = bits_;
bits_ &= ~mask(idx);
return bits_ != old;
}

bool getBit(uptr idx) const { return (bits_ & mask(idx)) != 0; }

uptr getAndClearFirstOne() {
CHECK(!empty());
uptr idx = LeastSignificantSetBitIndex(bits_);
clearBit(idx);
return idx;
}


bool setUnion(const BasicBitVector &v) {
basic_int_t old = bits_;
bits_ |= v.bits_;
return bits_ != old;
}


bool setIntersection(const BasicBitVector &v) {
basic_int_t old = bits_;
bits_ &= v.bits_;
return bits_ != old;
}


bool setDifference(const BasicBitVector &v) {
basic_int_t old = bits_;
bits_ &= ~v.bits_;
return bits_ != old;
}

void copyFrom(const BasicBitVector &v) { bits_ = v.bits_; }


bool intersectsWith(const BasicBitVector &v) const {
return (bits_ & v.bits_) != 0;
}





class Iterator {
public:
Iterator() { }
explicit Iterator(const BasicBitVector &bv) : bv_(bv) {}
bool hasNext() const { return !bv_.empty(); }
uptr next() { return bv_.getAndClearFirstOne(); }
void clear() { bv_.clear(); }
private:
BasicBitVector bv_;
};

private:
basic_int_t mask(uptr idx) const {
CHECK_LT(idx, size());
return (basic_int_t)1UL << idx;
}
basic_int_t bits_;
};




template <uptr kLevel1Size = 1, class BV = BasicBitVector<> >
class TwoLevelBitVector {





public:
enum SizeEnum : uptr { kSize = BV::kSize * BV::kSize * kLevel1Size };


uptr size() const { return kSize; }

void clear() {
for (uptr i = 0; i < kLevel1Size; i++)
l1_[i].clear();
}

void setAll() {
for (uptr i0 = 0; i0 < kLevel1Size; i0++) {
l1_[i0].setAll();
for (uptr i1 = 0; i1 < BV::kSize; i1++)
l2_[i0][i1].setAll();
}
}

bool empty() const {
for (uptr i = 0; i < kLevel1Size; i++)
if (!l1_[i].empty())
return false;
return true;
}


bool setBit(uptr idx) {
check(idx);
uptr i0 = idx0(idx);
uptr i1 = idx1(idx);
uptr i2 = idx2(idx);
if (!l1_[i0].getBit(i1)) {
l1_[i0].setBit(i1);
l2_[i0][i1].clear();
}
bool res = l2_[i0][i1].setBit(i2);


return res;
}

bool clearBit(uptr idx) {
check(idx);
uptr i0 = idx0(idx);
uptr i1 = idx1(idx);
uptr i2 = idx2(idx);
bool res = false;
if (l1_[i0].getBit(i1)) {
res = l2_[i0][i1].clearBit(i2);
if (l2_[i0][i1].empty())
l1_[i0].clearBit(i1);
}
return res;
}

bool getBit(uptr idx) const {
check(idx);
uptr i0 = idx0(idx);
uptr i1 = idx1(idx);
uptr i2 = idx2(idx);

return l1_[i0].getBit(i1) && l2_[i0][i1].getBit(i2);
}

uptr getAndClearFirstOne() {
for (uptr i0 = 0; i0 < kLevel1Size; i0++) {
if (l1_[i0].empty()) continue;
uptr i1 = l1_[i0].getAndClearFirstOne();
uptr i2 = l2_[i0][i1].getAndClearFirstOne();
if (!l2_[i0][i1].empty())
l1_[i0].setBit(i1);
uptr res = i0 * BV::kSize * BV::kSize + i1 * BV::kSize + i2;

return res;
}
CHECK(0);
return 0;
}


bool setUnion(const TwoLevelBitVector &v) {
bool res = false;
for (uptr i0 = 0; i0 < kLevel1Size; i0++) {
BV t = v.l1_[i0];
while (!t.empty()) {
uptr i1 = t.getAndClearFirstOne();
if (l1_[i0].setBit(i1))
l2_[i0][i1].clear();
if (l2_[i0][i1].setUnion(v.l2_[i0][i1]))
res = true;
}
}
return res;
}


bool setIntersection(const TwoLevelBitVector &v) {
bool res = false;
for (uptr i0 = 0; i0 < kLevel1Size; i0++) {
if (l1_[i0].setIntersection(v.l1_[i0]))
res = true;
if (!l1_[i0].empty()) {
BV t = l1_[i0];
while (!t.empty()) {
uptr i1 = t.getAndClearFirstOne();
if (l2_[i0][i1].setIntersection(v.l2_[i0][i1]))
res = true;
if (l2_[i0][i1].empty())
l1_[i0].clearBit(i1);
}
}
}
return res;
}


bool setDifference(const TwoLevelBitVector &v) {
bool res = false;
for (uptr i0 = 0; i0 < kLevel1Size; i0++) {
BV t = l1_[i0];
t.setIntersection(v.l1_[i0]);
while (!t.empty()) {
uptr i1 = t.getAndClearFirstOne();
if (l2_[i0][i1].setDifference(v.l2_[i0][i1]))
res = true;
if (l2_[i0][i1].empty())
l1_[i0].clearBit(i1);
}
}
return res;
}

void copyFrom(const TwoLevelBitVector &v) {
clear();
setUnion(v);
}


bool intersectsWith(const TwoLevelBitVector &v) const {
for (uptr i0 = 0; i0 < kLevel1Size; i0++) {
BV t = l1_[i0];
t.setIntersection(v.l1_[i0]);
while (!t.empty()) {
uptr i1 = t.getAndClearFirstOne();
if (!v.l1_[i0].getBit(i1)) continue;
if (l2_[i0][i1].intersectsWith(v.l2_[i0][i1]))
return true;
}
}
return false;
}





class Iterator {
public:
Iterator() { }
explicit Iterator(const TwoLevelBitVector &bv) : bv_(bv), i0_(0), i1_(0) {
it1_.clear();
it2_.clear();
}

bool hasNext() const {
if (it1_.hasNext()) return true;
for (uptr i = i0_; i < kLevel1Size; i++)
if (!bv_.l1_[i].empty()) return true;
return false;
}

uptr next() {


if (!it1_.hasNext() && !it2_.hasNext()) {
for (; i0_ < kLevel1Size; i0_++) {
if (bv_.l1_[i0_].empty()) continue;
it1_ = typename BV::Iterator(bv_.l1_[i0_]);


break;
}
}
if (!it2_.hasNext()) {
CHECK(it1_.hasNext());
i1_ = it1_.next();
it2_ = typename BV::Iterator(bv_.l2_[i0_][i1_]);


}
CHECK(it2_.hasNext());
uptr i2 = it2_.next();
uptr res = i0_ * BV::kSize * BV::kSize + i1_ * BV::kSize + i2;


if (!it1_.hasNext() && !it2_.hasNext())
i0_++;
return res;
}

private:
const TwoLevelBitVector &bv_;
uptr i0_, i1_;
typename BV::Iterator it1_, it2_;
};

private:
void check(uptr idx) const { CHECK_LE(idx, size()); }

uptr idx0(uptr idx) const {
uptr res = idx / (BV::kSize * BV::kSize);
CHECK_LE(res, kLevel1Size);
return res;
}

uptr idx1(uptr idx) const {
uptr res = (idx / BV::kSize) % BV::kSize;
CHECK_LE(res, BV::kSize);
return res;
}

uptr idx2(uptr idx) const {
uptr res = idx % BV::kSize;
CHECK_LE(res, BV::kSize);
return res;
}

BV l1_[kLevel1Size];
BV l2_[kLevel1Size][BV::kSize];
};

}

#endif
