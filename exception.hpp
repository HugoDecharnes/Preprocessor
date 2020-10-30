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
#include <iostream>
#include "string.hpp"
#include "token.hpp"

class Exception : std::exception {
public:
  Exception(const String& message);
  ~Exception();
  const String message;
};

////////////////////////////////////////////////////// PREPROCESSOR ERRORS ///////////////////////////////////////////////////////

class Preproc_error : public Exception {
public:
  Preproc_error(const Token& token, const String& message);
  ~Preproc_error();
private:
  String format(const Token& token, const String& message) const;
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

///////////////////////////////////////////////////////// RUNTIME ERRORS /////////////////////////////////////////////////////////

class Runtime_error : public Exception {
public:
  Runtime_error(const String& message);
  ~Runtime_error();
};

////////////////////////////////////////////////////////// LOGIC ERRORS //////////////////////////////////////////////////////////

class Out_of_range : public Exception {
public:
  Out_of_range(const String& message);
  ~Out_of_range();
};

#endif // ERROR_HPP
