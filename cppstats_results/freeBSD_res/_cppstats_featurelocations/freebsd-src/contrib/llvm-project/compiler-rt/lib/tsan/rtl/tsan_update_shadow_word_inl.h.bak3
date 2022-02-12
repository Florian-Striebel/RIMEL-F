













do {
const unsigned kAccessSize = 1 << kAccessSizeLog;
u64 *sp = &shadow_mem[idx];
old = LoadShadow(sp);
if (LIKELY(old.IsZero())) {
if (!stored) {
StoreIfNotYetStored(sp, &store_word);
stored = true;
}
break;
}

if (LIKELY(Shadow::Addr0AndSizeAreEqual(cur, old))) {

if (LIKELY(Shadow::TidsAreEqual(old, cur))) {
if (LIKELY(old.IsRWWeakerOrEqual(kAccessIsWrite, kIsAtomic))) {
StoreIfNotYetStored(sp, &store_word);
stored = true;
}
break;
}
if (HappensBefore(old, thr)) {
if (old.IsRWWeakerOrEqual(kAccessIsWrite, kIsAtomic)) {
StoreIfNotYetStored(sp, &store_word);
stored = true;
}
break;
}
if (LIKELY(old.IsBothReadsOrAtomic(kAccessIsWrite, kIsAtomic)))
break;
goto RACE;
}

if (Shadow::TwoRangesIntersect(old, cur, kAccessSize)) {
if (Shadow::TidsAreEqual(old, cur))
break;
if (old.IsBothReadsOrAtomic(kAccessIsWrite, kIsAtomic))
break;
if (LIKELY(HappensBefore(old, thr)))
break;
goto RACE;
}

break;
} while (0);
