
























#if !defined(TINYTEST_MACROS_H_INCLUDED_)
#define TINYTEST_MACROS_H_INCLUDED_


#define TT_STMT_BEGIN do {
#define TT_STMT_END } while (0)



#if !defined(TT_EXIT_TEST_FUNCTION)
#define TT_EXIT_TEST_FUNCTION TT_STMT_BEGIN goto end; TT_STMT_END
#endif


#if !defined(TT_DECLARE)
#define TT_DECLARE(prefix, args) TT_STMT_BEGIN printf("\n %s %s:%d: ",prefix,__FILE__,__LINE__); printf args ; TT_STMT_END




#endif


#define TT_GRIPE(args) TT_DECLARE("FAIL", args)


#define TT_BLATHER(args) TT_STMT_BEGIN if (tinytest_get_verbosity_()>1) TT_DECLARE(" OK", args); TT_STMT_END




#define TT_DIE(args) TT_STMT_BEGIN tinytest_set_test_failed_(); TT_GRIPE(args); TT_EXIT_TEST_FUNCTION; TT_STMT_END






#define TT_FAIL(args) TT_STMT_BEGIN tinytest_set_test_failed_(); TT_GRIPE(args); TT_STMT_END






#define tt_abort_printf(msg) TT_DIE(msg)
#define tt_abort_perror(op) TT_DIE(("%s: %s [%d]",(op),strerror(errno), errno))
#define tt_abort_msg(msg) TT_DIE(("%s", msg))
#define tt_abort() TT_DIE(("%s", "(Failed.)"))


#define tt_failprint_f(msg) TT_FAIL(msg)
#define tt_fail_perror(op) TT_FAIL(("%s: %s [%d]",(op),strerror(errno), errno))
#define tt_fail_msg(msg) TT_FAIL(("%s", msg))
#define tt_fail() TT_FAIL(("%s", "(Failed.)"))


#define tt_skip() TT_STMT_BEGIN tinytest_set_test_skipped_(); TT_EXIT_TEST_FUNCTION; TT_STMT_END





#define tt_want_(b, msg, fail) TT_STMT_BEGIN if (!(b)) { tinytest_set_test_failed_(); TT_GRIPE(("%s",msg)); fail; } else { TT_BLATHER(("%s",msg)); } TT_STMT_END











#define tt_want_msg(b, msg) tt_want_(b, msg, );



#define tt_assert_msg(b, msg) tt_want_(b, msg, TT_EXIT_TEST_FUNCTION);



#define tt_want(b) tt_want_msg( (b), "want("#b")")

#define tt_assert(b) tt_assert_msg((b), "assert("#b")")

#define tt_assert_test_fmt_type(a,b,str_test,type,test,printf_type,printf_fmt, setup_block,cleanup_block,die_on_fail) TT_STMT_BEGIN type val1_ = (a); type val2_ = (b); int tt_status_ = (test); if (!tt_status_ || tinytest_get_verbosity_()>1) { printf_type print_; printf_type print1_; printf_type print2_; type value_ = val1_; setup_block; print1_ = print_; value_ = val2_; setup_block; print2_ = print_; TT_DECLARE(tt_status_?" OK":"FAIL", ("assert(%s): "printf_fmt" vs "printf_fmt, str_test, print1_, print2_)); print_ = print1_; cleanup_block; print_ = print2_; cleanup_block; if (!tt_status_) { tinytest_set_test_failed_(); die_on_fail ; } } TT_STMT_END





























#define tt_assert_test_type(a,b,str_test,type,test,fmt,die_on_fail) tt_assert_test_fmt_type(a,b,str_test,type,test,type,fmt, {print_=value_;},{},die_on_fail)



#define tt_assert_test_type_opt(a,b,str_test,type,test,fmt,die_on_fail) tt_assert_test_fmt_type(a,b,str_test,type,test,type,fmt, {print_=value_?value_:"<NULL>";},{},die_on_fail)





#define tt_assert_op_type(a,op,b,type,fmt) tt_assert_test_type(a,b,#a" "#op" "#b,type,(val1_ op val2_),fmt, TT_EXIT_TEST_FUNCTION)



#define tt_int_op(a,op,b) tt_assert_test_type(a,b,#a" "#op" "#b,long,(val1_ op val2_), "%ld",TT_EXIT_TEST_FUNCTION)



#define tt_uint_op(a,op,b) tt_assert_test_type(a,b,#a" "#op" "#b,unsigned long, (val1_ op val2_),"%lu",TT_EXIT_TEST_FUNCTION)



#define tt_ptr_op(a,op,b) tt_assert_test_type(a,b,#a" "#op" "#b,const void*, (val1_ op val2_),"%p",TT_EXIT_TEST_FUNCTION)




#define tt_nstr_op(n,a,op,b) tt_assert_test_type_opt(a,b,#a" "#op" "#b,const char *, (val1_ && val2_ && strncmp(val1_,val2_,(n)) op 0),"<%s>", TT_EXIT_TEST_FUNCTION)




#define tt_str_op(a,op,b) tt_assert_test_type_opt(a,b,#a" "#op" "#b,const char *, (val1_ && val2_ && strcmp(val1_,val2_) op 0),"<%s>", TT_EXIT_TEST_FUNCTION)




#define tt_mem_op(expr1, op, expr2, len) tt_assert_test_fmt_type(expr1,expr2,#expr1" "#op" "#expr2, const void *, (val1_ && val2_ && memcmp(val1_, val2_, len) op 0), char *, "%s", { print_ = tinytest_format_hex_(value_, (len)); }, { if (print_) free(print_); }, TT_EXIT_TEST_FUNCTION );









#define tt_want_int_op(a,op,b) tt_assert_test_type(a,b,#a" "#op" "#b,long,(val1_ op val2_),"%ld",(void)0)


#define tt_want_uint_op(a,op,b) tt_assert_test_type(a,b,#a" "#op" "#b,unsigned long, (val1_ op val2_),"%lu",(void)0)



#define tt_want_ptr_op(a,op,b) tt_assert_test_type(a,b,#a" "#op" "#b,const void*, (val1_ op val2_),"%p",(void)0)



#define tt_want_str_op(a,op,b) tt_assert_test_type(a,b,#a" "#op" "#b,const char *, (strcmp(val1_,val2_) op 0),"<%s>",(void)0)



#endif
