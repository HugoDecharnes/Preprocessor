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

#ifndef VARIANT_HPP
#define VARIANT_HPP

#include "exception.hpp"
#include "list.hpp"
#include "map.hpp"
#include "string.hpp"
#include "token.hpp"

class Function;

class Variant {
private:
  enum class Type {
    VOID,
    INTEGER,
    BOOLEAN,
    STRING,
    ARRAY,
    DICTIONARY,
    FUNCTION
  };

  union Data {
    int INTEGER;
    bool BOOLEAN;
    String* STRING;
    List<Variant>* ARRAY;
    Map<Variant>* DICTIONARY;
    Function* FUNCTION;

    Data();
    ~Data();
  };

  Variant::Type type;
  Variant::Data data;

  String to_string(Variant::Type type) const;

public:
  Variant();
  Variant(int rhs);
  Variant(unsigned int rhs);
  Variant(bool rhs);
  Variant(const String& rhs);
  Variant(const List<Variant>& rhs);
  Variant(const Map<Variant>& rhs);
  Variant(Function* rhs);
  Variant(const Variant& rhs);
  ~Variant();

  Variant& operator=(int rhs);
  Variant& operator=(unsigned int rhs);
  Variant& operator=(bool rhs);
  Variant& operator=(const String& rhs);
  Variant& operator=(const List<Variant>& rhs);
  Variant& operator=(const Map<Variant>& rhs);
  Variant& operator=(Function* rhs);
  Variant& operator=(const Variant& rhs);

  Variant& operator+=(int rhs);
  Variant& operator+=(unsigned int rhs);
  Variant& operator+=(bool rhs);
  Variant& operator+=(const String& rhs);
  Variant& operator+=(const List<Variant>& rhs);
  Variant& operator+=(const Map<Variant>& rhs);
  Variant& operator+=(const Variant& rhs);

  Variant& operator[](int rhs) const;
  Variant& operator[](unsigned int rhs) const;
  Variant& operator[](const String& rhs) const;
  Variant& operator[](const Variant& rhs) const;

  Variant operator~() const;
  Variant operator!() const;
  Variant operator+() const;
  Variant operator-() const;
  Variant operator+(Variant rhs) const;
  Variant operator-(Variant rhs) const;
  Variant operator*(Variant rhs) const;
  Variant operator/(Variant rhs) const;
  Variant operator%(Variant rhs) const;
  Variant operator^(Variant rhs) const;
  Variant operator&(Variant rhs) const;
  Variant operator|(Variant rhs) const;
  Variant operator<(Variant rhs) const;
  Variant operator>(Variant rhs) const;
  Variant operator<<(Variant rhs) const;
  Variant operator>>(Variant rhs) const;
  Variant operator==(Variant rhs) const;
  Variant operator!=(Variant rhs) const;
  Variant operator<=(Variant rhs) const;
  Variant operator>=(Variant rhs) const;
  Variant operator&&(Variant rhs) const;
  Variant operator||(Variant rhs) const;

  Variant pow(const Variant& lhs);
  Variant log2();

  int get_int() const;
  bool get_bool() const;
  const String& get_string() const;
  const Function& get_function() const;
  const List<Variant>& get_array() const;
  const Map<Variant>& get_dictionary() const;

  String to_string() const;
};

/////////////////////////////////////////////////////// EXCEPTION CLASSES ////////////////////////////////////////////////////////

class Bad_variant : public std::exception {
public:
  explicit Bad_variant(const String& message);
  String message;
};

#endif // VARIANT_HPP
