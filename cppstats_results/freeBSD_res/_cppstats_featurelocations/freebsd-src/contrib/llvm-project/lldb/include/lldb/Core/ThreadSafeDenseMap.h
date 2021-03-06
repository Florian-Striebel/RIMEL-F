








#if !defined(LLDB_CORE_THREADSAFEDENSEMAP_H)
#define LLDB_CORE_THREADSAFEDENSEMAP_H

#include <mutex>

#include "llvm/ADT/DenseMap.h"


namespace lldb_private {

template <typename _KeyType, typename _ValueType,
typename _MutexType = std::mutex>
class ThreadSafeDenseMap {
public:
typedef llvm::DenseMap<_KeyType, _ValueType> LLVMMapType;

ThreadSafeDenseMap(unsigned map_initial_capacity = 0)
: m_map(map_initial_capacity), m_mutex() {}

void Insert(_KeyType k, _ValueType v) {
std::lock_guard<_MutexType> guard(m_mutex);
m_map.insert(std::make_pair(k, v));
}

void Erase(_KeyType k) {
std::lock_guard<_MutexType> guard(m_mutex);
m_map.erase(k);
}

_ValueType Lookup(_KeyType k) {
std::lock_guard<_MutexType> guard(m_mutex);
return m_map.lookup(k);
}

bool Lookup(_KeyType k, _ValueType &v) {
std::lock_guard<_MutexType> guard(m_mutex);
auto iter = m_map.find(k), end = m_map.end();
if (iter == end)
return false;
v = iter->second;
return true;
}

void Clear() {
std::lock_guard<_MutexType> guard(m_mutex);
m_map.clear();
}

protected:
LLVMMapType m_map;
_MutexType m_mutex;
};

}

#endif
