// Copyright (C) 2020-2021, Hugo Decharnes. All rights reserved.
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

#include "parser.hpp"

///////////////////////////////////////////////////////////// PUBLICS //////////////////////////////////////////////////////////////

Parser::Parser(Path& file_path, Lexer& lexer)
  : file_path(file_path), lexer(lexer), error_count(0)
{
}

Parser::~Parser()
{
}

Statement* Parser::parse()
{
  curr_token = lexer.get_token();
  Statement* statement = compound();
  try {
    consume(Token::Type::END_OF_FILE);
  }
  catch (const Preproc_error& error) {
    report(error);
  }
  if (error_count == 0) {
    return statement;
  }
  else {
    delete statement;
    if (error_count >= 5) {
      String message = file_path.string() + ": " + std::to_string(error_count - 5) + " more error(s)\n";
      std::cerr << message.data();
    }
    String message = file_path.string() + ": compilation failed due to " + std::to_string(error_count) + " error(s)";
    throw Runtime_error(message);
  }
}

//////////////////////////////////////////////////////////// STATEMENTS ////////////////////////////////////////////////////////////

Statement* Parser::compound()
{
  List<Statement*>* stmt_list = new List<Statement*>();
  for (;;) {
    if (match(Token::Type::BACKTICK)) {
      switch (curr_token.type) {
      case Token::Type::NEWLINE:
        advance();
        continue;
      case Token::Type::DEFINE: {
        Statement* statement = global_var_def();
        stmt_list->push_back(statement);
        continue;
      }
      case Token::Type::FOR: {
        Statement* statement = iteration();
        stmt_list->push_back(statement);
        continue;
      }
      case Token::Type::IF: {
        Statement* statement = selection();
        stmt_list->push_back(statement);
        continue;
      }
      case Token::Type::INCLUDE: {
        Statement* statement = inclusion();
        stmt_list->push_back(statement);
        continue;
      }
      case Token::Type::LET: {
        Statement* statement = local_var_def();
        stmt_list->push_back(statement);
        continue;
      }
      case Token::Type::MACRO: {
        Statement* statement = macro_def();
        stmt_list->push_back(statement);
        continue;
      }
      case Token::Type::ELSE:
      case Token::Type::ELSEIF:
      case Token::Type::ENDFOR:
      case Token::Type::ENDIF:
      case Token::Type::ENDMACRO:
        if (stmt_list->size() == 1) {
          Statement* statement = stmt_list->front();
          delete stmt_list;
          return statement;
        }
        else {
          return new Compound(stmt_list);
        }
      default:
        Statement* statement = expr_stmt();
        stmt_list->push_back(statement);
        continue;
      }
    }
    else {
      if (curr_token.type == Token::Type::PLAIN_TEXT) {
        Statement* statement = plain_text();
        stmt_list->push_back(statement);
        continue;
      }
      else {
        if (stmt_list->size() == 1) {
          Statement* statement = stmt_list->front();
          delete stmt_list;
          return statement;
        }
        else {
          return new Compound(stmt_list);
        }
      }
    }
  }
}

Statement* Parser::plain_text()
{
  Token token = advance();
  return new Plain_text(token);
}

Statement* Parser::expr_stmt()
{
  Token token = curr_token;
  Expression* expression = nullptr;
  try {
    expression = ternary();
  }
  catch (const Preproc_error& error) {
    report(error);
    synchronize();
  }
  return new Expr_stmt(token, expression);
}

Statement* Parser::local_var_def()
{
  Token token = advance();
  Storage* storage = nullptr;
  Expression* expression = nullptr;
  try {
    storage = lhs_storage();
    consume(Token::Type::EQUAL);
    expression = ternary();
    consume(Token::Type::NEWLINE);
  }
  catch (const Preproc_error& error) {
    report(error);
    synchronize();
  }
  return new Local_var_def(token, storage, expression);
}

Statement* Parser::global_var_def()
{
  Token token = advance();
  Storage* storage = nullptr;
  Expression* expression = nullptr;
  try {
    storage = lhs_storage();
    consume(Token::Type::EQUAL);
    expression = ternary();
    consume(Token::Type::NEWLINE);
  }
  catch (const Preproc_error& error) {
    report(error);
    synchronize();
  }
  return new Global_var_def(token, storage, expression);
}

