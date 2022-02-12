
































#include "flexdef.h"

scanflags_t* _sf_stk = NULL;
size_t _sf_top_ix=0, _sf_max=0;

void
sf_push (void)
{
if (_sf_top_ix + 1 >= _sf_max) {
_sf_max += 32;
_sf_stk = realloc(_sf_stk, sizeof(scanflags_t) * _sf_max);
}


_sf_stk[_sf_top_ix + 1] = _sf_stk[_sf_top_ix];
++_sf_top_ix;
}

void
sf_pop (void)
{
assert(_sf_top_ix > 0);
--_sf_top_ix;
}


void
sf_init (void)
{
assert(_sf_stk == NULL);
_sf_max = 32;
_sf_stk = malloc(sizeof(scanflags_t) * _sf_max);
if (!_sf_stk)
lerr_fatal(_("Unable to allocate %zu of stack"), sizeof(scanflags_t));
_sf_stk[_sf_top_ix] = 0;
}


