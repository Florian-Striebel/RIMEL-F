


























#if !defined(ASAN_SCARINESS_SCORE_H)
#define ASAN_SCARINESS_SCORE_H

#include "asan_flags.h"
#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_libc.h"

namespace __asan {
struct ScarinessScoreBase {
void Clear() {
descr[0] = 0;
score = 0;
}
void Scare(int add_to_score, const char *reason) {
if (descr[0])
internal_strlcat(descr, "-", sizeof(descr));
internal_strlcat(descr, reason, sizeof(descr));
score += add_to_score;
}
int GetScore() const { return score; }
const char *GetDescription() const { return descr; }
void Print() const {
if (score && flags()->print_scariness)
Printf("SCARINESS: %d (%s)\n", score, descr);
}
static void PrintSimple(int score, const char *descr) {
ScarinessScoreBase SSB;
SSB.Clear();
SSB.Scare(score, descr);
SSB.Print();
}

private:
int score;
char descr[1024];
};

struct ScarinessScore : ScarinessScoreBase {
ScarinessScore() {
Clear();
}
};

}

#endif
