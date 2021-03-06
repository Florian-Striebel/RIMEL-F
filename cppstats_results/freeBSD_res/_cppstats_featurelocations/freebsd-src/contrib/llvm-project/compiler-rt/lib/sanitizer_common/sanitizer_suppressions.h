










#if !defined(SANITIZER_SUPPRESSIONS_H)
#define SANITIZER_SUPPRESSIONS_H

#include "sanitizer_common.h"
#include "sanitizer_atomic.h"
#include "sanitizer_internal_defs.h"

namespace __sanitizer {

struct Suppression {
Suppression() { internal_memset(this, 0, sizeof(*this)); }
const char *type;
char *templ;
atomic_uint32_t hit_count;
uptr weight;
};

class SuppressionContext {
public:

SuppressionContext(const char *supprression_types[],
int suppression_types_num);

void ParseFromFile(const char *filename);
void Parse(const char *str);

bool Match(const char *str, const char *type, Suppression **s);
uptr SuppressionCount() const;
bool HasSuppressionType(const char *type) const;
const Suppression *SuppressionAt(uptr i) const;
void GetMatched(InternalMmapVector<Suppression *> *matched);

private:
static const int kMaxSuppressionTypes = 64;
const char **const suppression_types_;
const int suppression_types_num_;

InternalMmapVector<Suppression> suppressions_;
bool has_suppression_type_[kMaxSuppressionTypes];
bool can_parse_;
};

}

#endif
