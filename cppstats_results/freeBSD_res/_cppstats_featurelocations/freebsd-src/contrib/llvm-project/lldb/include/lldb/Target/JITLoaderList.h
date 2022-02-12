







#if !defined(LLDB_TARGET_JITLOADERLIST_H)
#define LLDB_TARGET_JITLOADERLIST_H

#include <mutex>
#include <vector>

#include "lldb/lldb-forward.h"

namespace lldb_private {




class JITLoaderList {
public:
JITLoaderList();
~JITLoaderList();

void Append(const lldb::JITLoaderSP &jit_loader_sp);

void Remove(const lldb::JITLoaderSP &jit_loader_sp);

size_t GetSize() const;

lldb::JITLoaderSP GetLoaderAtIndex(size_t idx);

void DidLaunch();

void DidAttach();

void ModulesDidLoad(ModuleList &module_list);

private:
std::vector<lldb::JITLoaderSP> m_jit_loaders_vec;
std::recursive_mutex m_jit_loaders_mutex;
};

}

#endif
