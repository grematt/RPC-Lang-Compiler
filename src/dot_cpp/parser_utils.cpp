#include "../dot_h/parser_utils.h"
#include "../dot_h/tokens.h"
#include "../dot_h/ir_utils.h"

std::map<int, int> operator_precedence;
int cur_token;

int get_next_tok(std::string fname) {
  return cur_token = get_token(fname);
}

int get_precedence() {
  if (cur_token > add_tok)
    return 0;
  return operator_precedence[cur_token];
}

bool is_arithmetic_operator(int token) {
  return token == multi_tok || 
         token == add_tok   || 
         token == sub_tok   || 
         token == div_tok;
}

bool is_boolean_operator(int token) {
  return token == less_tok       || 
         token == greater_tok    || 
         token == less_eq_tok    || 
         token == greater_eq_tok ||
         token == equal_tok      ||
         token == not_tok        ||
         token == and_tok        ||
         token == or_tok;
}

bool is_prim_decl_tok(int tok) {
  return tok == i32_decl_tok ||
         tok == f64_decl_tok ||
         tok == ch8_decl_tok;
}

bool is_arr_decl_tok(int tok) {
  return tok == i32_arr_tok ||
         tok == f64_arr_tok ||
         tok == ch8_arr_tok;
}

int arr_tok_to_from_prim_tok(int tok) {
  switch (tok) {
    case i32_decl_tok:
      return i32_arr_tok;
    case ch8_decl_tok:
      return ch8_arr_tok;
    case f64_decl_tok:
      return f64_arr_tok;
    case i32_arr_tok:
      return i32_decl_tok;
    case ch8_arr_tok:
      return ch8_decl_tok;
    case f64_arr_tok:
      return f64_decl_tok;
    default:
      post_error_msg("Array of unknown type declared");
      std::exit(1);
  }
}

bool arith_or_bool_condition_eval(bool is_arithmetic_expr) {
  if (is_arithmetic_expr)
    return is_arithmetic_operator(cur_token);
  return is_boolean_operator(cur_token);
}