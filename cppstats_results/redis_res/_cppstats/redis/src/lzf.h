#define LZF_VERSION 0x0105
size_t
lzf_compress (const void *const in_data, size_t in_len,
void *out_data, size_t out_len);
size_t
lzf_decompress (const void *const in_data, size_t in_len,
void *out_data, size_t out_len);
