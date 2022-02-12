







#if !defined(LLDB_SYMBOL_OBJECTCONTAINER_H)
#define LLDB_SYMBOL_OBJECTCONTAINER_H

#include "lldb/Core/ModuleChild.h"
#include "lldb/Core/PluginInterface.h"
#include "lldb/Utility/DataExtractor.h"
#include "lldb/Utility/Endian.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/lldb-private.h"

namespace lldb_private {










class ObjectContainer : public PluginInterface, public ModuleChild {
public:





ObjectContainer(const lldb::ModuleSP &module_sp, const FileSpec *file,
lldb::offset_t file_offset, lldb::offset_t length,
lldb::DataBufferSP &data_sp, lldb::offset_t data_offset)
: ModuleChild(module_sp),
m_file(),
m_offset(file_offset), m_length(length), m_data() {
if (file)
m_file = *file;
if (data_sp)
m_data.SetData(data_sp, data_offset, length);
}





~ObjectContainer() override = default;









virtual void Dump(Stream *s) const = 0;

















virtual bool GetArchitectureAtIndex(uint32_t idx, ArchSpec &arch) const {
return false;
}









virtual lldb::addr_t GetOffset() const { return m_offset; }

virtual lldb::addr_t GetByteSize() const { return m_length; }






virtual size_t GetNumObjects() const { return 0; }










virtual size_t GetNumArchitectures() const { return 0; }












virtual bool ParseHeader() = 0;
















virtual lldb::ObjectFileSP GetObjectFile(const FileSpec *file) = 0;

virtual bool ObjectAtIndexIsContainer(uint32_t object_idx) { return false; }

virtual ObjectFile *GetObjectFileAtIndex(uint32_t object_idx) {
return nullptr;
}

virtual ObjectContainer *GetObjectContainerAtIndex(uint32_t object_idx) {
return nullptr;
}

virtual const char *GetObjectNameAtIndex(uint32_t object_idx) const {
return nullptr;
}

protected:

FileSpec m_file;

lldb::addr_t
m_offset;
lldb::addr_t m_length;
DataExtractor
m_data;

private:
ObjectContainer(const ObjectContainer &) = delete;
const ObjectContainer &operator=(const ObjectContainer &) = delete;
};

}

#endif
