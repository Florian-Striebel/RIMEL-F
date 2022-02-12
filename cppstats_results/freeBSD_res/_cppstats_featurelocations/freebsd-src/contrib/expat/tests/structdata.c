































#if defined(HAVE_EXPAT_CONFIG_H)
#include "expat_config.h"
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "structdata.h"
#include "minicheck.h"

#define STRUCT_EXTENSION_COUNT 8

#if defined(XML_UNICODE_WCHAR_T)
#include <wchar.h>
#define XML_FMT_STR "ls"
#define xcstrlen(s) wcslen(s)
#define xcstrcmp(s, t) wcscmp((s), (t))
#else
#define XML_FMT_STR "s"
#define xcstrlen(s) strlen(s)
#define xcstrcmp(s, t) strcmp((s), (t))
#endif

static XML_Char *
xmlstrdup(const XML_Char *s) {
size_t byte_count = (xcstrlen(s) + 1) * sizeof(XML_Char);
XML_Char *dup = malloc(byte_count);

assert(dup != NULL);
memcpy(dup, s, byte_count);
return dup;
}

void
StructData_Init(StructData *storage) {
assert(storage != NULL);
storage->count = 0;
storage->max_count = 0;
storage->entries = NULL;
}

void
StructData_AddItem(StructData *storage, const XML_Char *s, int data0, int data1,
int data2) {
StructDataEntry *entry;

assert(storage != NULL);
assert(s != NULL);
if (storage->count == storage->max_count) {
StructDataEntry *new;

storage->max_count += STRUCT_EXTENSION_COUNT;
new = realloc(storage->entries,
storage->max_count * sizeof(StructDataEntry));
assert(new != NULL);
storage->entries = new;
}

entry = &storage->entries[storage->count];
entry->str = xmlstrdup(s);
entry->data0 = data0;
entry->data1 = data1;
entry->data2 = data2;
storage->count++;
}




void
StructData_CheckItems(StructData *storage, const StructDataEntry *expected,
int count) {
char buffer[1024];
int i;

assert(storage != NULL);
assert(expected != NULL);
if (count != storage->count) {
sprintf(buffer, "wrong number of entries: got %d, expected %d",
storage->count, count);
StructData_Dispose(storage);
fail(buffer);
} else {
for (i = 0; i < count; i++) {
const StructDataEntry *got = &storage->entries[i];
const StructDataEntry *want = &expected[i];

assert(got != NULL);
assert(want != NULL);

if (xcstrcmp(got->str, want->str) != 0) {
StructData_Dispose(storage);
fail("structure got bad string");
} else {
if (got->data0 != want->data0 || got->data1 != want->data1
|| got->data2 != want->data2) {
sprintf(buffer,
"struct '%" XML_FMT_STR
"' expected (%d,%d,%d), got (%d,%d,%d)",
got->str, want->data0, want->data1, want->data2, got->data0,
got->data1, got->data2);
StructData_Dispose(storage);
fail(buffer);
}
}
}
}
}

void
StructData_Dispose(StructData *storage) {
int i;

assert(storage != NULL);
for (i = 0; i < storage->count; i++)
free((void *)storage->entries[i].str);
free(storage->entries);

storage->count = 0;
storage->entries = NULL;
}
