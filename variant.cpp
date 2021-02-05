// Copyright (C) 2020-2021, Hugo Décharnes
// This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
// You should have received a copy of the GNU Lesser General Public License along with this program; if not, write to the Free
// Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include "variant.hpp"

//////////////////////////////////////////////////////// VARIANT DATA CLASS ////////////////////////////////////////////////////////

Variant::Data::Data()
{
}

Variant::Data::~Data()
{
}

////////////////////////////////////////////////////////// VARIANT CLASS ///////////////////////////////////////////////////////////

Variant::Variant()
{
  type = Variant::Type::VOID;
}

Variant::Variant(int rhs)
{
  type = Variant::Type::INTEGER;
  data.INTEGER = rhs;
}

Variant::Variant(uint rhs)
{
  type = Variant::Type::INTEGER;
  data.INTEGER = (int)rhs;
}

Variant::Variant(bool rhs)
{
  type = Variant::Type::BOOLEAN;
  data.BOOLEAN = rhs;
}

Variant::Variant(const String& rhs)
{
  type = Variant::Type::STRING;
  new (&data.STRING) Shared_ptr<String>();
  data.STRING = std::make_shared<String>(rhs);
}

Variant::Variant(const Vector<Variant>& rhs)
{
  type = Variant::Type::ARRAY;
  new (&data.ARRAY) Shared_ptr<Vector<Variant>>();
  data.ARRAY = std::make_shared<Vector<Variant>>(rhs);
}

Variant::Variant(const Map<String, Variant>& rhs)
{
  type = Variant::Type::DICTIONARY;
  new (&data.DICTIONARY) Shared_ptr<Map<String, Variant>>();
  data.DICTIONARY = std::make_shared<Map<String, Variant>>(rhs);
}

Variant::Variant(Macro* rhs)
{
  type = Variant::Type::MACRO;
  data.MACRO = rhs;
}

Variant::Variant(const Variant& rhs)
{
  switch (rhs.type) {
  case Variant::Type::INTEGER:
    type = Variant::Type::INTEGER;
    data.INTEGER = rhs.data.INTEGER;
    break;
  case Variant::Type::BOOLEAN:
    type = Variant::Type::BOOLEAN;
    data.BOOLEAN = rhs.data.BOOLEAN;
    break;
  case Variant::Type::STRING:
    type = Variant::Type::STRING;
    new (&data.STRING) Shared_ptr<String>();
    data.STRING = rhs.data.STRING;
    break;
  case Variant::Type::ARRAY:
    type = Variant::Type::ARRAY;
    new (&data.ARRAY) Shared_ptr<Vector<Variant>>();
    data.ARRAY = rhs.data.ARRAY;
    break;
  case Variant::Type::DICTIONARY:
    type = Variant::Type::DICTIONARY;
    new (&data.DICTIONARY) Shared_ptr<Map<String, Variant>>();
    data.DICTIONARY = rhs.data.DICTIONARY;
    break;
  case Variant::Type::MACRO:
    type = Variant::Type::MACRO;
    data.MACRO = rhs.data.MACRO;
    break;
  default:
    type = Variant::Type::VOID;
    break;
  }
}

Variant::~Variant()
{
  switch (type) {
  case Variant::Type::STRING:
    data.STRING.reset();
    break;
  case Variant::Type::ARRAY:
    data.ARRAY.reset();
    break;
  case Variant::Type::DICTIONARY:
    data.STRING.reset();
    break;
  default:
    break;
  }
}

Variant& Variant::operator=(int rhs)
{
  this->~Variant();
  type = Variant::Type::INTEGER;
  data.INTEGER = rhs;
  return *this;
}

Variant& Variant::operator=(uint rhs)
{
  this->~Variant();
  type = Variant::Type::INTEGER;
  data.INTEGER = (int)rhs;
  return *this;
}

Variant& Variant::operator=(bool rhs)
{
  this->~Variant();
  type = Variant::Type::BOOLEAN;
  data.BOOLEAN = rhs;
  return *this;
}

Variant& Variant::operator=(const String& rhs)
{
  this->~Variant();
  type = Variant::Type::STRING;
  new (&data.STRING) Shared_ptr<String>();
  data.STRING = std::make_shared<String>(rhs);
  return *this;
}

