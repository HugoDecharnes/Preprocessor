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

#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>

#include "exception.hpp"
#include "list.hpp"
#include "map.hpp"
#include "string.hpp"
#include "token.hpp"

class Lexer {
public:
  Lexer(const char* char_stream);
  ~Lexer();

  Token get_token();
  void synchronize();

private:
  enum class Mode {
    PREPROCESSOR,
    QUOTATION,
    VERILOG
  };

  Map<Token::Type> keywords;

  const char* start_char;
  const char* curr_char;
  unsigned int start_line;
  unsigned int curr_line;
  unsigned int start_column;
  unsigned int curr_column;
  unsigned int length;

  Mode mode;
  bool is_inline;
  unsigned int nesting_level;

  Token preprocessor();
  Token quotation();
  Token verilog();

  Token emit(Token::Type type);

  bool match(char expected);
  char advance();
  void reset();
};

#endif // LEXER_HPP
