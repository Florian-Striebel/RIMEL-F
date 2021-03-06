










#if !defined(SANITIZER_ALLOCATOR_H)
#error This file must be included inside sanitizer_allocator.h
#endif








































































































template <uptr kNumBits, uptr kMinSizeLog, uptr kMidSizeLog, uptr kMaxSizeLog,
uptr kMaxNumCachedHintT, uptr kMaxBytesCachedLog>
class SizeClassMap {
static const uptr kMinSize = 1 << kMinSizeLog;
static const uptr kMidSize = 1 << kMidSizeLog;
static const uptr kMidClass = kMidSize / kMinSize;
static const uptr S = kNumBits - 1;
static const uptr M = (1 << S) - 1;

public:


static const uptr kMaxNumCachedHint = kMaxNumCachedHintT;
COMPILER_CHECK((kMaxNumCachedHint & (kMaxNumCachedHint - 1)) == 0);

static const uptr kMaxSize = 1UL << kMaxSizeLog;
static const uptr kNumClasses =
kMidClass + ((kMaxSizeLog - kMidSizeLog) << S) + 1 + 1;
static const uptr kLargestClassID = kNumClasses - 2;
static const uptr kBatchClassID = kNumClasses - 1;
COMPILER_CHECK(kNumClasses >= 16 && kNumClasses <= 256);
static const uptr kNumClassesRounded =
kNumClasses <= 32 ? 32 :
kNumClasses <= 64 ? 64 :
kNumClasses <= 128 ? 128 : 256;

static uptr Size(uptr class_id) {



if (UNLIKELY(class_id == kBatchClassID))
return kMaxNumCachedHint * sizeof(uptr);
if (class_id <= kMidClass)
return kMinSize * class_id;
class_id -= kMidClass;
uptr t = kMidSize << (class_id >> S);
return t + (t >> S) * (class_id & M);
}

static uptr ClassID(uptr size) {
if (UNLIKELY(size > kMaxSize))
return 0;
if (size <= kMidSize)
return (size + kMinSize - 1) >> kMinSizeLog;
const uptr l = MostSignificantSetBitIndex(size);
const uptr hbits = (size >> (l - S)) & M;
const uptr lbits = size & ((1U << (l - S)) - 1);
const uptr l1 = l - kMidSizeLog;
return kMidClass + (l1 << S) + hbits + (lbits > 0);
}

static uptr MaxCachedHint(uptr size) {
DCHECK_LE(size, kMaxSize);
if (UNLIKELY(size == 0))
return 0;
uptr n;

if (kMaxBytesCachedLog > 31 || kMaxSizeLog > 31)
n = (1UL << kMaxBytesCachedLog) / size;
else
n = (1U << kMaxBytesCachedLog) / static_cast<u32>(size);
return Max<uptr>(1U, Min(kMaxNumCachedHint, n));
}

static void Print() {
uptr prev_s = 0;
uptr total_cached = 0;
for (uptr i = 0; i < kNumClasses; i++) {
uptr s = Size(i);
if (s >= kMidSize / 2 && (s & (s - 1)) == 0)
Printf("\n");
uptr d = s - prev_s;
uptr p = prev_s ? (d * 100 / prev_s) : 0;
uptr l = s ? MostSignificantSetBitIndex(s) : 0;
uptr cached = MaxCachedHint(s) * s;
if (i == kBatchClassID)
d = p = l = 0;
Printf("c%02zd => s: %zd diff: +%zd %02zd%% l %zd "
"cached: %zd %zd; id %zd\n",
i, Size(i), d, p, l, MaxCachedHint(s), cached, ClassID(s));
total_cached += cached;
prev_s = s;
}
Printf("Total cached: %zd\n", total_cached);
}

static void Validate() {
for (uptr c = 1; c < kNumClasses; c++) {

uptr s = Size(c);
CHECK_NE(s, 0U);
if (c == kBatchClassID)
continue;
CHECK_EQ(ClassID(s), c);
if (c < kLargestClassID)
CHECK_EQ(ClassID(s + 1), c + 1);
CHECK_EQ(ClassID(s - 1), c);
CHECK_GT(Size(c), Size(c - 1));
}
CHECK_EQ(ClassID(kMaxSize + 1), 0);

for (uptr s = 1; s <= kMaxSize; s++) {
uptr c = ClassID(s);

CHECK_LT(c, kNumClasses);
CHECK_GE(Size(c), s);
if (c > 0)
CHECK_LT(Size(c - 1), s);
}
}
};

typedef SizeClassMap<3, 4, 8, 17, 128, 16> DefaultSizeClassMap;
typedef SizeClassMap<3, 4, 8, 17, 64, 14> CompactSizeClassMap;
typedef SizeClassMap<2, 5, 9, 16, 64, 14> VeryCompactSizeClassMap;




typedef SizeClassMap<3, 4, 8, 17, 8, 10> DenseSizeClassMap;


typedef SizeClassMap<2, 5, 9, 16, 8, 10> VeryDenseSizeClassMap;
