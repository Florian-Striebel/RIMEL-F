













































#if !defined(__FLEX_LEXER_H)

#define __FLEX_LEXER_H

#include <iostream>

extern "C++" {

struct yy_buffer_state;
typedef int yy_state_type;

class FlexLexer
{
public:
virtual ~FlexLexer() { }

const char* YYText() const { return yytext; }
int YYLeng() const { return yyleng; }

virtual void
yy_switch_to_buffer( yy_buffer_state* new_buffer ) = 0;
virtual yy_buffer_state* yy_create_buffer( std::istream* s, int size ) = 0;
virtual yy_buffer_state* yy_create_buffer( std::istream& s, int size ) = 0;
virtual void yy_delete_buffer( yy_buffer_state* b ) = 0;
virtual void yyrestart( std::istream* s ) = 0;
virtual void yyrestart( std::istream& s ) = 0;

virtual int yylex() = 0;


int yylex( std::istream& new_in, std::ostream& new_out )
{
switch_streams( new_in, new_out );
return yylex();
}

int yylex( std::istream* new_in, std::ostream* new_out = 0)
{
switch_streams( new_in, new_out );
return yylex();
}



virtual void switch_streams( std::istream* new_in,
std::ostream* new_out ) = 0;
virtual void switch_streams( std::istream& new_in,
std::ostream& new_out ) = 0;

int lineno() const { return yylineno; }

int debug() const { return yy_flex_debug; }
void set_debug( int flag ) { yy_flex_debug = flag; }

protected:
char* yytext;
int yyleng;
int yylineno;
int yy_flex_debug;
};

}
#endif

#if defined(yyFlexLexer) || ! defined(yyFlexLexerOnce)



#define yyFlexLexerOnce

extern "C++" {

class yyFlexLexer : public FlexLexer {
public:


yyFlexLexer( std::istream& arg_yyin, std::ostream& arg_yyout );
yyFlexLexer( std::istream* arg_yyin = 0, std::ostream* arg_yyout = 0 );
private:
void ctor_common();

public:

virtual ~yyFlexLexer();

void yy_switch_to_buffer( yy_buffer_state* new_buffer );
yy_buffer_state* yy_create_buffer( std::istream* s, int size );
yy_buffer_state* yy_create_buffer( std::istream& s, int size );
void yy_delete_buffer( yy_buffer_state* b );
void yyrestart( std::istream* s );
void yyrestart( std::istream& s );

void yypush_buffer_state( yy_buffer_state* new_buffer );
void yypop_buffer_state();

virtual int yylex();
virtual void switch_streams( std::istream& new_in, std::ostream& new_out );
virtual void switch_streams( std::istream* new_in = 0, std::ostream* new_out = 0 );
virtual int yywrap();

protected:
virtual int LexerInput( char* buf, int max_size );
virtual void LexerOutput( const char* buf, int size );
virtual void LexerError( const char* msg );

void yyunput( int c, char* buf_ptr );
int yyinput();

void yy_load_buffer_state();
void yy_init_buffer( yy_buffer_state* b, std::istream& s );
void yy_flush_buffer( yy_buffer_state* b );

int yy_start_stack_ptr;
int yy_start_stack_depth;
int* yy_start_stack;

void yy_push_state( int new_state );
void yy_pop_state();
int yy_top_state();

yy_state_type yy_get_previous_state();
yy_state_type yy_try_NUL_trans( yy_state_type current_state );
int yy_get_next_buffer();

std::istream yyin;
std::ostream yyout;


char yy_hold_char;


int yy_n_chars;


char* yy_c_buf_p;

int yy_init;
int yy_start;



int yy_did_buffer_switch_on_eof;


size_t yy_buffer_stack_top;
size_t yy_buffer_stack_max;
yy_buffer_state ** yy_buffer_stack;
void yyensure_buffer_stack(void);




yy_state_type yy_last_accepting_state;
char* yy_last_accepting_cpos;

yy_state_type* yy_state_buf;
yy_state_type* yy_state_ptr;

char* yy_full_match;
int* yy_full_state;
int yy_full_lp;

int yy_lp;
int yy_looking_for_trail_begin;

int yy_more_flag;
int yy_more_len;
int yy_more_offset;
int yy_prev_more_offset;
};

}

#endif
