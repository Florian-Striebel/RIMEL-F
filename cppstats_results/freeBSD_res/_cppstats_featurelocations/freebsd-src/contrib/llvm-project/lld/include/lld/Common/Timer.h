







#if !defined(LLD_COMMON_TIMER_H)
#define LLD_COMMON_TIMER_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringRef.h"
#include <assert.h>
#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <vector>

namespace lld {

class Timer;

struct ScopedTimer {
explicit ScopedTimer(Timer &t);

~ScopedTimer();

void stop();

std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

Timer *t = nullptr;
};

class Timer {
public:
Timer(llvm::StringRef name, Timer &parent);

static Timer &root();

void addToTotal(std::chrono::nanoseconds time) { total += time.count(); }
void print();

double millis() const;

private:
explicit Timer(llvm::StringRef name);
void print(int depth, double totalDuration, bool recurse = true) const;

std::atomic<std::chrono::nanoseconds::rep> total;
std::vector<Timer *> children;
std::string name;
};

}

#endif
