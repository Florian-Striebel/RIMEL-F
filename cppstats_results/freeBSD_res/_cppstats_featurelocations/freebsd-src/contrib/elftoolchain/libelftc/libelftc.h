




























#if !defined(_LIBELFTC_H_)
#define _LIBELFTC_H_

#include <sys/stat.h>

#include <libelf.h>




typedef struct _Elftc_Bfd_Target Elftc_Bfd_Target;
typedef struct _Elftc_String_Table Elftc_String_Table;


typedef enum {
ETF_NONE,
ETF_ELF,
ETF_BINARY,
ETF_SREC,
ETF_IHEX,
ETF_PE,
ETF_EFI,
} Elftc_Bfd_Target_Flavor;






#define ELFTC_DEM_UNKNOWN 0x00000000U
#define ELFTC_DEM_ARM 0x00000001U
#define ELFTC_DEM_GNU2 0x00000002U
#define ELFTC_DEM_GNU3 0x00000004U


#define ELFTC_DEM_NOPARAM 0x00010000U

#if defined(__cplusplus)
extern "C" {
#endif
Elftc_Bfd_Target *elftc_bfd_find_target(const char *_tgt_name);
Elftc_Bfd_Target_Flavor elftc_bfd_target_flavor(Elftc_Bfd_Target *_tgt);
unsigned int elftc_bfd_target_byteorder(Elftc_Bfd_Target *_tgt);
unsigned int elftc_bfd_target_class(Elftc_Bfd_Target *_tgt);
unsigned int elftc_bfd_target_machine(Elftc_Bfd_Target *_tgt);
unsigned int elftc_bfd_target_osabi(Elftc_Bfd_Target *_tgt);
int elftc_copyfile(int _srcfd, int _dstfd);
int elftc_demangle(const char *_mangledname, char *_buffer,
size_t _bufsize, unsigned int _flags);
const char *elftc_reloc_type_str(unsigned int mach, unsigned int type);
int elftc_set_timestamps(const char *_filename, struct stat *_sb);
Elftc_String_Table *elftc_string_table_create(size_t _sizehint);
void elftc_string_table_destroy(Elftc_String_Table *_table);
Elftc_String_Table *elftc_string_table_from_section(Elf_Scn *_scn,
size_t _sizehint);
const char *elftc_string_table_image(Elftc_String_Table *_table,
size_t *_sz);
size_t elftc_string_table_insert(Elftc_String_Table *_table,
const char *_string);
size_t elftc_string_table_lookup(Elftc_String_Table *_table,
const char *_string);
int elftc_string_table_remove(Elftc_String_Table *_table,
const char *_string);
const char *elftc_string_table_to_string(Elftc_String_Table *_table,
size_t offset);
int elftc_timestamp(time_t *_timestamp);
const char *elftc_version(void);
#if defined(__cplusplus)
}
#endif

#endif
