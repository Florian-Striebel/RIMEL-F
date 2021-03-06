







#if !defined(LLDB_CORE_SECTION_H)
#define LLDB_CORE_SECTION_H

#include "lldb/Core/ModuleChild.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/Flags.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-types.h"

#include <memory>
#include <vector>

#include <cstddef>
#include <cstdint>

namespace lldb_private {
class Address;
class DataExtractor;
class ObjectFile;
class Section;
class Target;

class SectionList {
public:
typedef std::vector<lldb::SectionSP> collection;
typedef collection::iterator iterator;
typedef collection::const_iterator const_iterator;

const_iterator begin() const { return m_sections.begin(); }
const_iterator end() const { return m_sections.end(); }
const_iterator begin() { return m_sections.begin(); }
const_iterator end() { return m_sections.end(); }


SectionList() = default;

SectionList &operator=(const SectionList &rhs);

size_t AddSection(const lldb::SectionSP &section_sp);

size_t AddUniqueSection(const lldb::SectionSP &section_sp);

size_t FindSectionIndex(const Section *sect);

bool ContainsSection(lldb::user_id_t sect_id) const;

void Dump(llvm::raw_ostream &s, unsigned indent, Target *target,
bool show_header, uint32_t depth) const;

lldb::SectionSP FindSectionByName(ConstString section_dstr) const;

lldb::SectionSP FindSectionByID(lldb::user_id_t sect_id) const;

lldb::SectionSP FindSectionByType(lldb::SectionType sect_type,
bool check_children,
size_t start_idx = 0) const;

lldb::SectionSP
FindSectionContainingFileAddress(lldb::addr_t addr,
uint32_t depth = UINT32_MAX) const;


size_t GetSize() const { return m_sections.size(); }


size_t GetNumSections(uint32_t depth) const;

bool ReplaceSection(lldb::user_id_t sect_id,
const lldb::SectionSP &section_sp,
uint32_t depth = UINT32_MAX);


bool DeleteSection(size_t idx);

lldb::SectionSP GetSectionAtIndex(size_t idx) const;

size_t Slide(lldb::addr_t slide_amount, bool slide_children);

void Clear() { m_sections.clear(); }

protected:
collection m_sections;
};

class Section : public std::enable_shared_from_this<Section>,
public ModuleChild,
public UserID,
public Flags {
public:

Section(const lldb::ModuleSP &module_sp, ObjectFile *obj_file,
lldb::user_id_t sect_id, ConstString name,
lldb::SectionType sect_type, lldb::addr_t file_vm_addr,
lldb::addr_t vm_size, lldb::offset_t file_offset,
lldb::offset_t file_size, uint32_t log2align, uint32_t flags,
uint32_t target_byte_size = 1);


Section(const lldb::SectionSP &parent_section_sp,


const lldb::ModuleSP &module_sp, ObjectFile *obj_file,
lldb::user_id_t sect_id, ConstString name,
lldb::SectionType sect_type, lldb::addr_t file_vm_addr,
lldb::addr_t vm_size, lldb::offset_t file_offset,
lldb::offset_t file_size, uint32_t log2align, uint32_t flags,
uint32_t target_byte_size = 1);

~Section();

static int Compare(const Section &a, const Section &b);

bool ContainsFileAddress(lldb::addr_t vm_addr) const;

SectionList &GetChildren() { return m_children; }

const SectionList &GetChildren() const { return m_children; }

void Dump(llvm::raw_ostream &s, unsigned indent, Target *target,
uint32_t depth) const;

void DumpName(llvm::raw_ostream &s) const;

lldb::addr_t GetLoadBaseAddress(Target *target) const;

bool ResolveContainedAddress(lldb::addr_t offset, Address &so_addr,
bool allow_section_end = false) const;

lldb::offset_t GetFileOffset() const { return m_file_offset; }

void SetFileOffset(lldb::offset_t file_offset) {
m_file_offset = file_offset;
}

lldb::offset_t GetFileSize() const { return m_file_size; }

void SetFileSize(lldb::offset_t file_size) { m_file_size = file_size; }

lldb::addr_t GetFileAddress() const;

bool SetFileAddress(lldb::addr_t file_addr);

lldb::addr_t GetOffset() const;

lldb::addr_t GetByteSize() const { return m_byte_size; }

void SetByteSize(lldb::addr_t byte_size) { m_byte_size = byte_size; }

bool IsFake() const { return m_fake; }

void SetIsFake(bool fake) { m_fake = fake; }

bool IsEncrypted() const { return m_encrypted; }

void SetIsEncrypted(bool b) { m_encrypted = b; }

bool IsDescendant(const Section *section);

ConstString GetName() const { return m_name; }

bool Slide(lldb::addr_t slide_amount, bool slide_children);

lldb::SectionType GetType() const { return m_type; }

const char *GetTypeAsCString() const;

lldb::SectionSP GetParent() const { return m_parent_wp.lock(); }

bool IsThreadSpecific() const { return m_thread_specific; }

void SetIsThreadSpecific(bool b) { m_thread_specific = b; }


uint32_t GetPermissions() const;


void SetPermissions(uint32_t permissions);

ObjectFile *GetObjectFile() { return m_obj_file; }
const ObjectFile *GetObjectFile() const { return m_obj_file; }


















lldb::offset_t GetSectionData(void *dst, lldb::offset_t dst_len,
lldb::offset_t offset = 0);














lldb::offset_t GetSectionData(DataExtractor &data);

uint32_t GetLog2Align() { return m_log2align; }

void SetLog2Align(uint32_t align) { m_log2align = align; }


uint32_t GetTargetByteSize() const { return m_target_byte_size; }

bool IsRelocated() const { return m_relocated; }

void SetIsRelocated(bool b) { m_relocated = b; }

protected:
ObjectFile *m_obj_file;

lldb::SectionType m_type;
lldb::SectionWP m_parent_wp;
ConstString m_name;
lldb::addr_t m_file_addr;


lldb::addr_t m_byte_size;

lldb::offset_t m_file_offset;
lldb::offset_t m_file_size;

uint32_t m_log2align;

SectionList m_children;
bool m_fake : 1,




m_encrypted : 1,
m_thread_specific : 1,
m_readable : 1,
m_writable : 1,
m_executable : 1,
m_relocated : 1;
uint32_t m_target_byte_size;


private:
Section(const Section &) = delete;
const Section &operator=(const Section &) = delete;
};

}

#endif
