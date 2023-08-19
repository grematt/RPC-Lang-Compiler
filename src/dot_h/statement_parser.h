#ifndef STATEMENT_PARSER_H
#define STATEMENT_PARSER_H

#include "FuncDeclaration.h"
#include "FuncDefinition.h"

// parse the body of a if/otherwise statement, loop, or function
std::unique_ptr<std::vector<std::unique_ptr<Statement>>> parse_statement_body();
std::unique_ptr<Statement> parse_if_statement();
std::unique_ptr<Statement> parse_variable_declaration(bool = false);
// parse statements that end with a semi colon
std::unique_ptr<Statement> parse_end_with_semi_colon();
std::unique_ptr<Statement> parse_while_loop();
std::unique_ptr<Statement> parse_for_loop();
std::unique_ptr<Statement> parse_statement();
std::unique_ptr<Statement> parse_primative_var_dec(bool, int);
std::unique_ptr<Statement> parse_array_var_dec(bool, int);
std::unique_ptr<Statement> parse_while_loop();
std::unique_ptr<FuncDeclaration> parse_func_declaration();
std::unique_ptr<FuncDefinition> parse_func_definition();

#endif