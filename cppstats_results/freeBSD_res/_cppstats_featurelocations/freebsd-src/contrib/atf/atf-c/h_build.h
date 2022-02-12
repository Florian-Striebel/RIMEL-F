
























#if defined(ATF_C_H_BUILD_H)
#error "Cannot include h_build.h more than once."
#else
#define ATF_C_H_BUILD_H
#endif





static struct c_o_test {
const char *msg;
const char *cc;
const char *cflags;
const char *cppflags;
const char *sfile;
const char *ofile;
bool hasoptargs;
const char *const optargs[16];
const char *const expargv[16];
} c_o_tests[] = {
{
"No flags",
"cc",
"",
"",
"test.c",
"test.o",
false,
{
NULL
},
{
"cc", "-o", "test.o", "-c", "test.c", NULL
},
},

{
"Multi-word program name",
"cc -foo",
"",
"",
"test.c",
"test.o",
false,
{
NULL
},
{
"cc", "-foo", "-o", "test.o", "-c", "test.c", NULL
},
},

{
"Some cflags",
"cc",
"-f1 -f2 -f3 -f4-f5",
"",
"test.c",
"test.o",
false,
{
NULL
},
{
"cc", "-f1", "-f2", "-f3", "-f4-f5", "-o", "test.o",
"-c", "test.c", NULL
},
},

{
"Some cppflags",
"cc",
"",
"-f1 -f2 -f3 -f4-f5",
"test.c",
"test.o",
false,
{
NULL
},
{
"cc", "-f1", "-f2", "-f3", "-f4-f5", "-o", "test.o",
"-c", "test.c", NULL
},
},

{
"Some cflags and cppflags",
"cc",
"-f2",
"-f1",
"test.c",
"test.o",
false,
{
NULL
},
{
"cc", "-f1", "-f2", "-o", "test.o", "-c", "test.c", NULL
},
},

{
"Some optional arguments",
"cc",
"",
"",
"test.c",
"test.o",
true,
{
"-o1", "-o2", NULL
},
{
"cc", "-o1", "-o2", "-o", "test.o", "-c", "test.c", NULL
},
},

{
"Some cflags, cppflags and optional arguments",
"cc",
"-f2",
"-f1",
"test.c",
"test.o",
true,
{
"-o1", "-o2", NULL
},
{
"cc", "-f1", "-f2", "-o1", "-o2", "-o", "test.o",
"-c", "test.c", NULL
},
},

{
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
false,
{ NULL },
{ NULL },
},
};

static struct cpp_test {
const char *msg;
const char *cpp;
const char *cppflags;
const char *sfile;
const char *ofile;
bool hasoptargs;
const char *const optargs[16];
const char *const expargv[16];
} cpp_tests[] = {
{
"No flags",
"cpp",
"",
"test.c",
"test.out",
false,
{
NULL
},
{
"cpp", "-o", "test.out", "test.c", NULL
},
},

{
"Multi-word program name",
"cpp -foo",
"",
"test.c",
"test.out",
false,
{
NULL
},
{
"cpp", "-foo", "-o", "test.out", "test.c", NULL
},
},

{
"Some cppflags",
"cpp",
"-f1 -f2 -f3 -f4-f5",
"test.c",
"test.out",
false,
{
NULL
},
{
"cpp", "-f1", "-f2", "-f3", "-f4-f5", "-o", "test.out",
"test.c", NULL
},
},

{
"Some optional arguments",
"cpp",
"",
"test.c",
"test.out",
true,
{
"-o1", "-o2", NULL
},
{
"cpp", "-o1", "-o2", "-o", "test.out", "test.c", NULL
},
},

{
"Some cppflags and optional arguments",
"cpp",
"-f1",
"test.c",
"test.out",
true,
{
"-o1", "-o2", NULL
},
{
"cpp", "-f1", "-o1", "-o2", "-o", "test.out", "test.c", NULL
},
},

{
NULL,
NULL,
NULL,
NULL,
NULL,
false,
{ NULL },
{ NULL },
},
};

static struct cxx_o_test {
const char *msg;
const char *cxx;
const char *cxxflags;
const char *cppflags;
const char *sfile;
const char *ofile;
bool hasoptargs;
const char *const optargs[16];
const char *const expargv[16];
} cxx_o_tests[] = {
{
"No flags",
"c++",
"",
"",
"test.c",
"test.o",
false,
{
NULL
},
{
"c++", "-o", "test.o", "-c", "test.c", NULL
},
},

{
"Multi-word program name",
"c++ -foo",
"",
"",
"test.c",
"test.o",
false,
{
NULL
},
{
"c++", "-foo", "-o", "test.o", "-c", "test.c", NULL
},
},

{
"Some cxxflags",
"c++",
"-f1 -f2 -f3 -f4-f5",
"",
"test.c",
"test.o",
false,
{
NULL
},
{
"c++", "-f1", "-f2", "-f3", "-f4-f5", "-o", "test.o",
"-c", "test.c", NULL
},
},

{
"Some cppflags",
"c++",
"",
"-f1 -f2 -f3 -f4-f5",
"test.c",
"test.o",
false,
{
NULL
},
{
"c++", "-f1", "-f2", "-f3", "-f4-f5", "-o", "test.o",
"-c", "test.c", NULL
},
},

{
"Some cxxflags and cppflags",
"c++",
"-f2",
"-f1",
"test.c",
"test.o",
false,
{
NULL
},
{
"c++", "-f1", "-f2", "-o", "test.o", "-c", "test.c", NULL
},
},

{
"Some optional arguments",
"c++",
"",
"",
"test.c",
"test.o",
true,
{
"-o1", "-o2", NULL
},
{
"c++", "-o1", "-o2", "-o", "test.o", "-c", "test.c", NULL
},
},

{
"Some cxxflags, cppflags and optional arguments",
"c++",
"-f2",
"-f1",
"test.c",
"test.o",
true,
{
"-o1", "-o2", NULL
},
{
"c++", "-f1", "-f2", "-o1", "-o2", "-o", "test.o",
"-c", "test.c", NULL
},
},

{
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
false,
{ NULL },
{ NULL },
},
};