Statement* Parser::macro_def()
{
  Token token = advance();
  Storage* storage = nullptr;
  Statement* statement = nullptr;
  List<Identifier*>* parameters = new List<Identifier*>();
  try {
    storage = lhs_storage();
    consume(Token::Type::LEFT_PAREN);
    if (!match(Token::Type::RIGHT_PAREN)) {
      do {
        Token token = consume(Token::Type::IDENTIFIER);
        Identifier* parameter = new Identifier(token);
        parameters->push_back(parameter);
      } while (match(Token::Type::COMMA));
      consume(Token::Type::RIGHT_PAREN);
    }
    consume(Token::Type::NEWLINE);
  }
  catch (const Preproc_error& error) {
    report(error);
    synchronize();
  }
  statement = compound();
  try {
    consume(Token::Type::ENDMACRO);
    consume(Token::Type::NEWLINE);
  }
  catch (const Preproc_error& error) {
    report(error);
    synchronize();
  }
  Macro* macro = new Macro(file_path, parameters, statement);
  return new Macro_def(token, storage, macro);
}

Statement* Parser::selection()
{
  Token token = advance();
  List<Pair<Expression*, Statement*>>* alternatives = new List<Pair<Expression*, Statement*>>();
  {
    Expression* expression = nullptr;
    try {
      consume(Token::Type::LEFT_PAREN);
      expression = ternary();
      consume(Token::Type::RIGHT_PAREN);
      consume(Token::Type::NEWLINE);
    }
    catch (const Preproc_error& error) {
      report(error);
      synchronize();
    }
    Statement* statement = compound();
    alternatives->push_back(Pair<Expression*, Statement*>(expression, statement));
  }
  while (curr_token.type == Token::Type::ELSEIF) {
    advance();
    Expression* expression = nullptr;
    try {
      consume(Token::Type::LEFT_PAREN);
      expression = ternary();
      consume(Token::Type::RIGHT_PAREN);
      consume(Token::Type::NEWLINE);
    }
    catch (const Preproc_error& error) {
      report(error);
      synchronize();
    }
    Statement* statement = compound();
    alternatives->push_back(Pair<Expression*, Statement*>(expression, statement));
  }
  if (curr_token.type == Token::Type::ELSE) {
    Token token = advance();
    try {
      consume(Token::Type::NEWLINE);
    }
    catch (const Preproc_error& error) {
      report(error);
      synchronize();
    }
    Expression* expression = new True_const(token);
    Statement* statement = compound();
    alternatives->push_back(Pair<Expression*, Statement*>(expression, statement));
  }
  try {
    consume(Token::Type::ENDIF);
    consume(Token::Type::NEWLINE);
  }
  catch (const Preproc_error& error) {
    report(error);
    synchronize();
  }
  return new Selection(token, alternatives);
}

Statement* Parser::iteration()
{
  Token token = advance();
  Storage* storage = nullptr;
  Expression* expression = nullptr;
  try {
    consume(Token::Type::LEFT_PAREN);
    storage = lhs_storage();
    consume(Token::Type::COLON);
    expression = ternary();
    consume(Token::Type::RIGHT_PAREN);
    consume(Token::Type::NEWLINE);
  }
  catch (const Preproc_error& error) {
    report(error);
    synchronize();
  }
  Statement* statement = compound();
  try {
    consume(Token::Type::ENDFOR);
    consume(Token::Type::NEWLINE);
  }
  catch (const Preproc_error& error) {
    report(error);
    synchronize();
  }
  return new Iteration(token, storage, expression, statement);
}

Statement* Parser::inclusion()
{
  Token token = advance();
  Expression* expression = nullptr;
  try {
    expression = ternary();
    consume(Token::Type::NEWLINE);
  }
  catch (const Preproc_error& error) {
    report(error);
    synchronize();
  }
  return new Inclusion(token, expression);
}

/////////////////////////////////////////////////// RIGHT-HAND SIDE EXPRESSIONS ////////////////////////////////////////////////////

