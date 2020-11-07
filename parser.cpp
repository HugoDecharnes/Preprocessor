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

#include "parser.hpp"

///////////////////////////////////////////////////////////// PUBLICS //////////////////////////////////////////////////////////////

Parser::Parser(const String& file_name, Lexer& lexer)
  : file_name(file_name), lexer(lexer)
{
}

Parser::~Parser()
{
}

Statement* Parser::parse()
{
  curr_token = lexer.get_token();
  error_count = 0;
  Statement* statement = compound();
  try {
    consume(Token::Type::END_OF_FILE);
  }
  catch (const Syntactic_error& error) {
    report(error);
  }
  if (error_count == 0) {
    return statement;
  }
  else {
    delete statement;
    String message = file_name + ": compilation failed due to " + to_string(error_count) + " error(s)";
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
      case Token::Type::BLOCK: {
        Statement* statement = block();
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
      case Token::Type::MUT: {
        Statement* statement = mutate();
        stmt_list->push_back(statement);
        continue;
      }
      case Token::Type::RETURN: {
        Statement* statement = func_return();
        stmt_list->push_back(statement);
        continue;
      }
      case Token::Type::GLOBAL:
      case Token::Type::LOCAL: {
        Statement* statement = definition();
        stmt_list->push_back(statement);
        continue;
      }
      case Token::Type::ELSE:
      case Token::Type::ELSEIF:
      case Token::Type::END:
      case Token::Type::ENDBLOCK:
      case Token::Type::ENDIF:
      case Token::Type::ENDFOR:
        if (stmt_list->get_size() == 1) {
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
        if (stmt_list->get_size() == 1) {
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

Statement* Parser::block()
{
  Token token = advance();
  try {
    consume(Token::Type::NEWLINE);
  }
  catch (const Syntactic_error& error) {
    report(error);
    synchronize();
  }
  Statement* statement = compound();
  try {
    consume(Token::Type::ENDBLOCK);
    consume(Token::Type::NEWLINE);
  }
  catch (const Syntactic_error& error) {
    report(error);
    synchronize();
  }
  return new Block(token, statement);
}

Statement* Parser::definition()
{
  Token token = advance();
  Storage* storage = nullptr;
  try {
    storage = lhs_storage();
    if (match(Token::Type::LEFT_PAREN)) {
      Function* function = function_def();
      return new Function_def(token, storage, function);
    }
    else {
      Expression* expression = variable_def();
      return new Variable_def(token, storage, expression);
    }
  }
  catch (const Syntactic_error& error) {
    delete storage;
    report(error);
    synchronize();
    return nullptr;
  }
}

Expression* Parser::variable_def()
{
  Expression* expression = nullptr;
  try {
    consume(Token::Type::EQUAL);
    expression = ternary();
    consume(Token::Type::NEWLINE);
    return expression;
  }
  catch (const Syntactic_error& error) {
    delete expression;
    throw error;
  }
}

Function* Parser::function_def()
{
  List<Identifier*>* parameters = new List<Identifier*>();
  Statement* statement = nullptr;
  try {
    if (!match(Token::Type::RIGHT_PAREN)) {
      do {
        Token token = consume(Token::Type::IDENTIFIER);
        Identifier* parameter = new Identifier(token);
        parameters->push_back(parameter);
      } while (match(Token::Type::COMMA));
      consume(Token::Type::RIGHT_PAREN);
    }
    switch (curr_token.type) {
    case Token::Type::EQUAL: {
      Token token = advance();
      Expression* expression = ternary();
      statement = new Func_return(token, expression);
      consume(Token::Type::NEWLINE);
      break;
    }
    case Token::Type::BEGIN: {
      advance();
      consume(Token::Type::NEWLINE);
      statement = compound();
      consume(Token::Type::END);
      consume(Token::Type::NEWLINE);
      break;
    }
    default:
      String message = "expecting '=' or 'begin'; found " + to_string(curr_token.type);
      throw Syntactic_error(curr_token, message);
    }
    return new Function(file_name, parameters, statement);
  }
  catch (const Syntactic_error& error) {
    delete parameters;
    delete statement;
    throw error;
  }
}

Statement* Parser::func_return()
{
  Token token = advance();
  Expression* expression = nullptr;
  try {
    expression = ternary();
    consume(Token::Type::NEWLINE);
  }
  catch (const Syntactic_error& error) {
    report(error);
    synchronize();
  }
  return new Func_return(token, expression);
}

Statement* Parser::inclusion()
{
  Token token = advance();
  Expression* expression = nullptr;
  try {
    expression = ternary();
    consume(Token::Type::NEWLINE);
  }
  catch (const Syntactic_error& error) {
    report(error);
    synchronize();
  }
  return new Inclusion(token, expression);
}

Statement* Parser::mutate()
{
  Token token = advance();
  Location* location = nullptr;
  Expression* expression = nullptr;
  bool is_accumul = false;
  try {
    location = lhs_prefix();
    switch (curr_token.type) {
    case Token::Type::PLUS_EQUAL:
      advance();
      is_accumul = true;
      break;
    case Token::Type::EQUAL:
      advance();
      break;
    default:
      String message = "expecting '=' or '+='; found " + to_string(curr_token.type);
      throw Syntactic_error(curr_token, message);
    }
    expression = ternary();
    consume(Token::Type::NEWLINE);
  }
  catch (const Syntactic_error& error) {
    report(error);
    synchronize();
  }
  if (is_accumul) {
    return new Accumulation(token, location, expression);
  }
  else {
    return new Mutate(token, location, expression);
  }
}

Statement* Parser::expr_stmt()
{
  Token token = curr_token;
  Expression* expression = nullptr;
  try {
    expression = ternary();
  }
  catch (const Syntactic_error& error) {
    report(error);
    synchronize();
  }
  return new Expr_stmt(token, expression);
}

Statement* Parser::plain_text()
{
  Token token = advance();
  return new Plain_text(token);
}

Statement* Parser::selection()
{
  Token token = advance();
  List<std::pair<Expression*, Statement*>>* alternatives
    = new List<std::pair<Expression*, Statement*>>();
  {
    Expression* expression = nullptr;
    try {
      consume(Token::Type::LEFT_PAREN);
      expression = ternary();
      consume(Token::Type::RIGHT_PAREN);
      consume(Token::Type::NEWLINE);
    }
    catch (const Syntactic_error& error) {
      report(error);
      synchronize();
    }
    Statement* statement = compound();
    alternatives->push_back(std::pair<Expression*, Statement*>(expression, statement));
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
    catch (const Syntactic_error& error) {
      report(error);
      synchronize();
    }
    Statement* statement = compound();
    alternatives->push_back(std::pair<Expression*, Statement*>(expression, statement));
  }
  if (curr_token.type == Token::Type::ELSE) {
    Token token = advance();
    try {
      consume(Token::Type::NEWLINE);
    }
    catch (const Syntactic_error& error) {
      report(error);
      synchronize();
    }
    Expression* expression = new True_const(token);
    Statement* statement = compound();
    alternatives->push_back(std::pair<Expression*, Statement*>(expression, statement));
  }
  try {
    consume(Token::Type::ENDIF);
    consume(Token::Type::NEWLINE);
  }
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
    report(error);
    synchronize();
  }
  Statement* statement = compound();
  try {
    consume(Token::Type::ENDFOR);
    consume(Token::Type::NEWLINE);
  }
  catch (const Syntactic_error& error) {
    report(error);
    synchronize();
  }
  return new Iteration(token, storage, expression, statement);
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  case Token::Type::DOLLAR_SIGN: {
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
        List<Expression*>* expr_list = function_call();
        expression = new Function_call(token, expression, expr_list);
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
  catch (const Syntactic_error& error) {
    delete expression;
    throw error;
  }
}

// This function parses the argument list after the opening parenthese, and returns that expression list enclosed in between.
List<Expression*>* Parser::function_call()
{
  List<Expression*>* expr_list = nullptr;
  try {
    expr_list = new List<Expression*>();
    if (!match(Token::Type::RIGHT_PAREN)) {
      do {
        Expression* expression = ternary();
        expr_list->push_back(expression);
      } while (match(Token::Type::COMMA));
      consume(Token::Type::RIGHT_PAREN);
    }
    return expr_list;
  }
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
    delete expr_list;
    throw error;
  }
}

Expression* Parser::array()
{
  List<std::pair<Expression*, Expression*>>* expr_list = new List<std::pair<Expression*, Expression*>>();
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
        expr_list->push_back(std::pair<Expression*, Expression*>(left_expr, right_expr));
        left_expr = nullptr;
        right_expr = nullptr;
      } while (match(Token::Type::COMMA));
    }
    consume(Token::Type::RIGHT_BRACK);
    return new Array(token, expr_list);
  }
  catch (const Syntactic_error& error) {
    delete left_expr;
    delete right_expr;
    delete expr_list;
    throw error;
  }
}

Expression* Parser::dictionary()
{
  List<std::pair<Expression*, Expression*>>* expr_list = new List<std::pair<Expression*, Expression*>>();
  Expression* left_expr = nullptr;
  Expression* right_expr = nullptr;
  try {
    Token token = advance();
    if (curr_token.type != Token::Type::RIGHT_CURLY) {
      do {
        left_expr = ternary();
        consume(Token::Type::COLON);
        right_expr = ternary();
        expr_list->push_back(std::pair<Expression*, Expression*>(left_expr, right_expr));
        left_expr = nullptr;
        right_expr = nullptr;
      } while (match(Token::Type::COMMA));
    }
    consume(Token::Type::RIGHT_CURLY);
    return new Dictionary(token, expr_list);
  }
  catch (const Syntactic_error& error) {
    delete left_expr;
    delete right_expr;
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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
  catch (const Syntactic_error& error) {
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

void Parser::report(const Syntactic_error& error)
{
  String message = file_name + error.message + "\n";
  std::cerr << message.data();
  error_count++;
}
