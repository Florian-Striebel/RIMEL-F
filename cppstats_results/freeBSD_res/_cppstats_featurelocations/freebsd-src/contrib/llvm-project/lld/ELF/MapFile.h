







#if !defined(LLD_ELF_MAPFILE_H)
#define LLD_ELF_MAPFILE_H

namespace lld {
namespace elf {
void writeMapFile();
void writeCrossReferenceTable();
void writeArchiveStats();
}
}

#endif
