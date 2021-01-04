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

#include "lexer.hpp"

#define is_upper(c) ((c) >= 'A' && (c) <= 'Z')
#define is_lower(c) ((c) >= 'a' && (c) <= 'z')
#define is_digit(c) ((c) >= '0' && (c) <= '9')

#define is_alpha(c) (is_upper(c) || is_lower(c) || (c) == '_')
#define is_alnum(c) (is_alpha(c) || is_digit(c))

///////////////////////////////////////////////////////////// PUBLICS //////////////////////////////////////////////////////////////

Lexer::Lexer(const char* input_stream)
{
  keywords.insert(Pair<String, Token::Type>("define",   Token::Type::DEFINE));
  keywords.insert(Pair<String, Token::Type>("else",     Token::Type::ELSE));
  keywords.insert(Pair<String, Token::Type>("elseif",   Token::Type::ELSEIF));
  keywords.insert(Pair<String, Token::Type>("endfor",   Token::Type::ENDFOR));
  keywords.insert(Pair<String, Token::Type>("endif",    Token::Type::ENDIF));
  keywords.insert(Pair<String, Token::Type>("endmacro", Token::Type::ENDMACRO));
  keywords.insert(Pair<String, Token::Type>("eval",     Token::Type::EVAL));
  keywords.insert(Pair<String, Token::Type>("for",      Token::Type::FOR));
  keywords.insert(Pair<String, Token::Type>("if",       Token::Type::IF));
  keywords.insert(Pair<String, Token::Type>("include",  Token::Type::INCLUDE));
  keywords.insert(Pair<String, Token::Type>("let",      Token::Type::LET));
  keywords.insert(Pair<String, Token::Type>("macro",    Token::Type::MACRO));

  builtins.insert(Pair<String, Token::Type>("false",  Token::Type::FALSE));
  builtins.insert(Pair<String, Token::Type>("inside", Token::Type::INSIDE));
  builtins.insert(Pair<String, Token::Type>("log2",   Token::Type::LOG2));
  builtins.insert(Pair<String, Token::Type>("max",    Token::Type::MAX));
  builtins.insert(Pair<String, Token::Type>("min",    Token::Type::MIN));
  builtins.insert(Pair<String, Token::Type>("size",   Token::Type::SIZE));
  builtins.insert(Pair<String, Token::Type>("true",   Token::Type::TRUE));

  curr_char = input_stream;
  curr_line = 1;
  curr_column = 1;

  mode = Lexer::Mode::VERILOG;
  is_inline = false;
  nesting_level = 0;

  reset();
}

Lexer::~Lexer()
{
}

///////////////////////////////////////////////////////////// ANALYSIS /////////////////////////////////////////////////////////////

Token Lexer::get_token()
{
  switch (mode) {
  case Lexer::Mode::PREPROCESSOR:
    return preprocessor();
  case Lexer::Mode::QUOTATION:
    return quotation();
  default:
    return verilog();
  }
}

Token Lexer::verilog()
{
  switch (Lexer::advance()) {
  case '\0':
    return emit(Token::Type::END_OF_FILE);

  case '`':
    if (match('`')) {
      start_char++;
      start_column++;
      length = 1;
      while (*curr_char == '`') {
        advance();
      }
      return emit(Token::Type::PLAIN_TEXT);
    }
    else {
      mode = Lexer::Mode::PREPROCESSOR;
      is_inline = true;
      return emit(Token::Type::BACKTICK);
    }

  default:
    while (*curr_char != '`' && *curr_char != '\0') {
      advance();
    }
    return emit(Token::Type::PLAIN_TEXT);
  }
}