Expression* Parser::ternary()
{
  Expression* expression = nullptr;
  Expression* true_branch = nullptr;
  try {
    expression = logical_or();
    if (curr_token.type == Token::Type::QUESTION) {
      Token token = advance();
      true_branch = ternary();
      consume(Token::Type::COLON);
      Expression* false_branch = ternary();
      expression = new Ternary(token, expression, true_branch, false_branch);
    }
    return expression;
  }
  catch (const Preproc_error& error) {
    delete expression;
    delete true_branch;
    throw error;
  }
}

Expression* Parser::logical_or()
{
  Expression* expression = nullptr;
  try {
    expression = logical_and();
    while (curr_token.type == Token::Type::PIPE_PIPE) {
      Token token = advance();
      Expression* right_expr = logical_and();
      expression = new Logical_or(token, expression, right_expr);
    }
    return expression;
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::logical_and()
{
  Expression* expression = nullptr;
  try {
    expression = bitwise_or();
    while (curr_token.type == Token::Type::AMPERS_AMPERS) {
      Token token = advance();
      Expression* right_expr = bitwise_or();
      expression = new Logical_and(token, expression, right_expr);
    }
    return expression;
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::bitwise_or()
{
  Expression* expression = nullptr;
  try {
    expression = bitwise_xor();
    while (curr_token.type == Token::Type::PIPE) {
      Token token = advance();
      Expression* right_expr = bitwise_xor();
      expression = new Bitwise_or(token, expression, right_expr);
    }
    return expression;
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::bitwise_xor()
{
  Expression* expression = nullptr;
  try {
    expression = bitwise_and();
    while (curr_token.type == Token::Type::CARET) {
      Token token = advance();
      Expression* right_expr = bitwise_and();
      expression = new Bitwise_xor(token, expression, right_expr);
    }
    return expression;
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::bitwise_and()
{
  Expression* expression = nullptr;
  try {
    expression = equality();
    while (curr_token.type == Token::Type::AMPERS) {
      Token token = advance();
      Expression* right_expr = equality();
      expression = new Bitwise_and(token, expression, right_expr);
    }
    return expression;
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::equality()
{
  Expression* expression = nullptr;
  try {
    expression = relational();
    for (;;) {
      switch (curr_token.type) {
      case Token::Type::EQUAL_EQUAL: {
        Token token = advance();
        Expression* right_expr = relational();
        expression = new Equal(token, expression, right_expr);
        continue;
      }
      case Token::Type::BANG_EQUAL: {
        Token token = advance();
        Expression* right_expr = relational();
        expression = new Not_equal(token, expression, right_expr);
        continue;
      }
      default:
        return expression;
      }
    }
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::relational()
{
  Expression* expression = nullptr;
  try {
    expression = shift();
    switch (curr_token.type) {
    case Token::Type::GREATER: {
      Token token = advance();
      Expression* right_expr = shift();
      return new Strict_super(token, expression, right_expr);
    }
    case Token::Type::GREATER_EQUAL: {
      Token token = advance();
      Expression* right_expr = shift();
      return new Loose_super(token, expression, right_expr);
    }
    case Token::Type::LESS: {
      Token token = advance();
      Expression* right_expr = shift();
      return new Strict_infer(token, expression, right_expr);
    }
    case Token::Type::LESS_EQUAL: {
      Token token = advance();
      Expression* right_expr = shift();
      return new Loose_infer(token, expression, right_expr);
    }
    case Token::Type::INSIDE: {
      Token token = advance();
      Expression* right_expr = shift();
      return new Inside(token, expression, right_expr);
    }
    default:
      return expression;
    }
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::shift()
{
  Expression* expression = nullptr;
  try {
    expression = additive();
    for (;;) {
      switch (curr_token.type) {
      case Token::Type::LESS_LESS: {
        Token token = advance();
        Expression* right_expr = additive();
        expression = new Left_shift(token, expression, right_expr);
        continue;
      }
      case Token::Type::GREATER_GREATER: {
        Token token = advance();
        Expression* right_expr = additive();
        expression = new Right_shift(token, expression, right_expr);
        continue;
      }
      default:
        return expression;
      }
    }
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::additive()
{
  Expression* expression = nullptr;
  try {
    expression = multiplicative();
    for (;;) {
      switch (curr_token.type) {
      case Token::Type::PLUS: {
        Token token = advance();
        Expression* right_expr = multiplicative();
        expression = new Addition(token, expression, right_expr);
        continue;
      }
      case Token::Type::MINUS: {
        Token token = advance();
        Expression* right_expr = multiplicative();
        expression = new Subtraction(token, expression, right_expr);
        continue;
      }
      default:
        return expression;
      }
    }
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::multiplicative()
{
  Expression* expression = nullptr;
  try {
    expression = exponentiation();
    for (;;) {
      switch (curr_token.type) {
      case Token::Type::STAR: {
        Token token = advance();
        Expression* right_expr = exponentiation();
        expression = new Multiplication(token, expression, right_expr);
        continue;
      }
      case Token::Type::SLASH: {
        Token token = advance();
        Expression* right_expr = exponentiation();
        expression = new Division(token, expression, right_expr);
        continue;
      }
      case Token::Type::PERCENT: {
        Token token = advance();
        Expression* right_expr = exponentiation();
        expression = new Modulo(token, expression, right_expr);
        continue;
      }
      default:
        return expression;
      }
    }
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::exponentiation()
{
  Expression* expression = nullptr;
  try {
    expression = rhs_prefix();
    if (curr_token.type == Token::Type::STAR_STAR) {
      Token token = advance();
      Expression* right_expr = exponentiation();
      expression = new Exponentiation(token, expression, right_expr);
    }
    return expression;
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::rhs_prefix()
{
  switch (curr_token.type) {
  case Token::Type::BANG: {
    Token token = advance();
    Expression* expression = rhs_prefix();
    return new Logical_not(token, expression);
  }
  case Token::Type::DOLLAR: {
    Token token = advance();
    Expression* expression = rhs_prefix();
    return new Interpolate(token, expression);
  }
  case Token::Type::PLUS: {
    Token token = advance();
    Expression* expression = rhs_prefix();
    return new Unary_plus(token, expression);
  }
  case Token::Type::MINUS: {
    Token token = advance();
    Expression* expression = rhs_prefix();
    return new Unary_minus(token, expression);
  }
  case Token::Type::TILDE: {
    Token token = advance();
    Expression* expression = rhs_prefix();
    return new Bitwise_not(token, expression);
  }
  case Token::Type::AT_SIGN: {
    Token token = advance();
    Expression* expression = rhs_prefix();
    return new Indirection(token, expression);
  }
  default:
    return rhs_postfix();
  }
}

Expression* Parser::rhs_postfix()
{
  Expression* expression = nullptr;
  try {
    expression = rhs_primary();
    for (;;) {
      switch (curr_token.type) {
      case Token::Type::LEFT_PAREN: {
        Token token = advance();
        List<Expression*>* expr_list = macro_call();
        expression = new Macro_call(token, expression, expr_list);
        continue;
      }
      case Token::Type::LEFT_BRACK: {
        Token token = advance();
        Expression* right_expr = subscript();
        expression = new Subscript(token, expression, right_expr);
        continue;
      }
      default:
        return expression;
      }
    }
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

// This macro parses the argument list after the opening parenthese, and returns that expression list enclosed in between.
List<Expression*>* Parser::macro_call()
{
  List<Expression*>* expr_list = new List<Expression*>();
  try {
    if (!match(Token::Type::RIGHT_PAREN)) {
      do {
        Expression* expression = ternary();
        expr_list->push_back(expression);
      } while (match(Token::Type::COMMA));
      consume(Token::Type::RIGHT_PAREN);
    }
    return expr_list;
  }
  catch (const Preproc_error& error) {
    for (Expression*& expression : *expr_list) {
      delete expression;
    }
    delete expr_list;
    throw error;
  }
}

Expression* Parser::rhs_primary()
{
  switch (curr_token.type) {
  case Token::Type::DOUBLE_QUOTE:
    return quotation();
  case Token::Type::LEFT_PAREN:
    return rhs_grouping();
  case Token::Type::LEFT_BRACK:
    return array();
  case Token::Type::LEFT_CURLY:
    return dictionary();
  case Token::Type::LOG2:
    return log2_bif();
  case Token::Type::CLOG2:
    return clog2_bif();
  case Token::Type::MAX:
    return max_bif();
  case Token::Type::MIN:
    return min_bif();
  case Token::Type::SIZE:
    return size_bif();
  case Token::Type::INTEGER: {
    Token token = advance();
    return new Integer(token);
  }
  case Token::Type::IDENTIFIER: {
    Token token = advance();
    return new Identifier(token);
  }
  case Token::Type::TRUE: {
    Token token = advance();
    return new True_const(token);
  }
  case Token::Type::FALSE: {
    Token token = advance();
    return new False_const(token);
  }
  default:
    String message = "expecting \"(\", \"[\", literal or identifier; found " + to_string(curr_token.type);
    throw Syntactic_error(curr_token, message);
  }
}

Expression* Parser::rhs_grouping()
{
  Expression* expression = nullptr;
  try {
    consume(Token::Type::LEFT_PAREN);
    expression = ternary();
    consume(Token::Type::RIGHT_PAREN);
    return expression;
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::quotation()
{
  Token token = advance();
  List<Expression*>* expr_list = new List<Expression*>();
  try {
    for (;;) {
      switch (curr_token.type) {
      case Token::Type::ESCAPE_SEQ: {
        Token token = advance();
        Expression* expression = new Escape_seq(token);
        expr_list->push_back(expression);
        continue;
      }
      case Token::Type::PLAIN_TEXT: {
        Token token = advance();
        Expression* expression = new String_literal(token);
        expr_list->push_back(expression);
        continue;
      }
      case Token::Type::DOUBLE_QUOTE: {
        advance();
        return new Quotation(token, expr_list);
      }
      default:
        String message = "expecting text or escaped character in string; found " + to_string(curr_token.type);
        throw Syntactic_error(curr_token, message);
      }
    }
  }
  catch (const Preproc_error& error) {
    for (Expression*& expression : *expr_list) {
      delete expression;
    }
    delete expr_list;
    throw error;
  }
}

Expression* Parser::array()
{
  List<Pair<Expression*, Expression*>>* expr_list = new List<Pair<Expression*, Expression*>>();
  Expression* left_expr = nullptr;
  Expression* right_expr = nullptr;
  try {
    Token token = advance();
    if (curr_token.type != Token::Type::RIGHT_BRACK) {
      do {
        left_expr = ternary();
        if (match(Token::Type::DOT_DOT)) {
          right_expr = ternary();
        }
        expr_list->push_back(Pair<Expression*, Expression*>(left_expr, right_expr));
        left_expr = nullptr;
        right_expr = nullptr;
      } while (match(Token::Type::COMMA));
    }
    consume(Token::Type::RIGHT_BRACK);
    return new Array(token, expr_list);
  }
  catch (const Preproc_error& error) {
    delete left_expr;
    delete right_expr;
    for (Pair<Expression*, Expression*>& pair : *expr_list) {
      delete pair.first;
      delete pair.second;
    }
    delete expr_list;
    throw error;
  }
}

Expression* Parser::dictionary()
{
  List<Pair<Expression*, Expression*>>* expr_list = new List<Pair<Expression*, Expression*>>();
  Expression* left_expr = nullptr;
  Expression* right_expr = nullptr;
  try {
    Token token = advance();
    if (curr_token.type != Token::Type::RIGHT_CURLY) {
      do {
        left_expr = ternary();
        consume(Token::Type::COLON);
        right_expr = ternary();
        expr_list->push_back(Pair<Expression*, Expression*>(left_expr, right_expr));
        left_expr = nullptr;
        right_expr = nullptr;
      } while (match(Token::Type::COMMA));
    }
    consume(Token::Type::RIGHT_CURLY);
    return new Dictionary(token, expr_list);
  }
  catch (const Preproc_error& error) {
    delete left_expr;
    delete right_expr;
    for (Pair<Expression*, Expression*>& pair : *expr_list) {
      delete pair.first;
      delete pair.second;
    }
    delete expr_list;
    throw error;
  }
}

Expression* Parser::log2_bif()
{
  Expression* expression = nullptr;
  try {
    Token token = advance();
    consume(Token::Type::LEFT_PAREN);
    expression = ternary();
    consume(Token::Type::RIGHT_PAREN);
    return new Log2_bif(token, expression);
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

Expression* Parser::clog2_bif()
{
  Expression* expression = nullptr;
  try {
    Token token = advance();
    consume(Token::Type::LEFT_PAREN);
    expression = ternary();
    consume(Token::Type::RIGHT_PAREN);
    return new Clog2_bif(token, expression);
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

//////////////////////////////////////////////////////// BUILT-IN FUNCTIONS ////////////////////////////////////////////////////////

Expression* Parser::max_bif()
{
  List<Expression*>* expr_list = new List<Expression*>();
  Expression* expression = nullptr;
  try {
    Token token = advance();
    consume(Token::Type::LEFT_PAREN);
    do {
      expression = ternary();
      expr_list->push_back(expression);
      expression = nullptr;
    } while (match(Token::Type::COMMA));
    consume(Token::Type::RIGHT_PAREN);
    return new Max_bif(token, expr_list);
  }
  catch (const Preproc_error& error) {
    for (Expression*& expression : *expr_list) {
      delete expression;
    }
    delete expr_list;
    delete expression;
    throw error;
  }
}

Expression* Parser::min_bif()
{
  List<Expression*>* expr_list = new List<Expression*>();
  Expression* expression = nullptr;
  try {
    Token token = advance();
    consume(Token::Type::LEFT_PAREN);
    do {
      expression = ternary();
      expr_list->push_back(expression);
      expression = nullptr;
    } while (match(Token::Type::COMMA));
    consume(Token::Type::RIGHT_PAREN);
    return new Min_bif(token, expr_list);
  }
  catch (const Preproc_error& error) {
    for (Expression*& expression : *expr_list) {
      delete expression;
    }
    delete expr_list;
    delete expression;
    throw error;
  }
}

Expression* Parser::size_bif()
{
  Expression* expression = nullptr;
  try {
    Token token = advance();
    consume(Token::Type::LEFT_PAREN);
    expression = ternary();
    consume(Token::Type::RIGHT_PAREN);
    return new Size_bif(token, expression);
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

//////////////////////////////////////////////////////////// LOCATIONS /////////////////////////////////////////////////////////////

Location* Parser::lhs_prefix()
{
  if (curr_token.type == Token::Type::AT_SIGN) {
    Token token = advance();
    Expression* expression = rhs_prefix();
    return new Indirection(token, expression);
  }
  else {
    return lhs_postfix();
  }
}

Location* Parser::lhs_postfix()
{
  Location* location = nullptr;
  try {
    Token token = consume(Token::Type::IDENTIFIER);
    location = new Identifier(token);
    while (curr_token.type == Token::Type::LEFT_BRACK) {
      Token token = advance();
      Expression* right_expr = subscript();
      location = new Subscript(token, location, right_expr);
    }
    return location;
  }
  catch (const Preproc_error& error) {
    delete location;
    throw error;
  }
}

Expression* Parser::subscript()
{
  Expression* expression = nullptr;
  try {
    expression = ternary();
    consume(Token::Type::RIGHT_BRACK);
    return expression;
  }
  catch (const Preproc_error& error) {
    delete expression;
    throw error;
  }
}

///////////////////////////////////////////////////////////// STORAGES /////////////////////////////////////////////////////////////

Storage* Parser::lhs_storage()
{
  if (curr_token.type == Token::Type::AT_SIGN) {
    Token token = advance();
    Expression* expression = rhs_prefix();
    return new Indirection(token, expression);
  }
  else {
    Token token = consume(Token::Type::IDENTIFIER);
    return new Identifier(token);
  }
}

///////////////////////////////////////////////////////////// HANDLES //////////////////////////////////////////////////////////////

Token Parser::consume(Token::Type type)
{
  if (curr_token.type == type) {
    return advance();
  }
  else {
    String message = "expecting " + to_string(type) + "; found " + to_string(curr_token.type);
    throw Syntactic_error(curr_token, message);
  }
}

Token Parser::advance()
{
  Token prev_token = curr_token;
  if (curr_token.type != Token::Type::END_OF_FILE) {
    curr_token = lexer.get_token();
  }
  return prev_token;
}

bool Parser::match(Token::Type type)
{
  if (curr_token.type == type) {
    advance();
    return true;
  }
  else {
    return false;
  }
}

void Parser::synchronize()
{
  lexer.synchronize();
  advance();
}

void Parser::report(const Preproc_error& error)
{
  if (error_count < 5) {
    String message = file_path.string() + ":" + error.message + "\n";
    std::cerr << message.data();
  }
  error_count++;
}
