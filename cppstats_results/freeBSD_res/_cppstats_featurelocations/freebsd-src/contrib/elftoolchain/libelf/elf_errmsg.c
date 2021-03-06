

























#include <libelf.h>
#include <stdio.h>
#include <string.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_errmsg.c 3174 2015-03-27 17:13:41Z emaste $");





static const char *_libelf_errors[] = {
#define DEFINE_ERROR(N,S) [ELF_E_##N] = S
DEFINE_ERROR(NONE, "No Error"),
DEFINE_ERROR(ARCHIVE, "Malformed ar(1) archive"),
DEFINE_ERROR(ARGUMENT, "Invalid argument"),
DEFINE_ERROR(CLASS, "ELF class mismatch"),
DEFINE_ERROR(DATA, "Invalid data buffer descriptor"),
DEFINE_ERROR(HEADER, "Missing or malformed ELF header"),
DEFINE_ERROR(IO, "I/O error"),
DEFINE_ERROR(LAYOUT, "Layout constraint violation"),
DEFINE_ERROR(MODE, "Incorrect ELF descriptor mode"),
DEFINE_ERROR(RANGE, "Value out of range of target"),
DEFINE_ERROR(RESOURCE, "Resource exhaustion"),
DEFINE_ERROR(SECTION, "Invalid section descriptor"),
DEFINE_ERROR(SEQUENCE, "API calls out of sequence"),
DEFINE_ERROR(UNIMPL, "Unimplemented feature"),
DEFINE_ERROR(VERSION, "Unknown ELF API version"),
DEFINE_ERROR(NUM, "Unknown error")
#undef DEFINE_ERROR
};

const char *
elf_errmsg(int error)
{
int oserr;

if (error == ELF_E_NONE &&
(error = LIBELF_PRIVATE(error)) == 0)
return NULL;
else if (error == -1)
error = LIBELF_PRIVATE(error);

oserr = error >> LIBELF_OS_ERROR_SHIFT;
error &= LIBELF_ELF_ERROR_MASK;

if (error < ELF_E_NONE || error >= ELF_E_NUM)
return _libelf_errors[ELF_E_NUM];
if (oserr) {
(void) snprintf((char *) LIBELF_PRIVATE(msg),
sizeof(LIBELF_PRIVATE(msg)), "%s: %s",
_libelf_errors[error], strerror(oserr));
return (const char *)&LIBELF_PRIVATE(msg);
}
return _libelf_errors[error];
}
