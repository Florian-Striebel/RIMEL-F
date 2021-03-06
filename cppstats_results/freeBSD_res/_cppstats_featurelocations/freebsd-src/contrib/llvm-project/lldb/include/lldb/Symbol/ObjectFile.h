







#if !defined(LLDB_SYMBOL_OBJECTFILE_H)
#define LLDB_SYMBOL_OBJECTFILE_H

#include "lldb/Core/FileSpecList.h"
#include "lldb/Core/ModuleChild.h"
#include "lldb/Core/PluginInterface.h"
#include "lldb/Symbol/Symtab.h"
#include "lldb/Symbol/UnwindTable.h"
#include "lldb/Utility/DataExtractor.h"
#include "lldb/Utility/Endian.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/UUID.h"
#include "lldb/lldb-private.h"
#include "llvm/Support/VersionTuple.h"

namespace lldb_private {

class ObjectFileJITDelegate {
public:
ObjectFileJITDelegate() = default;

virtual ~ObjectFileJITDelegate() = default;

virtual lldb::ByteOrder GetByteOrder() const = 0;

virtual uint32_t GetAddressByteSize() const = 0;

virtual void PopulateSymtab(lldb_private::ObjectFile *obj_file,
lldb_private::Symtab &symtab) = 0;

virtual void PopulateSectionList(lldb_private::ObjectFile *obj_file,
lldb_private::SectionList &section_list) = 0;

virtual ArchSpec GetArchitecture() = 0;
};














class ObjectFile : public std::enable_shared_from_this<ObjectFile>,
public PluginInterface,
public ModuleChild {
friend class lldb_private::Module;

public:
enum Type {
eTypeInvalid = 0,

eTypeCoreFile,

eTypeExecutable,

eTypeDebugInfo,

eTypeDynamicLinker,

eTypeObjectFile,

eTypeSharedLibrary,

eTypeStubLibrary,

eTypeJIT,
eTypeUnknown
};

enum Strata {
eStrataInvalid = 0,
eStrataUnknown,
eStrataUser,
eStrataKernel,
eStrataRawImage,
eStrataJIT
};




enum BinaryType {
eBinaryTypeInvalid = 0,
eBinaryTypeUnknown,
eBinaryTypeKernel,
eBinaryTypeUser,
eBinaryTypeStandalone
};

struct LoadableData {
lldb::addr_t Dest;
llvm::ArrayRef<uint8_t> Contents;
};






ObjectFile(const lldb::ModuleSP &module_sp, const FileSpec *file_spec_ptr,
lldb::offset_t file_offset, lldb::offset_t length,
const lldb::DataBufferSP &data_sp, lldb::offset_t data_offset);

ObjectFile(const lldb::ModuleSP &module_sp, const lldb::ProcessSP &process_sp,
lldb::addr_t header_addr, lldb::DataBufferSP &data_sp);





~ObjectFile() override;









virtual void Dump(Stream *s) = 0;
























static lldb::ObjectFileSP
FindPlugin(const lldb::ModuleSP &module_sp, const FileSpec *file_spec,
lldb::offset_t file_offset, lldb::offset_t file_size,
lldb::DataBufferSP &data_sp, lldb::offset_t &data_offset);
















static lldb::ObjectFileSP FindPlugin(const lldb::ModuleSP &module_sp,
const lldb::ProcessSP &process_sp,
lldb::addr_t header_addr,
lldb::DataBufferSP &file_data_sp);

static size_t
GetModuleSpecifications(const FileSpec &file, lldb::offset_t file_offset,
lldb::offset_t file_size, ModuleSpecList &specs,
lldb::DataBufferSP data_sp = lldb::DataBufferSP());

static size_t GetModuleSpecifications(const lldb_private::FileSpec &file,
lldb::DataBufferSP &data_sp,
lldb::offset_t data_offset,
lldb::offset_t file_offset,
lldb::offset_t file_size,
lldb_private::ModuleSpecList &specs);























static bool SplitArchivePathWithObject(
llvm::StringRef path_with_object, lldb_private::FileSpec &archive_file,
lldb_private::ConstString &archive_object, bool must_exist);


static char ID;
virtual bool isA(const void *ClassID) const { return ClassID == &ID; }







virtual uint32_t GetAddressByteSize() const = 0;











virtual AddressClass GetAddressClass(lldb::addr_t file_addr);

















virtual uint32_t GetDependentModules(FileSpecList &file_list) = 0;






virtual bool IsExecutable() const = 0;









virtual lldb::addr_t GetFileOffset() const { return m_file_offset; }

virtual lldb::addr_t GetByteSize() const { return m_length; }






virtual FileSpec &GetFileSpec() { return m_file; }






virtual const FileSpec &GetFileSpec() const { return m_file; }






virtual ArchSpec GetArchitecture() = 0;









virtual SectionList *GetSectionList(bool update_module_section_list = true);

virtual void CreateSections(SectionList &unified_section_list) = 0;



virtual void SectionFileAddressesChanged() {}









virtual Symtab *GetSymtab() = 0;



virtual void RelocateSection(lldb_private::Section *section);












virtual Symbol *ResolveSymbolForAddress(const Address &so_addr,
bool verify_unique) {







return nullptr;
}







virtual bool IsStripped() = 0;




virtual void ClearSymtab();










virtual UUID GetUUID() = 0;









virtual lldb_private::FileSpecList GetReExportedLibraries() {
return FileSpecList();
}






virtual bool SetLoadAddress(Target &target, lldb::addr_t value,
bool value_is_offset) {
return false;
}







virtual lldb::ByteOrder GetByteOrder() const = 0;












virtual bool ParseHeader() = 0;






















virtual bool AllowAssemblyEmulationUnwindPlans() { return true; }













virtual lldb_private::Address GetImageInfoAddress(Target *target) {
return Address();
}







virtual lldb_private::Address GetEntryPointAddress() { return Address(); }









virtual lldb_private::Address GetBaseAddress() {
return Address(m_memory_addr);
}

virtual uint32_t GetNumThreadContexts() { return 0; }








virtual std::string GetIdentifierString () {
return std::string();
}









virtual lldb::addr_t GetAddressMask() { return 0; }























virtual bool GetCorefileMainBinaryInfo(lldb::addr_t &address, UUID &uuid,
ObjectFile::BinaryType &type) {
address = LLDB_INVALID_ADDRESS;
uuid.Clear();
return false;
}

virtual lldb::RegisterContextSP
GetThreadContextAtIndex(uint32_t idx, lldb_private::Thread &thread) {
return lldb::RegisterContextSP();
}










virtual Type CalculateType() = 0;




virtual void SetType(Type type) { m_type = type; }












virtual Strata CalculateStrata() = 0;












virtual llvm::VersionTuple GetVersion() { return llvm::VersionTuple(); }










virtual llvm::VersionTuple GetMinimumOSVersion() {
return llvm::VersionTuple();
}







virtual llvm::VersionTuple GetSDKVersion() { return llvm::VersionTuple(); }








virtual bool GetIsDynamicLinkEditor() { return false; }


Type GetType() {
if (m_type == eTypeInvalid)
m_type = CalculateType();
return m_type;
}

Strata GetStrata() {
if (m_strata == eStrataInvalid)
m_strata = CalculateStrata();
return m_strata;
}




static lldb::DataBufferSP ReadMemory(const lldb::ProcessSP &process_sp,
lldb::addr_t addr, size_t byte_size);



size_t GetData(lldb::offset_t offset, size_t length,
DataExtractor &data) const;



size_t CopyData(lldb::offset_t offset, size_t length, void *dst) const;



virtual size_t ReadSectionData(Section *section,
lldb::offset_t section_offset, void *dst,
size_t dst_len);




virtual size_t ReadSectionData(Section *section,
DataExtractor &section_data);

bool IsInMemory() const { return m_memory_addr != LLDB_INVALID_ADDRESS; }


virtual llvm::StringRef
StripLinkerSymbolAnnotations(llvm::StringRef symbol_name) const {
return symbol_name;
}

static lldb::SymbolType GetSymbolTypeFromName(
llvm::StringRef name,
lldb::SymbolType symbol_type_hint = lldb::eSymbolTypeUndefined);









virtual std::vector<LoadableData> GetLoadableData(Target &target);


virtual std::unique_ptr<CallFrameInfo> CreateCallFrameInfo();













virtual bool LoadCoreFileImages(lldb_private::Process &process) {
return false;
}

protected:

FileSpec m_file;
Type m_type;
Strata m_strata;
lldb::addr_t m_file_offset;

lldb::addr_t m_length;


DataExtractor
m_data;
lldb::ProcessWP m_process_wp;
const lldb::addr_t m_memory_addr;
std::unique_ptr<lldb_private::SectionList> m_sections_up;
std::unique_ptr<lldb_private::Symtab> m_symtab_up;
uint32_t m_synthetic_symbol_idx;











bool SetModulesArchitecture(const ArchSpec &new_arch);

static lldb::DataBufferSP MapFileData(const FileSpec &file, uint64_t Size,
uint64_t Offset);

private:
ObjectFile(const ObjectFile &) = delete;
const ObjectFile &operator=(const ObjectFile &) = delete;
};

}

namespace llvm {
template <> struct format_provider<lldb_private::ObjectFile::Type> {
static void format(const lldb_private::ObjectFile::Type &type,
raw_ostream &OS, StringRef Style);
};

template <> struct format_provider<lldb_private::ObjectFile::Strata> {
static void format(const lldb_private::ObjectFile::Strata &strata,
raw_ostream &OS, StringRef Style);
};
}

#endif
