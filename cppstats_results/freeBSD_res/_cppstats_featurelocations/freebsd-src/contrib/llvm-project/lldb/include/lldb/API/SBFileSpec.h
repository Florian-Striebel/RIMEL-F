







#if !defined(LLDB_API_SBFILESPEC_H)
#define LLDB_API_SBFILESPEC_H

#include "lldb/API/SBDefines.h"

namespace lldb {

class LLDB_API SBFileSpec {
public:
SBFileSpec();

SBFileSpec(const lldb::SBFileSpec &rhs);

SBFileSpec(const char *path);


SBFileSpec(const char *path, bool resolve);

~SBFileSpec();

const SBFileSpec &operator=(const lldb::SBFileSpec &rhs);

explicit operator bool() const;

bool operator==(const SBFileSpec &rhs) const;

bool operator!=(const SBFileSpec &rhs) const;

bool IsValid() const;

bool Exists() const;

bool ResolveExecutableLocation();

const char *GetFilename() const;

const char *GetDirectory() const;

void SetFilename(const char *filename);

void SetDirectory(const char *directory);

uint32_t GetPath(char *dst_path, size_t dst_len) const;

static int ResolvePath(const char *src_path, char *dst_path, size_t dst_len);

bool GetDescription(lldb::SBStream &description) const;

void AppendPathComponent(const char *file_or_directory);

private:
friend class SBAttachInfo;
friend class SBBlock;
friend class SBCommandInterpreter;
friend class SBCompileUnit;
friend class SBDeclaration;
friend class SBFileSpecList;
friend class SBHostOS;
friend class SBLaunchInfo;
friend class SBLineEntry;
friend class SBModule;
friend class SBModuleSpec;
friend class SBPlatform;
friend class SBProcess;
friend class SBProcessInfo;
friend class SBSourceManager;
friend class SBTarget;
friend class SBThread;

SBFileSpec(const lldb_private::FileSpec &fspec);

void SetFileSpec(const lldb_private::FileSpec &fspec);

const lldb_private::FileSpec *operator->() const;

const lldb_private::FileSpec *get() const;

const lldb_private::FileSpec &operator*() const;

const lldb_private::FileSpec &ref() const;

std::unique_ptr<lldb_private::FileSpec> m_opaque_up;
};

}

#endif
