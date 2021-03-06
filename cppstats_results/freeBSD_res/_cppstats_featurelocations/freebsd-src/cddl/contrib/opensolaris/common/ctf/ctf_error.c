




























#include <ctf_impl.h>

static const char *const _ctf_errlist[] = {
"File is not in CTF or ELF format",
"File uses more recent ELF version than libctf",
"File uses more recent CTF version than libctf",
"File is a different endian-ness than libctf",
"Symbol table uses invalid entry size",
"Symbol table data buffer is not valid",
"String table data buffer is not valid",
"File data structure corruption detected",
"File does not contain CTF data",
"Buffer does not contain CTF data",
"Symbol table information is not available",
"Type information is in parent and unavailable",
"Cannot import types with different data model",
"Failed to mmap a needed data section",
"Decompression package SUNWzlib not installed",
"Failed to initialize decompression library",
"Failed to allocate decompression buffer",
"Failed to decompress CTF data",
"External string table is not available",
"String name offset is corrupt",
"Invalid type identifier",
"Type is not a struct or union",
"Type is not an enum",
"Type is not a struct, union, or enum",
"Type is not an integer or float",
"Type is not an array",
"Type does not reference another type",
"Input buffer is too small for type name",
"No type information available for that name",
"Syntax error in type name",
"Symbol table entry is not a function",
"No function information available for symbol",
"Symbol table entry is not a data object",
"No type information available for symbol",
"No label information available for that name",
"File does not contain any labels",
"Feature not supported",
"Invalid enum element name",
"Invalid member name",
"CTF container is read-only",
"Limit on number of dynamic type members reached",
"Limit on number of dynamic types reached",
"Duplicate member name definition",
"Conflicting type is already defined",
"Type has outstanding references",
"Type is not a dynamic type"
};

static const int _ctf_nerr = sizeof (_ctf_errlist) / sizeof (_ctf_errlist[0]);

const char *
ctf_errmsg(int error)
{
const char *str;

if (error >= ECTF_BASE && (error - ECTF_BASE) < _ctf_nerr)
str = _ctf_errlist[error - ECTF_BASE];
else
str = ctf_strerror(error);

return (str ? str : "Unknown error");
}

int
ctf_errno(ctf_file_t *fp)
{
return (fp->ctf_errno);
}
