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

#include <iostream>
#include "token.hpp"

Token::Token()
  : type(Token::Type::INVALID), start(nullptr), length(0), line(0), column(0)
{
}

Token::Token(Token::Type type, const char* start, unsigned int length, unsigned int line, unsigned int column)
  : type(type), start(start), length(length), line(line), column(column)
{
}

Token& Token::operator=(const Token& token)
{
  new (this) Token(token);
  return *this;
}

String Token::get_text() const
{
  return String(start, length);
}

String Token::to_string() const
{
  return "[" + ::to_string(line) + ":" + ::to_string(column) + "=\"" + String(start, length)
    + "\"," + ::to_string(type) + "]";
}

String to_string(Token::Type type)
{
  switch (type) {
  case Token::Type::END_OF_FILE:
    return "END_OF_FILE";
  case Token::Type::NEWLINE:
    return "NEWLINE";
  case Token::Type::BANG:
    return "BANG";
  case Token::Type::BANG_EQUAL:
    return "BANG_EQUAL";
  case Token::Type::DOUBLE_QUOTE:
    return "DOUBLE_QUOTE";
  case Token::Type::DOLLAR_SIGN:
    return "DOLLAR_SIGN";
  case Token::Type::PERCENT:
    return "PERCENT";
  case Token::Type::AMPERS:
    return "AMPERS";
  case Token::Type::AMPERS_AMPERS:
    return "AMPERS_AMPERS";
  case Token::Type::LEFT_PAREN:
    return "LEFT_PAREN";
  case Token::Type::RIGHT_PAREN:
    return "RIGHT_PAREN";
  case Token::Type::STAR:
    return "STAR";
  case Token::Type::STAR_STAR:
    return "STAR_STAR";
  case Token::Type::PLUS:
    return "PLUS";
  case Token::Type::PLUS_EQUAL:
    return "PLUS_EQUAL";
  case Token::Type::COMMA:
    return "COMMA";
  case Token::Type::MINUS:
    return "MINUS";
  case Token::Type::DOT:
    return "DOT";
  case Token::Type::DOT_DOT:
    return "DOT_DOT";
  case Token::Type::SLASH:
    return "SLASH";
  case Token::Type::INTEGER:
    return "INTEGER";
  case Token::Type::COLON:
    return "COLON";
  case Token::Type::LESS:
    return "LESS";
  case Token::Type::LESS_LESS:
    return "LESS_LESS";
  case Token::Type::LESS_EQUAL:
    return "LESS_EQUAL";
  case Token::Type::EQUAL:
    return "EQUAL";
  case Token::Type::EQUAL_EQUAL:
    return "EQUAL_EQUAL";
  case Token::Type::GREATER:
    return "GREATER";
  case Token::Type::GREATER_GREATER:
    return "GREATER_GREATER";
  case Token::Type::GREATER_EQUAL:
    return "GREATER_EQUAL";
  case Token::Type::QUESTION:
    return "QUESTION";
  case Token::Type::AT_SIGN:
    return "AT_SIGN";
  case Token::Type::PLAIN_TEXT:
    return "PLAIN_TEXT";
  case Token::Type::IDENTIFIER:
    return "IDENTIFIER";
  case Token::Type::BLOCK:
    return "BLOCK";
  case Token::Type::ELSE:
    return "ELSE";
  case Token::Type::ELSEIF:
    return "ELSEIF";
  case Token::Type::ENDBLOCK:
    return "ENDBLOCK";
  case Token::Type::ENDIF:
    return "ENDIF";
  case Token::Type::ENDFOR:
    return "ENDFOR";
  case Token::Type::ENDFUNC:
    return "ENDFUNC";
  case Token::Type::FALSE:
    return "FALSE";
  case Token::Type::FOR:
    return "FOR";
  case Token::Type::FUNC:
    return "FUNC";
  case Token::Type::GLOBAL:
    return "GLOBAL";
  case Token::Type::IF:
    return "IF";
  case Token::Type::INCLUDE:
    return "INCLUDE";
  case Token::Type::INSIDE:
    return "INLINE";
  case Token::Type::LOG2:
    return "LOG2";
  case Token::Type::VAR:
    return "VAR";
  case Token::Type::MUT:
    return "MUT";
  case Token::Type::RETURN:
    return "RETURN";
  case Token::Type::TRUE:
    return "TRUE";
  case Token::Type::LEFT_BRACK:
    return "LEFT_BRACK";
  case Token::Type::ESCAPE_SEQ:
    return "ESCAPE_SEQ";
  case Token::Type::RIGHT_BRACK:
    return "RIGHT_BRACK";
  case Token::Type::CARET:
    return "CARET";
  case Token::Type::BACKTICK:
    return "BACKTICK";
  case Token::Type::LEFT_CURLY:
    return "LEFT_CURLY";
  case Token::Type::PIPE:
    return "PIPE";
  case Token::Type::PIPE_PIPE:
    return "PIPE_PIPE";
  case Token::Type::RIGHT_CURLY:
    return "RIGHT_CURLY";
  case Token::Type::TILDE:
    return "TILDE";
  default:
    return "INVALID";
  }
}
