#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <cctype>

// used for getting the value signaled by a token,
// i.e. token_int for i32_tok
extern std::string token_string;
extern int32_t token_int;
extern double token_double;
extern char token_char;

// parses the next token from input file
char get_token(std::string fname = "");

#endif