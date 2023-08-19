#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include "lexer.h"
#include "Expression.h"
#include <string>

extern std::map<int, int> operator_precedence;
extern int cur_token;

int get_next_tok(std::string fname = "");

// get operator precedence of the current token
int get_precedence();

bool is_arithmetic_operator(int token);
bool is_boolean_operator(int token);

// calls either is_arithmetic_operator or is_boolean_operator
// depending on if the expression is boolean or arithmetic
bool arith_or_bool_condition_eval(bool is_arithmetic_expr);

// returns if a token is used to declare a primative varaible
bool is_prim_decl_tok(int token);

// converts between array tokens and primative type tokens.
// This conversion works both ways
int arr_tok_to_from_prim_tok(int token);

// returns if a token is used to delcare an array
bool is_arr_decl_tok(int token);

#endif