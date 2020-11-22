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
    return "end of file";
  case Token::Type::NEWLINE:
    return "'\\n'";
  case Token::Type::BANG:
    return "'!'";
  case Token::Type::BANG_EQUAL:
    return "'!='";
  case Token::Type::DOUBLE_QUOTE:
    return "'\"'";
  case Token::Type::DOLLAR_SIGN:
    return "'$'";
  case Token::Type::PERCENT:
    return "'%'";
  case Token::Type::AMPERS:
    return "'&'";
  case Token::Type::AMPERS_AMPERS:
    return "'&&'";
  case Token::Type::LEFT_PAREN:
    return "'('";
  case Token::Type::RIGHT_PAREN:
    return "')'";
  case Token::Type::STAR:
    return "'*'";
  case Token::Type::STAR_STAR:
    return "'**'";
  case Token::Type::PLUS:
    return "'+'";
  case Token::Type::PLUS_EQUAL:
    return "'+='";
  case Token::Type::COMMA:
    return "','";
  case Token::Type::MINUS:
    return "'-'";
  case Token::Type::DOT:
    return "'.'";
  case Token::Type::DOT_DOT:
    return "'..'";
  case Token::Type::SLASH:
    return "'/'";
  case Token::Type::INTEGER:
    return "integer";
  case Token::Type::COLON:
    return "':'";
  case Token::Type::LESS:
    return "'<'";
  case Token::Type::LESS_LESS:
    return "'<<";
  case Token::Type::LESS_EQUAL:
    return "'<='";
  case Token::Type::EQUAL:
    return "'='";
  case Token::Type::EQUAL_EQUAL:
    return "'=='";
  case Token::Type::GREATER:
    return "'>'";
  case Token::Type::GREATER_GREATER:
    return "'>='";
  case Token::Type::GREATER_EQUAL:
    return "'>>'";
  case Token::Type::QUESTION:
    return "'?'";
  case Token::Type::AT_SIGN:
    return "'@'";
  case Token::Type::PLAIN_TEXT:
    return "plain text";
  case Token::Type::IDENTIFIER:
    return "identifier";
  case Token::Type::BEGIN:
    return "'begin'";
  case Token::Type::BLOCK:
    return "'block'";
  case Token::Type::DEFINE:
    return "'define'";
  case Token::Type::ELSE:
    return "'else'";
  case Token::Type::ELSEIF:
    return "'elseif'";
  case Token::Type::ENDBLOCK:
    return "'endblock'";
  case Token::Type::ENDFOR:
    return "'endfor'";
  case Token::Type::ENDIF:
    return "'endif'";
  case Token::Type::ENDMACRO:
    return "'endmacro'";
  case Token::Type::FALSE:
    return "'false'";
  case Token::Type::FOR:
    return "'for'";
  case Token::Type::IF:
    return "'if'";
  case Token::Type::INCLUDE:
    return "'include'";
  case Token::Type::INSIDE:
    return "'inside'";
  case Token::Type::LET:
    return "'let'";
  case Token::Type::LOG2:
    return "'log2'";
  case Token::Type::MACRO:
    return "'macro'";
  case Token::Type::MUT:
    return "'mut'";
  case Token::Type::RETURN:
    return "'return'";
  case Token::Type::SIZE:
    return "'size'";
  case Token::Type::TRUE:
    return "'true'";
  case Token::Type::LEFT_BRACK:
    return "'['";
  case Token::Type::ESCAPE_SEQ:
    return "'\\\\'";
  case Token::Type::RIGHT_BRACK:
    return "']'";
  case Token::Type::CARET:
    return "'^'";
  case Token::Type::BACKTICK:
    return "'`'";
  case Token::Type::LEFT_CURLY:
    return "'{'";
  case Token::Type::PIPE:
    return "'|'";
  case Token::Type::PIPE_PIPE:
    return "'||'";
  case Token::Type::RIGHT_CURLY:
    return "'}'";
  case Token::Type::TILDE:
    return "'~'";
  default:
    return "invalid token";
  }
}
