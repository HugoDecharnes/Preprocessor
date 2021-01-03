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

class Variant;
class Macro;

#include "exception.hpp"
#include "map.hpp"
#include "memory.hpp"
#include "string.hpp"
#include "token.hpp"
#include "utility.hpp"
#include "vector.hpp"

class Variant {
private:
  enum class Type {
    VOID,
    INTEGER,
    BOOLEAN,
    STRING,
    ARRAY,
    DICTIONARY,
    MACRO
  };

  union Data {
    int INTEGER;
    bool BOOLEAN;
    Shared_ptr<String> STRING;
    Shared_ptr<Vector<Variant>> ARRAY;
    Shared_ptr<Map<String, Variant>> DICTIONARY;
    Macro* MACRO;

    Data();
    ~Data();
  };

  Variant::Type type;
  Variant::Data data;

  String to_string(Variant::Type type) const;

public:
  Variant();
  Variant(int rhs);
  Variant(uint rhs);
  Variant(bool rhs);
  Variant(const String& rhs);
  Variant(const Vector<Variant>& rhs);
  Variant(const Map<String, Variant>& rhs);
  Variant(Macro* rhs);
  Variant(const Variant& rhs);
  ~Variant();

  Variant& operator=(int rhs);
  Variant& operator=(uint rhs);
  Variant& operator=(bool rhs);
  Variant& operator=(const String& rhs);
  Variant& operator=(const Vector<Variant>& rhs);
  Variant& operator=(const Map<String, Variant>& rhs);
  Variant& operator=(Macro* rhs);
  Variant& operator=(const Variant& rhs);

  Variant& operator+=(int rhs);
  Variant& operator+=(uint rhs);
  Variant& operator+=(bool rhs);
  Variant& operator+=(const String& rhs);
  Variant& operator+=(const Vector<Variant>& rhs);
  Variant& operator+=(const Map<String, Variant>& rhs);
  Variant& operator+=(const Variant& rhs);

  Variant& operator[](int rhs) const;
  Variant& operator[](uint rhs) const;
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
  String& get_string() const;
  Vector<Variant>& get_array() const;
  Map<String, Variant>& get_dictionary() const;
  Macro* get_macro() const;

  String to_string() const;
};

class Bad_variant_access : public Exception {
public:
  Bad_variant_access(const String& message);
  ~Bad_variant_access();
  const String message;
};

#endif // VARIANT_HPP
