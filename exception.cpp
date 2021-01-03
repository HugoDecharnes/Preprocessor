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

#include "exception.hpp"

Preproc_error::Preproc_error(const Token& token, const String& message)
  : message(format(token, message))
{
}

Preproc_error::~Preproc_error()
{
}

String Preproc_error::format(const Token& token, const String& message)
{
  String format_message = std::to_string(token.line) + ":" + std::to_string(token.column) + ": " + message + "\n";
  const char* line_start = token.start - token.column + 1;
  uint length = 0;
  while (line_start[length] != '\n' && line_start[length] != '\0') {
    length++;
  }
  format_message += String(line_start, length) + "\n";
  for (uint offset = 1; offset < token.column; offset++) {
    format_message += " ";
  }
  format_message += "^";
  return format_message;
}

Lexical_error::Lexical_error(const Token& token, const String& message)
  : Preproc_error(token, "lexical error: " + message)
{
}

Lexical_error::~Lexical_error()
{
}

Syntactic_error::Syntactic_error(const Token& token, const String& message)
  : Preproc_error(token, "syntax error: " + message)
{
}

Syntactic_error::~Syntactic_error()
{
}

Semantic_error::Semantic_error(const Token& token, const String& message)
  : Preproc_error(token, "semantic error: " + message)
{
}

Semantic_error::~Semantic_error()
{
}
