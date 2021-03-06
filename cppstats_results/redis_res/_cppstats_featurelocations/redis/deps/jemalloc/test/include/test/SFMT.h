
































































#if !defined(SFMT_H)
#define SFMT_H

typedef struct sfmt_s sfmt_t;

uint32_t gen_rand32(sfmt_t *ctx);
uint32_t gen_rand32_range(sfmt_t *ctx, uint32_t limit);
uint64_t gen_rand64(sfmt_t *ctx);
uint64_t gen_rand64_range(sfmt_t *ctx, uint64_t limit);
void fill_array32(sfmt_t *ctx, uint32_t *array, int size);
void fill_array64(sfmt_t *ctx, uint64_t *array, int size);
sfmt_t *init_gen_rand(uint32_t seed);
sfmt_t *init_by_array(uint32_t *init_key, int key_length);
void fini_gen_rand(sfmt_t *ctx);
const char *get_idstring(void);
int get_min_array_size32(void);
int get_min_array_size64(void);



static inline double to_real1(uint32_t v) {
return v * (1.0/4294967295.0);

}


static inline double genrand_real1(sfmt_t *ctx) {
return to_real1(gen_rand32(ctx));
}


static inline double to_real2(uint32_t v) {
return v * (1.0/4294967296.0);

}


static inline double genrand_real2(sfmt_t *ctx) {
return to_real2(gen_rand32(ctx));
}


static inline double to_real3(uint32_t v) {
return (((double)v) + 0.5)*(1.0/4294967296.0);

}


static inline double genrand_real3(sfmt_t *ctx) {
return to_real3(gen_rand32(ctx));
}



static inline double to_res53(uint64_t v) {
return v * (1.0/18446744073709551616.0L);
}



static inline double to_res53_mix(uint32_t x, uint32_t y) {
return to_res53(x | ((uint64_t)y << 32));
}



static inline double genrand_res53(sfmt_t *ctx) {
return to_res53(gen_rand64(ctx));
}




static inline double genrand_res53_mix(sfmt_t *ctx) {
uint32_t x, y;

x = gen_rand32(ctx);
y = gen_rand32(ctx);
return to_res53_mix(x, y);
}
#endif