Variant& Variant::operator=(const Vector<Variant>& rhs)
{
  this->~Variant();
  type = Variant::Type::ARRAY;
  new (&data.ARRAY) Shared_ptr<Vector<Variant>>();
  data.ARRAY = std::make_shared<Vector<Variant>>(rhs);
  return *this;
}

Variant& Variant::operator=(const Map<String, Variant>& rhs)
{
  this->~Variant();
  type = Variant::Type::DICTIONARY;
  new (&data.DICTIONARY) Shared_ptr<Map<String, Variant>>();
  data.DICTIONARY = std::make_shared<Map<String, Variant>>(rhs);
  return *this;
}

Variant& Variant::operator=(Macro* rhs)
{
  this->~Variant();
  type = Variant::Type::MACRO;
  data.MACRO = rhs;
  return *this;
}

Variant& Variant::operator=(const Variant& rhs)
{
  if (this != &rhs) {
    this->~Variant();
    switch (rhs.type) {
    case Variant::Type::INTEGER:
      type = Variant::Type::INTEGER;
      data.INTEGER = rhs.data.INTEGER;
      break;
    case Variant::Type::BOOLEAN:
      type = Variant::Type::BOOLEAN;
      data.BOOLEAN = rhs.data.BOOLEAN;
      break;
    case Variant::Type::STRING:
      type = Variant::Type::STRING;
      new (&data.STRING) Shared_ptr<String>();
      data.STRING = rhs.data.STRING;
      break;
    case Variant::Type::ARRAY:
      type = Variant::Type::ARRAY;
      new (&data.ARRAY) Shared_ptr<Vector<Variant>>();
      data.ARRAY = rhs.data.ARRAY;
      break;
    case Variant::Type::DICTIONARY:
      type = Variant::Type::DICTIONARY;
      new (&data.DICTIONARY) Shared_ptr<Map<String, Variant>>();
      data.DICTIONARY = rhs.data.DICTIONARY;
      break;
    case Variant::Type::MACRO:
      type = Variant::Type::MACRO;
      data.MACRO = rhs.data.MACRO;
      break;
    default:
      type = Variant::Type::VOID;
      break;
    }
  }
  return *this;
}

