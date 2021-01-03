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

#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>
#include <stdexcept>

using Exception     = std::exception;
using Runtime_error = std::runtime_error;
using Out_of_range  = std::out_of_range;

class Preproc_error;
class Syntactic_error;
class Semantic_error;

#include "filesystem.hpp"
#include "string.hpp"
#include "token.hpp"
#include "utility.hpp"

////////////////////////////////////////////////////// PREPROCESSOR ERRORS ///////////////////////////////////////////////////////

class Preproc_error : public Exception {
public:
  Preproc_error(const Token& token, const String& message);
  ~Preproc_error();
  const String message;
private:
  String format(const Token& token, const String& message);
};

class Lexical_error : public Preproc_error {
public:
  Lexical_error(const Token& token, const String& message);
  ~Lexical_error();
};

class Syntactic_error : public Preproc_error {
public:
  Syntactic_error(const Token& token, const String& message);
  ~Syntactic_error();
};

class Semantic_error : public Preproc_error {
public:
  Semantic_error(const Token& token, const String& message);
  ~Semantic_error();
};

#endif // ERROR_HPP
