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

#ifndef STRING_HPP
#define STRING_HPP

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <cstdlib>
#include <cstring>
#include <iostream>

class String {
public:
  String();
  String(const char* chars);
  String(const char* chars, unsigned int length);
  String(const String& string);

  ~String();

  String& operator=(const char* rhs);
  String& operator=(const String& rhs);

  String& operator+=(const char* rhs);
  String& operator+=(const String& rhs);

  String& append(const char* chars, unsigned int length);

  friend String operator+(const char* lhs, const String& rhs);
  friend String operator+(const String& lhs, const char* rhs);
  friend String operator+(const String& lhs, const String& rhs);

  friend bool operator==(const char* lhs, const String& rhs);
  friend bool operator==(const String& lhs, const char* rhs);
  friend bool operator==(const String& lhs, const String& rhs);

  friend bool operator!=(const char* lhs, const String& rhs);
  friend bool operator!=(const String& lhs, const char* rhs);
  friend bool operator!=(const String& lhs, const String& rhs);

  int compare(const char* chars) const;
  int compare(const String& string) const;

  const char* data() const;

  String str() const;

  String extension() const;

  String& remove_extension();

private:
  struct Substr {
  public:
    String::Substr* next;
    char chars[1];
  };

  mutable String::Substr* head;
  mutable String::Substr* tail;
  unsigned int length;

  static String::Substr* new_substr(const char* start, unsigned int length);
  static String::Substr* new_substr(const String& string);

  String& append(String::Substr* substr, unsigned int length);

  void flatten() const;
  void clear();
};

String operator+(const char* lhs, const String& rhs);
String operator+(const String& lhs, const char* rhs);
String operator+(const String& lhs, const String& rhs);

bool operator==(const char* lhs, const String& rhs);
bool operator==(const String& lhs, const char* rhs);
bool operator==(const String& lhs, const String& rhs);

bool operator!=(const char* lhs, const String& rhs);
bool operator!=(const String& lhs, const char* rhs);
bool operator!=(const String& lhs, const String& rhs);

String to_string(bool val);
String to_string(int val);
String to_string(unsigned int val);

char* itoa(int value, char* start, int base);

#endif // STRING_HPP
