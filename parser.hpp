// Copyright (C) 2020, Hugo Decharnes, Bryan Aggoun. All rights reserved.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <utility>
#include "exception.hpp"
#include "list.hpp"
#include "lexer.hpp"
#include "string.hpp"
#include "token.hpp"
#include "tree.hpp"

class Parser {
public:
  Parser(const String& file_name, Lexer& lexer);
  ~Parser();

private:
  const String& file_name;
  Lexer& lexer;
  Token curr_token;
  unsigned int error_count;

public:
  Statement* parse();

private:
  Statement* compound();
  Statement* block();
  Statement* global_var_def();
  Statement* iteration();
  Statement* selection();
  Statement* inclusion();
  Statement* local_var_def();
  Statement* macro_def();
  Statement* mutate();
  Statement* func_return();
  Statement* expr_stmt();
  Statement* plain_text();

  Expression* ternary();
  Expression* logical_or();
  Expression* logical_and();
  Expression* bitwise_or();
  Expression* bitwise_xor();
  Expression* bitwise_and();
  Expression* equality();
  Expression* relational();
  Expression* shift();
  Expression* additive();
  Expression* multiplicative();
  Expression* exponentiation();
  Expression* quotation();
  Expression* array();
  Expression* dictionary();

  Expression* log2_bif();
  Expression* size_bif();

  Expression* rhs_prefix();
  Expression* rhs_postfix();
  Expression* rhs_primary();
  Expression* rhs_grouping();

  Location* lhs_prefix();
  Location* lhs_postfix();

  Storage* lhs_storage();

  List<Expression*>* macro_call();
  Expression* subscript();

  Token advance();
  Token consume(Token::Type type);
  bool match(Token::Type type);

  void synchronize();
  void report(const Syntactic_error& error);
};

#endif // PARSER_HPP
