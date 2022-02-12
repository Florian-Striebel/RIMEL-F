






















































#if !defined(__MT19937_64_H)
#define __MT19937_64_H


void init_genrand64(unsigned long long seed);




void init_by_array64(unsigned long long init_key[],
unsigned long long key_length);


unsigned long long genrand64_int64(void);



long long genrand64_int63(void);


double genrand64_real1(void);


double genrand64_real2(void);


double genrand64_real3(void);


double genrand64_real4(void);

#endif
