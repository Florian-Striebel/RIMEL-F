#include <limits.h>
#include <stddef.h>
#include "lua.h"
typedef LUAI_UINT32 lu_int32;
typedef LUAI_UMEM lu_mem;
typedef LUAI_MEM l_mem;
typedef unsigned char lu_byte;
#define MAX_SIZET ((size_t)(~(size_t)0)-2)
#define MAX_LUMEM ((lu_mem)(~(lu_mem)0)-2)
#define MAX_INT (INT_MAX-2)
#define IntPoint(p) ((unsigned int)(lu_mem)(p))
typedef LUAI_USER_ALIGNMENT_T L_Umaxalign;
typedef LUAI_UACNUMBER l_uacNumber;
#if defined(lua_assert)
#define check_exp(c,e) (lua_assert(c), (e))
#define api_check(l,e) lua_assert(e)
#else
#define lua_assert(c) ((void)0)
#define check_exp(c,e) (e)
#define api_check luai_apicheck
#endif
#if !defined(UNUSED)
#define UNUSED(x) ((void)(x))
#endif
#if !defined(cast)
#define cast(t, exp) ((t)(exp))
#endif
#define cast_byte(i) cast(lu_byte, (i))
#define cast_num(i) cast(lua_Number, (i))
#define cast_int(i) cast(int, (i))
typedef lu_int32 Instruction;
#define MAXSTACK 250
#if !defined(MINSTRTABSIZE)
#define MINSTRTABSIZE 32
#endif
#if !defined(LUA_MINBUFFER)
#define LUA_MINBUFFER 32
#endif
#if !defined(lua_lock)
#define lua_lock(L) ((void) 0)
#define lua_unlock(L) ((void) 0)
#endif
#if !defined(luai_threadyield)
#define luai_threadyield(L) {lua_unlock(L); lua_lock(L);}
#endif
#if !defined(HARDSTACKTESTS)
#define condhardstacktests(x) ((void)0)
#else
#define condhardstacktests(x) x
#endif