Variant& Variant::operator+=(int rhs)
{
  if (type == Variant::Type::INTEGER) {
    data.INTEGER += rhs;
    return *this;
  }
  else {
    String message = "unexpected " + to_string(type) + " on '+=' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant& Variant::operator+=(uint rhs)
{
  if (type == Variant::Type::INTEGER) {
    data.INTEGER += (int)rhs;
    return *this;
  }
  else {
    String message = "unexpected " + to_string(type) + " on '+=' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant& Variant::operator+=(bool rhs)
{
  if (type == Variant::Type::BOOLEAN) {
    data.BOOLEAN += rhs;
    return *this;
  }
  else {
    String message = "unexpected " + to_string(type) + " on '+=' left-hand side; expecting boolean";
    throw Bad_variant_access(message);
  }
}

Variant& Variant::operator+=(const String& rhs)
{
  if (type == Variant::Type::STRING) {
    *data.STRING += rhs;
    return *this;
  }
  else {
    String message = "unexpected " + to_string(type) + " on '+=' left-hand side; expecting string";
    throw Bad_variant_access(message);
  }
}

Variant& Variant::operator+=(const Vector<Variant>& rhs)
{
  if (type == Variant::Type::ARRAY) {
    for (const Variant& item : rhs) {
      data.ARRAY->push_back(item);
    }
    return *this;
  }
  else {
    String message = "unexpected " + to_string(type) + " on '+=' left-hand side; expecting list";
    throw Bad_variant_access(message);
  }
}

Variant& Variant::operator+=(const Map<String, Variant>& rhs)
{
  if (type == Variant::Type::DICTIONARY) {
    for (const Pair<String, Variant>& item : rhs) {
      data.DICTIONARY->insert(item);
    }
    return *this;
  }
  else {
    String message = "unexpected " + to_string(type) + " on '+=' left-hand side; expecting dictionary";
    throw Bad_variant_access(message);
  }
}

Variant& Variant::operator+=(const Variant& rhs)
{
  switch (type) {
  case Variant::Type::INTEGER:
    if (rhs.type == Variant::Type::INTEGER) {
      data.INTEGER += rhs.data.INTEGER;
      break;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '+=' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  case Variant::Type::BOOLEAN:
    if (rhs.type == Variant::Type::BOOLEAN) {
      data.BOOLEAN += rhs.data.BOOLEAN;
      break;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '+=' right-hand side; expecting boolean";
      throw Bad_variant_access(message);
    }
  case Variant::Type::STRING:
    if (rhs.type == Variant::Type::STRING) {
      *data.STRING += *rhs.data.STRING;
      break;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '+=' right-hand side; expecting string";
      throw Bad_variant_access(message);
    }
  case Variant::Type::ARRAY:
    if (rhs.type == Variant::Type::ARRAY) {
      for (const Variant& item : *rhs.data.ARRAY) {
        data.ARRAY->push_back(item);
      }
      break;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '+=' right-hand side; expecting list";
      throw Bad_variant_access(message);
    }
  case Variant::Type::DICTIONARY:
    if (rhs.type == Variant::Type::DICTIONARY) {
      for (const Pair<String, Variant>& item : *rhs.data.DICTIONARY) {
        data.DICTIONARY->insert(item);
      }
      break;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '+=' right-hand side; expecting dictionary";
      throw Bad_variant_access(message);
    }
  default:
    String message = "unexpected void type on '+=' left-hand side; expecting any valid type";
    throw Bad_variant_access(message);
  }
  return *this;
}

Variant& Variant::operator[](int rhs) const
{
  if (type == Variant::Type::ARRAY) {
    return data.ARRAY->at(rhs);
  }
  else {
    String message = "unexpected " + to_string(type) + " on '[]' left-hand side; expecting list or dictionary";
    throw Bad_variant_access(message);
  }
}

Variant& Variant::operator[](uint rhs) const
{
  if (type == Variant::Type::ARRAY) {
    return data.ARRAY->at(rhs);
  }
  else {
    String message = "unexpected " + to_string(type) + " on '[]' left-hand side; expecting list or dictionary";
    throw Bad_variant_access(message);
  }
}

Variant& Variant::operator[](const String& rhs) const
{
  if (type == Variant::Type::DICTIONARY) {
    return data.DICTIONARY->at(rhs);
  }
  else {
    String message = "unexpected " + to_string(type) + " on '[]' left-hand side; expecting list or dictionary";
    throw Bad_variant_access(message);
  }
}

Variant& Variant::operator[](const Variant& rhs) const
{
  switch (type) {
  case Variant::Type::ARRAY:
    if (rhs.type == Variant::Type::INTEGER) {
      return data.ARRAY->at(rhs.data.INTEGER);
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '[]' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  case Variant::Type::DICTIONARY:
    if (rhs.type == Variant::Type::STRING) {
      return data.DICTIONARY->at(*rhs.data.STRING);
    }
    else {
      String message = "unexpected " + to_string(type) + " on '[]' right-hand side; expecting integer or string";
      throw Bad_variant_access(message);
    }
  default:
    String message = "unexpected " + to_string(type) + " on '[]' left-hand side; expecting list or dictionary";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator~() const
{
  if (type == Variant::Type::INTEGER) {
    return ~data.INTEGER;
  }
  else {
    String message = "unexpected " + to_string(type) + " on '~'; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator!() const
{
  if (type == Variant::Type::BOOLEAN) {
    return !data.BOOLEAN;
  }
  else {
    String message = "unexpected " + to_string(type) + " on '!'; expecting boolean";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator+() const
{
  if (type == Variant::Type::INTEGER) {
    return +data.INTEGER;
  }
  else {
    String message = "unexpected " + to_string(type) + " on '+'; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator-() const
{
  if (type == Variant::Type::INTEGER) {
    return -data.INTEGER;
  }
  else {
    String message = "unexpected " + to_string(type) + " on '-'; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator+(Variant rhs) const
{
  Variant result;
  switch (type) {
  case Variant::Type::INTEGER:
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER + rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '+' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  case Variant::Type::BOOLEAN:
    if (rhs.type == Variant::Type::BOOLEAN) {
      return data.BOOLEAN + rhs.data.BOOLEAN;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '+' right-hand side; expecting boolean";
      throw Bad_variant_access(message);
    }
  case Variant::Type::STRING:
    if (rhs.type == Variant::Type::STRING) {
      Variant result(*data.STRING);
      result += *rhs.data.STRING;
      return result;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '+' right-hand side; expecting string";
      throw Bad_variant_access(message);
    }
  case Variant::Type::ARRAY:
    if (rhs.type == Variant::Type::ARRAY) {
      Variant result(*data.ARRAY);
      result += *rhs.data.ARRAY;
      return result;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '+' right-hand side; expecting list";
      throw Bad_variant_access(message);
    }
  case Variant::Type::DICTIONARY:
    if (rhs.type == Variant::Type::DICTIONARY) {
      Variant result(*data.DICTIONARY);
      result += *rhs.data.DICTIONARY;
      return result;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '+' right-hand side; expecting dictionary";
      throw Bad_variant_access(message);
    }
  default:
    String message = "unexpected void type on '+' left-hand side; expecting any valid type";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator-(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER - rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '-' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '-' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator*(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER * rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '*' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '*' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator/(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER / rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '/' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '/' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator%(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER % rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '%' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '%' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator^(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER ^ rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '^' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '^' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator&(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER & rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '&' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '&' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator|(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER | rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '|' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '|' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator<(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER < rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '<' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '<' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator>(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER > rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '>' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '>' left'-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator<<(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER << rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '<<' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '<<' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator>>(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER >> rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '>>' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '>>' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator==(Variant rhs) const
{
  switch (type) {
  case Variant::Type::INTEGER:
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER == rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(type) + " on '==' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  case Variant::Type::BOOLEAN:
    if (rhs.type == Variant::Type::BOOLEAN) {
      return data.BOOLEAN == rhs.data.BOOLEAN;
    }
    else {
      String message = "unexpected " + to_string(type) + " on '==' right-hand side; expecting boolean";
      throw Bad_variant_access(message);
    }
  case Variant::Type::STRING:
    if (rhs.type == Variant::Type::STRING) {
      return data.STRING == rhs.data.STRING;
    }
    else {
      String message = "unexpected " + to_string(type) + " on '==' right-hand side; expecting string";
      throw Bad_variant_access(message);
    }
  default:
    String message = "unexpected " + to_string(type) + " on '==' left-hand side; expecting integer, boolean or string";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator!=(Variant rhs) const
{
  switch (type) {
  case Variant::Type::INTEGER:
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER != rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(type) + " on '!=' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  case Variant::Type::BOOLEAN:
    if (rhs.type == Variant::Type::BOOLEAN) {
      return data.BOOLEAN != rhs.data.BOOLEAN;
    }
    else {
      String message = "unexpected " + to_string(type) + " on '!=' right-hand side; expecting boolean";
      throw Bad_variant_access(message);
    }
  case Variant::Type::STRING:
    if (rhs.type == Variant::Type::STRING) {
      return data.STRING != rhs.data.STRING;
    }
    else {
      String message = "unexpected " + to_string(type) + " on '!=' right-hand side; expecting string";
      throw Bad_variant_access(message);
    }
  default:
    String message = "unexpected " + to_string(type) + " on '!=' left-hand side; expecting integer, boolean or string";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator<=(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER <= rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '<=' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '<=' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator>=(Variant rhs) const
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      return data.INTEGER >= rhs.data.INTEGER;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '>=' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '>=' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator&&(Variant rhs) const
{
  if (type == Variant::Type::BOOLEAN) {
    if (rhs.type == Variant::Type::BOOLEAN) {
      return data.BOOLEAN && rhs.data.BOOLEAN;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '&&' right-hand side; expecting boolean";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '&&' left-hand side; expecting boolean";
    throw Bad_variant_access(message);
  }
}

Variant Variant::operator||(Variant rhs) const
{
  if (type == Variant::Type::BOOLEAN) {
    if (rhs.type == Variant::Type::BOOLEAN) {
      return data.BOOLEAN || rhs.data.BOOLEAN;
    }
    else {
      String message = "unexpected " + to_string(rhs.type) + " on '||' right-hand side; expecting boolean";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '||' left-hand side; expecting boolean";
    throw Bad_variant_access(message);
  }
}

Variant Variant::pow(const Variant& rhs)
{
  if (type == Variant::Type::INTEGER) {
    if (rhs.type == Variant::Type::INTEGER) {
      if (rhs.data.INTEGER >= 0) {
        int result = 1;
        for (int i = 0; i != rhs.data.INTEGER; i++) {
          result *= data.INTEGER;
        }
        return result;
      }
      else {
        String message = "value on '**' right-hand side must be positive or null";
        throw Bad_variant_access(message);
      }
    }
    else {
      String message = "unexpected " + rhs.to_string(rhs.type) + " on '**' right-hand side; expecting integer";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on '**' left-hand side; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::log2()
{
  if (type == Variant::Type::INTEGER) {
    if (data.INTEGER > 0) {
      int result = 0;
      int value = data.INTEGER;
      while (value >>= 1) {
        result++;
      }
      return result;
    }
    else {
      String message = "value on 'log2' must be positive";
      throw Bad_variant_access(message);
    }
  }
  else {
    String message = "unexpected " + to_string(type) + " on 'log2'; expecting integer";
    throw Bad_variant_access(message);
  }
}

Variant Variant::clog2()
{
  Variant result = this->log2();
  if ((data.INTEGER & (data.INTEGER - 1)) != 0) {
    result.data.INTEGER++;
  }
  return result;
}

int Variant::get_int() const
{
  if (type == Variant::Type::INTEGER) {
    return data.INTEGER;
  }
  else {
    String message = "unexpected " + to_string(type) + " on type conversion; expecting integer";
    throw Bad_variant_access(message);
  }
}

bool Variant::get_bool() const
{
  if (type == Variant::Type::BOOLEAN) {
    return data.BOOLEAN;
  }
  else {
    String message = "unexpected " + to_string(type) + " on type conversion; expecting boolean";
    throw Bad_variant_access(message);
  }
}

String& Variant::get_string() const
{
  if (type == Variant::Type::STRING) {
    return *data.STRING;
  }
  else {
    String message = "unexpected " + to_string(type) + " on type conversion; expecting string";
    throw Bad_variant_access(message);
  }
}

Vector<Variant>& Variant::get_array() const
{
  if (type == Variant::Type::ARRAY) {
    return *data.ARRAY;
  }
  else {
    String message = "unexpected " + to_string(type) + " on type conversion; expecting list";
    throw Bad_variant_access(message);
  }
}

Map<String, Variant>& Variant::get_dictionary() const
{
  if (type == Variant::Type::DICTIONARY) {
    return *data.DICTIONARY;
  }
  else {
    String message = "unexpected " + to_string(type) + " on type conversion; expecting dictionary";
    throw Bad_variant_access(message);
  }
}

Macro* Variant::get_macro() const
{
  if (type == Variant::Type::MACRO) {
    return data.MACRO;
  }
  else {
    String message = "unexpected " + to_string(type) + " on type conversion; expecting macro";
    throw Bad_variant_access(message);
  }
}

///////////////////////////////////////////////////////////// HANDLERS /////////////////////////////////////////////////////////////

String Variant::to_string() const
{
  switch (type) {
  case Variant::Type::INTEGER:
    return std::to_string(data.INTEGER);
  case Variant::Type::BOOLEAN:
    return std::to_string(data.BOOLEAN);
  case Variant::Type::STRING:
    return *data.STRING;
  case Variant::Type::VOID:
    return "";
  default:
    String message = "cannot stringify " + to_string(type);
    throw Bad_variant_access(message);
  }
}

String Variant::to_string(Variant::Type type) const
{
  switch (type) {
  case Variant::Type::INTEGER:
    return "integer";
  case Variant::Type::BOOLEAN:
    return "boolean";
  case Variant::Type::STRING:
    return "string";
  case Variant::Type::ARRAY:
    return "list";
  case Variant::Type::DICTIONARY:
    return "dictionary";
  case Variant::Type::MACRO:
    return "macro";
  default:
    return "void type";
  }
}

Bad_variant_access::Bad_variant_access(const String& message)
  : message(message)
{
}

Bad_variant_access::~Bad_variant_access()
{
}
