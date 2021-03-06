








#if !defined(HDR_HISTOGRAM_H)
#define HDR_HISTOGRAM_H 1

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
struct hdr_histogram
{
int64_t lowest_trackable_value;
int64_t highest_trackable_value;
int32_t unit_magnitude;
int32_t significant_figures;
int32_t sub_bucket_half_count_magnitude;
int32_t sub_bucket_half_count;
int64_t sub_bucket_mask;
int32_t sub_bucket_count;
int32_t bucket_count;
int64_t min_value;
int64_t max_value;
int32_t normalizing_index_offset;
double conversion_ratio;
int32_t counts_len;
int64_t total_count;
int64_t* counts;
};

#if defined(__cplusplus)
extern "C" {
#endif





















int hdr_init(
int64_t lowest_trackable_value,
int64_t highest_trackable_value,
int significant_figures,
struct hdr_histogram** result);






void hdr_close(struct hdr_histogram* h);







int hdr_alloc(int64_t highest_trackable_value, int significant_figures, struct hdr_histogram** result);











void hdr_reset(struct hdr_histogram* h);







size_t hdr_get_memory_size(struct hdr_histogram* h);










bool hdr_record_value(struct hdr_histogram* h, int64_t value);














bool hdr_record_value_atomic(struct hdr_histogram* h, int64_t value);












bool hdr_record_values(struct hdr_histogram* h, int64_t value, int64_t count);
















bool hdr_record_values_atomic(struct hdr_histogram* h, int64_t value, int64_t count);
















bool hdr_record_corrected_value(struct hdr_histogram* h, int64_t value, int64_t expexcted_interval);




















bool hdr_record_corrected_value_atomic(struct hdr_histogram* h, int64_t value, int64_t expexcted_interval);












bool hdr_record_corrected_values(struct hdr_histogram* h, int64_t value, int64_t count, int64_t expected_interval);
















bool hdr_record_corrected_values_atomic(struct hdr_histogram* h, int64_t value, int64_t count, int64_t expected_interval);











int64_t hdr_add(struct hdr_histogram* h, const struct hdr_histogram* from);











int64_t hdr_add_while_correcting_for_coordinated_omission(
struct hdr_histogram* h, struct hdr_histogram* from, int64_t expected_interval);







int64_t hdr_min(const struct hdr_histogram* h);







int64_t hdr_max(const struct hdr_histogram* h);







int64_t hdr_value_at_percentile(const struct hdr_histogram* h, double percentile);







double hdr_stddev(const struct hdr_histogram* h);







double hdr_mean(const struct hdr_histogram* h);











bool hdr_values_are_equivalent(const struct hdr_histogram* h, int64_t a, int64_t b);










int64_t hdr_lowest_equivalent_value(const struct hdr_histogram* h, int64_t value);










int64_t hdr_count_at_value(const struct hdr_histogram* h, int64_t value);

int64_t hdr_count_at_index(const struct hdr_histogram* h, int32_t index);

int64_t hdr_value_at_index(const struct hdr_histogram* h, int32_t index);

struct hdr_iter_percentiles
{
bool seen_last_value;
int32_t ticks_per_half_distance;
double percentile_to_iterate_to;
double percentile;
};

struct hdr_iter_recorded
{
int64_t count_added_in_this_iteration_step;
};

struct hdr_iter_linear
{
int64_t value_units_per_bucket;
int64_t count_added_in_this_iteration_step;
int64_t next_value_reporting_level;
int64_t next_value_reporting_level_lowest_equivalent;
};

struct hdr_iter_log
{
double log_base;
int64_t count_added_in_this_iteration_step;
int64_t next_value_reporting_level;
int64_t next_value_reporting_level_lowest_equivalent;
};









struct hdr_iter
{
const struct hdr_histogram* h;

int32_t counts_index;

int64_t total_count;

int64_t count;

int64_t cumulative_count;

int64_t value;
int64_t highest_equivalent_value;
int64_t lowest_equivalent_value;
int64_t median_equivalent_value;
int64_t value_iterated_from;
int64_t value_iterated_to;

union
{
struct hdr_iter_percentiles percentiles;
struct hdr_iter_recorded recorded;
struct hdr_iter_linear linear;
struct hdr_iter_log log;
} specifics;

bool (* _next_fp)(struct hdr_iter* iter);

};







void hdr_iter_init(struct hdr_iter* iter, const struct hdr_histogram* h);




void hdr_iter_percentile_init(struct hdr_iter* iter, const struct hdr_histogram* h, int32_t ticks_per_half_distance);




void hdr_iter_recorded_init(struct hdr_iter* iter, const struct hdr_histogram* h);




void hdr_iter_linear_init(
struct hdr_iter* iter,
const struct hdr_histogram* h,
int64_t value_units_per_bucket);




void hdr_iter_linear_set_value_units_per_bucket(struct hdr_iter* iter, int64_t value_units_per_bucket);




void hdr_iter_log_init(
struct hdr_iter* iter,
const struct hdr_histogram* h,
int64_t value_units_first_bucket,
double log_base);








bool hdr_iter_next(struct hdr_iter* iter);

typedef enum
{
CLASSIC,
CSV
} format_type;













int hdr_percentiles_print(
struct hdr_histogram* h, FILE* stream, int32_t ticks_per_half_distance,
double value_scale, format_type format);




struct hdr_histogram_bucket_config
{
int64_t lowest_trackable_value;
int64_t highest_trackable_value;
int64_t unit_magnitude;
int64_t significant_figures;
int32_t sub_bucket_half_count_magnitude;
int32_t sub_bucket_half_count;
int64_t sub_bucket_mask;
int32_t sub_bucket_count;
int32_t bucket_count;
int32_t counts_len;
};

int hdr_calculate_bucket_config(
int64_t lowest_trackable_value,
int64_t highest_trackable_value,
int significant_figures,
struct hdr_histogram_bucket_config* cfg);

void hdr_init_preallocated(struct hdr_histogram* h, struct hdr_histogram_bucket_config* cfg);

int64_t hdr_size_of_equivalent_value_range(const struct hdr_histogram* h, int64_t value);

int64_t hdr_next_non_equivalent_value(const struct hdr_histogram* h, int64_t value);

int64_t hdr_median_equivalent_value(const struct hdr_histogram* h, int64_t value);





void hdr_reset_internal_counters(struct hdr_histogram* h);

#if defined(__cplusplus)
}
#endif

#endif
