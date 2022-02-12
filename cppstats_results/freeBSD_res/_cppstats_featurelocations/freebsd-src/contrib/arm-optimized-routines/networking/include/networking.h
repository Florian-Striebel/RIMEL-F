






unsigned short __chksum (const void *, unsigned int);
#if __aarch64__ && __ARM_NEON
unsigned short __chksum_aarch64_simd (const void *, unsigned int);
#endif
#if __arm__ && __ARM_NEON
unsigned short __chksum_arm_simd (const void *, unsigned int);
#endif
