typedef struct base_block_s base_block_t;
typedef struct base_s base_t;
#define METADATA_THP_DEFAULT metadata_thp_disabled
#define BASE_AUTO_THP_THRESHOLD 2
#define BASE_AUTO_THP_THRESHOLD_A0 5
typedef enum {
metadata_thp_disabled = 0,
metadata_thp_auto = 1,
metadata_thp_always = 2,
metadata_thp_mode_limit = 3
} metadata_thp_mode_t;
