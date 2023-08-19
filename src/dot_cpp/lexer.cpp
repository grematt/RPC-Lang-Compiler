#include "../dot_h/lexer.h"
#include "../dot_h/tokens.h"
#include "../dot_h/parser_utils.h"
#include "../dot_h/ir_utils.h"
#include <string>
#include <cctype>
#include <fstream>

std::string token_string;
int32_t token_int;
double token_double;
char token_char;

static int cur_char {' '};
static std::ifstream file;

int tokenize_array(int type) {
  if (cur_char != '[')
    return type;
  return arr_tok_to_from_prim_tok(type);
}

bool check_or_parse_newline() {
  if (cur_char == '\\') {
    cur_char = file.get();
    if (cur_char == 'n') {
      cur_char = '\n';
      return false;
    }
    cur_char = '\\';
    return true;
  }
  return false;
}

char get_token(std::string fname) {
  if (!file.is_open())
    file = std::ifstream {fname};

  // ignore whitespace
  while(isspace(cur_char))
    cur_char = file.get();

  // function or variable declarations
  if (isalpha(cur_char)) {
    token_string = cur_char;
    while (cur_char = file.get()) {
      if (isalnum(cur_char) || 
         cur_char == '-'    || 
         cur_char == '_')
        token_string += cur_char;
      else 
        break;
    }

    // keywords
    if (token_string == "func") 
      return func_tok;
    if (token_string == "i32") 
      return tokenize_array(i32_decl_tok);
    if (token_string == "f64") 
      return tokenize_array(f64_decl_tok);
    if (token_string == "ch8")
      return tokenize_array(ch8_decl_tok);
    if (token_string == "def")
      return def_tok;
    if (token_string == "if")
      return if_tok;
    if (token_string == "otherwise")
      return otherwise_tok;
    if (token_string == "return")
      return return_tok;
    if (token_string == "var")
      return var_dec_tok;
    if (token_string == "while")
      return while_tok;
    if (token_string == "for")
      return for_tok;
    if (token_string == "void")
      return void_ret_tok;
    if (token_string == "and")
      return and_tok;
    if (token_string == "or")
      return or_tok;
    if (token_string == "not")
      return not_tok;
    return id_tok;
  }

  // character
  if (cur_char == '\'') {
    cur_char = file.get();
    bool got_next_char {check_or_parse_newline()};
    token_char = cur_char;

    if (!got_next_char)
      cur_char = file.get();
    if (cur_char == '\'') {
      cur_char = file.get();
      return ch8_tok;
    }

    post_error_msg("Invalid single quotes");
    std::exit(1);
  }

  // string/char array initialization
  if (cur_char == '\"') {
    token_string = "";
    cur_char = file.get();
    while (cur_char != '\"') {
      bool got_next_char {check_or_parse_newline()};
      token_string += cur_char;
      if (!got_next_char)
        cur_char = file.get();
    }
    cur_char = file.get();
    return string_tok;    
  }

  int dot_count {};
  int neg_count {};
  if (isdigit(cur_char) || cur_char == '.' || cur_char == '-') {  // int or float
    std::string num_val {};
    
    while ((isdigit(cur_char) || cur_char == '.' || cur_char == '-')) {
      if (cur_char == '.')
        dot_count++;
      else if (cur_char == '-')
        neg_count++;
      num_val += cur_char;
      cur_char = file.get();
    }

    // only 1 decimal should be present
    if (dot_count > 1) {
      post_error_msg("Invalid f64 value");
      std::exit(1);
    }

    if (dot_count == 0) {
      token_int = std::stoi(num_val);
      return i32_tok;
    }
    token_double = std::stod(num_val);
    return f64_tok;
  }

  // skip commented line, then return next token
  if (cur_char == '$') {
    do {
      cur_char = file.get();
    } while (cur_char != '\n' && cur_char != '\r');

    if (cur_char != EOF)
      return get_token();
  }

  // operators
  int ret_char {none};
  if (cur_char == '=')
    ret_char = equal_tok;
  if (cur_char == '+')
    ret_char = add_tok;
  if (cur_char == '-')
    ret_char = sub_tok;
  if (cur_char == '*')
    ret_char = multi_tok;
  if (cur_char == '/')
    ret_char = div_tok;
  if (cur_char == '<') {
    cur_char = file.get();
    if (cur_char == '=')
      ret_char = less_eq_tok;
    else if (cur_char == '-')
      ret_char = assignment_tok;
    else
      return less_tok;
  }
  if (cur_char == '>') {
    cur_char = file.get();
    if (cur_char == '=')
      ret_char = greater_eq_tok;
    else
      return greater_tok;
  }

  if (ret_char != none) {
    cur_char = file.get();
    return ret_char;
  }

  if (cur_char == EOF)
    return eof_tok;

  int temp {cur_char};
  cur_char = file.get();
  return temp;
}