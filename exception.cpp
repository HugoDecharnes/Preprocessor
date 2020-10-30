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

Exception::Exception(const String& message)
  : message(message)
{
}

Exception::~Exception()
{
}

/////////////////////////////////////////////////////// PREPROCESSOR ERRORS ////////////////////////////////////////////////////////

Preproc_error::Preproc_error(const Token& token, const String& message)
  : Exception(format(token, message))
{
}

Preproc_error::~Preproc_error()
{
}

String Preproc_error::format(const Token& token, const String& message) const
{
  String format_message = to_string(token.line) + ":" + to_string(token.column) + ": "
    + message + "\n";
  const char* const line_start = token.start - token.column + 1;
  unsigned int length = 0;
  while (line_start[length] != '\n' && line_start[length] != '\0') {
    length++;
  }
  format_message += String(line_start, length) + "\n";
  for (unsigned int offset = 1; offset < token.column; offset++) {
    format_message += " ";
  }
  format_message += "^";
  return format_message;
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

////////////////////////////////////////////////////////// RUNTIME ERRORS //////////////////////////////////////////////////////////

Runtime_error::Runtime_error(const String& message)
  : Exception(message)
{
}

Runtime_error::~Runtime_error()
{
}

/////////////////////////////////////////////////////////// LOGIC ERRORS ///////////////////////////////////////////////////////////

Out_of_range::Out_of_range(const String& message)
  : Exception(message)
{
}

Out_of_range::~Out_of_range()
{
}