Token Lexer::preprocessor()
{
  for (;;) {
    switch (Lexer::advance()) {
    case '\0':
      return emit(Token::Type::END_OF_FILE);

    case '\n':
      if (nesting_level == 0) {
        mode = Lexer::Mode::VERILOG;
        return emit(Token::Type::NEWLINE);
      }
      continue;

    case ' ':
    case '\t':
      while (match(' ')) {
      }
      reset();
      continue;

    case '!':
      if (match('=')) {
        return emit(Token::Type::BANG_EQUAL);
      }
      else {
        return emit(Token::Type::BANG);
      }
    
    case '\"':
      mode = Lexer::Mode::QUOTATION;
      return emit(Token::Type::DOUBLE_QUOTE);

    case '%':
      return emit(Token::Type::PERCENT);

    case '&':
      if (match('&')) {
        return emit(Token::Type::AMPERS_AMPERS);
      }
      else {
        return emit(Token::Type::AMPERS);
      }

    case '(':
      nesting_level++;
      return emit(Token::Type::LEFT_PAREN);

    case ')':
      if (nesting_level != 0) {
        nesting_level--;
      }
      if (nesting_level == 0 && is_inline) {
        mode = Lexer::Mode::VERILOG;
      }
      return emit(Token::Type::RIGHT_PAREN);

    case '*':
      if (match('*')) {
        return emit(Token::Type::STAR_STAR);
      }
      else {
        return emit(Token::Type::STAR);
      }

    case '+':
      if (match('=')) {
        return emit(Token::Type::PLUS_EQUAL);
      }
      else {
        return emit(Token::Type::PLUS);
      }
    
    case ',':
      return emit(Token::Type::COMMA);

    case '-':
      return emit(Token::Type::MINUS);
    
    case '.':
      if (match('.')) {
        return emit(Token::Type::DOT_DOT);
      }
      else {
        return emit(Token::Type::DOT);
      }

    case '/':
      return emit(Token::Type::SLASH);

    case '0' ... '9':
      while (is_digit(*curr_char)) {
        advance();
      }
      if (nesting_level == 0 && is_inline) {
        mode = Lexer::Mode::VERILOG;
      }
      return emit(Token::Type::INTEGER);
    
    case ':':
      return emit(Token::Type::COLON);
    
    case '<':
      if (match('=')) {
        return emit(Token::Type::LESS_EQUAL);
      }
      else if (match('<')) {
        return emit(Token::Type::LESS_LESS);
      }
      else {
        return emit(Token::Type::LESS);
      }
    
    case '=':
      if (match('=')) {
        return emit(Token::Type::EQUAL_EQUAL);
      }
      else {
        return emit(Token::Type::EQUAL);
      }
    
    case '>':
      if (match('=')) {
        return emit(Token::Type::GREATER_EQUAL);
      }
      else if (match('>')) {
        return emit(Token::Type::GREATER_GREATER);
      }
      else {
        return emit(Token::Type::GREATER);
      }
    
    case '?':
      return emit(Token::Type::QUESTION);
    
    case '@':
      return emit(Token::Type::AT_SIGN);
    
    case 'A' ... 'Z':
    case 'a' ... 'z':
    case '_': {
      while (is_alnum(*curr_char)) {
        advance();
      }
      String string(start_char, length);
      Map<String, Token::Type>::iterator type = keywords.find(string);
      if (type != keywords.end()) {
        is_inline = false;
        return emit(type->second);
      }
      type = builtins.find(string);
      if (type != builtins.end()) {
        return emit(type->second);
      }
      if (nesting_level == 0 && is_inline && *curr_char != '(') {
        mode = Lexer::Mode::VERILOG;
      }
      return emit(Token::Type::IDENTIFIER);
    }
    
    case '[':
      nesting_level++;
      return emit(Token::Type::LEFT_BRACK);
    
    case ']':
      if (nesting_level != 0) {
        nesting_level--;
      }
      if (nesting_level == 0 && is_inline) {
        mode = Lexer::Mode::VERILOG;
      }
      return emit(Token::Type::RIGHT_BRACK);

    case '^':
      return emit(Token::Type::CARET);
    
    case '`':
      return emit(Token::Type::BACKTICK);
    
    case '{':
      nesting_level++;
      return emit(Token::Type::LEFT_CURLY);
    
    case '|':
      if (match('|')) {
        return emit(Token::Type::PIPE_PIPE);
      }
      else {
        return emit(Token::Type::PIPE);
      }
    
    case '}':
      if (nesting_level != 0) {
        nesting_level--;
      }
      if (nesting_level == 0 && is_inline) {
        mode = Lexer::Mode::VERILOG;
      }
      return emit(Token::Type::RIGHT_CURLY);

    case '~':
      return emit(Token::Type::TILDE);

    default:
      Token token = emit(Token::Type::INVALID);
      String message = "unexpected character";
      throw Lexical_error(token, message);
    }
  }
}

Token Lexer::quotation()
{
  switch (Lexer::advance()) {
  case '\0':
    return emit(Token::Type::END_OF_FILE);
  
  case '\"':
    mode = Lexer::Mode::PREPROCESSOR;
    return emit(Token::Type::DOUBLE_QUOTE);
  
  case '\\':
    reset();
    advance();
    return emit(Token::Type::ESCAPE_SEQ);
  
  default:
    while (*curr_char != '\\' && *curr_char != '\"' && *curr_char != '\0') {
      advance();
    }
    return emit(Token::Type::PLAIN_TEXT);
  }
}

Token Lexer::emit(Token::Type type)
{
  Token token(type, start_char, length, start_line, start_column);
  reset();
  return token;
}

///////////////////////////////////////////////////////////// HANDLES //////////////////////////////////////////////////////////////

inline void Lexer::reset()
{
  start_char = curr_char;
  start_line = curr_line;
  start_column = curr_column;
  length = 0;
}

bool Lexer::match(char expected)
{
  if (*curr_char == expected) {
    advance();
    return true;
  }
  else {
    return false;
  }
}

char Lexer::advance()
{
  if (*curr_char == '\n') {
    curr_line++;
    curr_column = 1;
  }
  else {
    curr_column++;
  }
  length++;
  return *curr_char++;
}

void Lexer::synchronize()
{
  mode = Lexer::Mode::VERILOG;
  is_inline = false;
  nesting_level = 0;
}

#undef is_upper
#undef is_lower
#undef is_digit
#undef is_alpha
#undef is_alnum
