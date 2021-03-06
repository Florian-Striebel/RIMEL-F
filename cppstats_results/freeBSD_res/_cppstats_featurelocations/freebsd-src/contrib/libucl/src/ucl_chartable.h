






















#if !defined(UCL_CHARTABLE_H_)
#define UCL_CHARTABLE_H_

#include "ucl_internal.h"

static const unsigned int ucl_chartable[256] = {
UCL_CHARACTER_JSON_UNSAFE|UCL_CHARACTER_VALUE_END|UCL_CHARACTER_UCL_UNSAFE, UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED,
UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED,
UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED,
UCL_CHARACTER_JSON_UNSAFE|UCL_CHARACTER_UCL_UNSAFE,
UCL_CHARACTER_WHITESPACE|UCL_CHARACTER_WHITESPACE_UNSAFE|UCL_CHARACTER_KEY_SEP|UCL_CHARACTER_JSON_UNSAFE|UCL_CHARACTER_UCL_UNSAFE,
UCL_CHARACTER_WHITESPACE_UNSAFE|UCL_CHARACTER_VALUE_END|UCL_CHARACTER_JSON_UNSAFE|UCL_CHARACTER_UCL_UNSAFE,
UCL_CHARACTER_WHITESPACE_UNSAFE,
UCL_CHARACTER_WHITESPACE_UNSAFE|UCL_CHARACTER_JSON_UNSAFE|UCL_CHARACTER_UCL_UNSAFE,
UCL_CHARACTER_WHITESPACE_UNSAFE|UCL_CHARACTER_VALUE_END|UCL_CHARACTER_JSON_UNSAFE|UCL_CHARACTER_UCL_UNSAFE,
UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED,
UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED,
UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED,
UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED,
UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED,
UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED, UCL_CHARACTER_DENIED,
UCL_CHARACTER_WHITESPACE|UCL_CHARACTER_WHITESPACE_UNSAFE|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_KEY_SEP|UCL_CHARACTER_UCL_UNSAFE ,
UCL_CHARACTER_VALUE_STR ,
UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_ESCAPE|UCL_CHARACTER_JSON_UNSAFE|UCL_CHARACTER_UCL_UNSAFE ,
UCL_CHARACTER_VALUE_END , UCL_CHARACTER_VALUE_STR ,
UCL_CHARACTER_VALUE_STR , UCL_CHARACTER_VALUE_STR ,
UCL_CHARACTER_VALUE_STR , UCL_CHARACTER_VALUE_STR ,
UCL_CHARACTER_VALUE_STR , UCL_CHARACTER_VALUE_STR ,
UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT|UCL_CHARACTER_UCL_UNSAFE ,
UCL_CHARACTER_VALUE_END ,
UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT_START|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_ESCAPE ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT_START|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT_START|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT_START|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT_START|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT_START|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT_START|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT_START|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT_START|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT_START|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT_START|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_KEY_SEP|UCL_CHARACTER_UCL_UNSAFE ,
UCL_CHARACTER_VALUE_END , UCL_CHARACTER_VALUE_STR ,
UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_KEY_SEP|UCL_CHARACTER_UCL_UNSAFE ,
UCL_CHARACTER_VALUE_STR , UCL_CHARACTER_VALUE_STR ,
UCL_CHARACTER_VALUE_STR ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_UCL_UNSAFE ,
UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_ESCAPE|UCL_CHARACTER_JSON_UNSAFE|UCL_CHARACTER_UCL_UNSAFE ,
UCL_CHARACTER_VALUE_END , UCL_CHARACTER_VALUE_STR ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR ,
UCL_CHARACTER_VALUE_STR ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT|UCL_CHARACTER_ESCAPE ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT|UCL_CHARACTER_ESCAPE ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT|UCL_CHARACTER_ESCAPE ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT|UCL_CHARACTER_ESCAPE ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT|UCL_CHARACTER_ESCAPE ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT|UCL_CHARACTER_ESCAPE ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_VALUE_DIGIT ,
UCL_CHARACTER_VALUE_STR|UCL_CHARACTER_UCL_UNSAFE ,
UCL_CHARACTER_VALUE_STR , UCL_CHARACTER_VALUE_END ,
UCL_CHARACTER_VALUE_STR , UCL_CHARACTER_DENIED,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR,
UCL_CHARACTER_KEY_START|UCL_CHARACTER_KEY|UCL_CHARACTER_VALUE_STR
};

static inline bool
ucl_test_character (unsigned char c, int type_flags)
{
return (ucl_chartable[c] & type_flags) != 0;
}

#endif
