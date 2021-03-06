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

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <iostream>

#include "string.hpp"
#include "utility.hpp"

class Token {
public:
  enum class Type {
    END_OF_FILE,
    NEWLINE,
    BANG,
    BANG_EQUAL,
    DOUBLE_QUOTE,
    DOLLAR,
    PERCENT,
    AMPERS,
    AMPERS_AMPERS,
    LEFT_PAREN,
    RIGHT_PAREN,
    STAR,
    STAR_STAR,
    PLUS,
    PLUS_EQUAL,
    COMMA,
    MINUS,
    DOT,
    DOT_DOT,
    SLASH,
    INTEGER,
    COLON,
    LESS,
    LESS_LESS,
    LESS_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_GREATER,
    GREATER_EQUAL,
    QUESTION,
    AT_SIGN,
    PLAIN_TEXT,
    IDENTIFIER,
    ASSERT,
    DEFINE,
    ELSE,
    ELSEIF,
    ENDFOR,
    ENDIF,
    ENDMACRO,
    FALSE,
    FOR,
    IF,
    INCLUDE,
    INSIDE,
    LET,
    LOG2,
    CLOG2,
    MACRO,
    MAX,
    MIN,
    PRINT,
    SIZE,
    TRUE,
    LEFT_BRACK,
    ESCAPE_SEQ,
    RIGHT_BRACK,
    CARET,
    BACKTICK,
    LEFT_CURLY,
    PIPE,
    PIPE_PIPE,
    RIGHT_CURLY,
    TILDE,
    INVALID
  };

  const Token::Type type;
  const char* start;
  const uint length;
  const uint line;
  const uint column;

  Token();
  Token(const Token&) = default;
  Token(Token::Type type, const char* start, uint length, uint line, uint column);
  Token& operator=(const Token&);

  String get_text() const;
  String to_string() const;
};

String to_string(Token::Type type);

#endif // TOKEN_HPP
