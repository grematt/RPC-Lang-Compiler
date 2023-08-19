#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include "Expression.h"

std::unique_ptr<Expression> parse_not_expr();
std::unique_ptr<Expression> parse_constant_array();
std::unique_ptr<Expression> parse_literal();
std::unique_ptr<Expression> parse_paren_expr();
std::unique_ptr<Expression> parse_return_expr();
std::unique_ptr<Expression> parse_id_expr();
std::unique_ptr<Expression> parse_primary_expr();
// parse 0 or more values to the right of the first operator in a binary expr
std::unique_ptr<Expression> parse_bin_expr_rhs(std::unique_ptr<Expression> lhs, 
  int prev_op_prec, bool is_arith_expr);
std::unique_ptr<Expression> parse_bin_expr();
std::unique_ptr<Expression> parse_expression();

#endif