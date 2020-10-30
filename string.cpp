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

#include "string.hpp"

#define is_upper(c) ((c) >= 'A' && (c) <= 'Z')
#define is_lower(c) ((c) >= 'a' && (c) <= 'z')
#define is_digit(c) ((c) >= '0' && (c) <= '9')

#define is_alpha(c) (is_upper(c) || is_lower(c) || (c) == '_')
#define is_alnum(c) (is_alpha(c) || is_digit(c))

////////////////////////////////////////////////////////////// STRING //////////////////////////////////////////////////////////////

String::String()
  : head(nullptr), tail(nullptr), length(0)
{
}

String::String(const char* chars)
  : String::String()
{
  unsigned int length = strlen(chars);
  String::Substr* substr = new_substr(chars, length);
  append(substr, length);
}

String::String(const char* chars, unsigned int length)
  : String::String()
{
  String::Substr* substr = new_substr(chars, length);
  append(substr, length);
}

String::String(const String& string)
  : String::String()
{
  String::Substr* substr = new_substr(string);
  append(substr, string.length);
}

String::~String()
{
  clear();
}

String& String::operator=(const char* chars)
{
  unsigned int length = strlen(chars);
  String::Substr* substr = new_substr(chars, length);
  clear();
  append(substr, length);
  return *this;
}

String& String::operator=(const String& string)
{
  String::Substr* substr = new_substr(string);
  clear();
  append(substr, string.length);
  return *this;
}

String& String::operator+=(const char* chars)
{
  unsigned int length = strlen(chars);
  String::Substr* substr = new_substr(chars, length);
  append(substr, length);
  return *this;
}

String& String::operator+=(const String& string)
{
  String::Substr* substr = new_substr(string);
  append(substr, string.length);
  return *this;
}

String& String::append(const char* chars, unsigned int length)
{
  String::Substr* substr = new_substr(chars, length);
  append(substr, length);
  return *this;
}

String& String::append(String::Substr* substr, unsigned int length)
{
  if (tail != nullptr) {
    tail = tail->next = substr;
  }
  else {
    head = tail = substr;
  }
  this->length += length;
  return *this;
}

String operator+(const char* lhs, const String& rhs)
{
  String result(lhs);
  result += rhs;
  return result;
}

String operator+(const String& lhs, const char* rhs)
{
  String result(lhs);
  result += rhs;
  return result;
}

String operator+(const String& lhs, const String& rhs)
{
  String result(lhs);
  result += rhs;
  return result;
}

bool operator==(const char* lhs, const String& rhs)
{
  return rhs.compare(lhs) == 0;
}

bool operator==(const String& lhs, const char* rhs)
{
  return lhs.compare(rhs) == 0;
}

bool operator==(const String& lhs, const String& rhs)
{
  return lhs.compare(rhs) == 0;
}

bool operator!=(const char* lhs, const String& rhs)
{
  return rhs.compare(lhs) != 0;
}

bool operator!=(const String& lhs, const char* rhs)
{
  return lhs.compare(rhs) != 0;
}

bool operator!=(const String& lhs, const String& rhs)
{
  return lhs.compare(rhs) != 0;
}

int String::compare(const char* chars) const
{
  unsigned int chars_length = strlen(chars);
  int length_diff = length - chars_length;
  if (length_diff != 0) {
    return length_diff;
  }
  String::Substr* lhs_substr = head;
  const char* lhs_chars = lhs_substr->chars;
  const char* rhs_chars = chars;
  while (lhs_substr != nullptr) {
    while (*lhs_chars == '\0') {
      lhs_substr = lhs_substr->next;
      if (lhs_substr != nullptr) {
        lhs_chars = lhs_substr->chars;
      }
      else {
        break;
      }
    }
    int char_diff = *lhs_chars - *rhs_chars;
    if (char_diff != 0) {
      return char_diff;
    }
    lhs_chars++;
    rhs_chars++;
  }
  return 0;
}

int String::compare(const String& string) const
{
  int length_diff = length - string.length;
  if (length_diff != 0) {
    return length_diff;
  }
  String::Substr* lhs_substr = head;
  String::Substr* rhs_substr = string.head;
  const char* lhs_chars = lhs_substr->chars;
  const char* rhs_chars = rhs_substr->chars;
  while (lhs_substr != nullptr) {
    while (*lhs_chars == '\0') {
      lhs_substr = lhs_substr->next;
      if (lhs_substr != nullptr) {
        lhs_chars = lhs_substr->chars;
      }
      else {
        break;
      }
    }
    while (*rhs_chars == '\0') {
      rhs_substr = rhs_substr->next;
      if (rhs_substr != nullptr) {
        rhs_chars = rhs_substr->chars;
      }
      else {
        break;
      }
    }
    int char_diff = *lhs_chars - *rhs_chars;
    if (char_diff != 0) {
      return char_diff;
    }
    lhs_chars++;
    rhs_chars++;
  }
  return 0;
}

const char* String::data() const
{
  flatten();
  return (const char*)head->chars;  
}

String String::str() const
{
  String output = "\"";
  if (length != 0) {
    flatten();
    char* curr_char = head->chars;
    while (*curr_char != '\0') {
      switch (*curr_char++) {
      case '\'':
        output += "\\\'";
        continue;
      case '\"':
        output += "\\\"";
        continue;
      case '\\':
        output += "\\\\";
        continue;
      case '\a':
        output += "\\a";
        continue;
      case '\b':
        output += "\\b";
        continue;
      case '\f':
        output += "\\f";
        continue;
      case '\n':
        output += "\\n";
        continue;
      case '\r':
        output += "\\r";
        continue;
      case '\t':
        output += "\\t";
        continue;
      case '\v':
        output += "\\v";
        continue;
      default:
        char* chars = curr_char - 1;
        unsigned int length = 1;
        while (*curr_char != '\'' && *curr_char != '\"' && *curr_char != '\\' && *curr_char >= 32) {
          curr_char++;
          length++;
        }
        String::Substr* substr = new_substr(chars, length);
        output.append(substr, length);
        continue;
      }
    }
  }
  output += "\"";
  return output;
}

String String::extension() const
{
  if (length != 0) {
    flatten();
    char* ext_start = head->chars + length - 1;
    unsigned int ext_length = 1;
    while (ext_start != head->chars && is_alnum(*ext_start)) {
      ext_start--;
      ext_length++;
    }
    if (*ext_start == '.') {
      return String(ext_start, ext_length);
    }
  }
  return "";
}

String& String::remove_extension()
{
  if (length != 0) {
    flatten();
    char* ext_start = head->chars + length - 1;
    unsigned int ext_length = 1;
    while (ext_start != head->chars && is_alnum(*ext_start)) {
      ext_start--;
      ext_length++;
    }
    if (*ext_start == '.') {
      unsigned int next_length = length - ext_length;
      String::Substr* substr = new_substr(head->chars, next_length);
      clear();
      append(substr, next_length);
    }
  }
  return *this;
}

void String::flatten() const
{
  if (head != tail) {
    String::Substr* flat_substr = new_substr(*this);
    String::Substr* substr = head;
    while (substr != nullptr) {
      String::Substr* next_substr = substr->next;
      free(substr);
      substr = next_substr;
    }
    head = tail = flat_substr;
  }
}

void String::clear()
{
  String::Substr* substr = head;
  while (substr != nullptr) {
    String::Substr* next_substr = substr->next;
    free(substr);
    substr = next_substr;
  }
  head = tail = nullptr;
  length = 0;
}

//////////////////////////////////////////////////////////// SUBSTRING /////////////////////////////////////////////////////////////

String::Substr* String::new_substr(const char* chars, unsigned int length)
{
  String::Substr* substr = (String::Substr*)aligned_alloc(sizeof(void*), sizeof(void*) + (length + 1) * sizeof(char));
  substr->next = nullptr;
  memcpy(substr->chars, chars, length);
  substr->chars[length] = '\0';
  return substr;
}

String::Substr* String::new_substr(const String& string)
{
  String::Substr* substr = (String::Substr*)aligned_alloc(sizeof(void*), sizeof(void*) + (string.length + 1) * sizeof(char));
  substr->next = nullptr;
  unsigned int offset = 0;
  for (String::Substr* str_substr = string.head; str_substr != nullptr; str_substr = str_substr->next) {
    unsigned int length = strlen(str_substr->chars);
    memcpy(substr->chars + offset, str_substr->chars, length);
    offset += length;
  }
  substr->chars[offset] = '\0';
  return substr;
}

///////////////////////////////////////////////////////////// HANDLES //////////////////////////////////////////////////////////////

String to_string(bool val)
{
  if (val) {
    return "true";
  }
  else {
    return "false";
  }
}

String to_string(int val)
{
  char buffer[11];
  itoa(val, buffer, 10);
  return buffer;
}

String to_string(unsigned int val)
{
  char buffer[11];
  itoa((int)val, buffer, 10);
  return buffer;
}

char* itoa(int value, char* chars, int base)
{
  unsigned int place = 0;
  bool negative = false;
  if (value < 0 && base == 10) {
    negative = true;
    value = -value;
  }
  chars[place++] = '\0';
  if (value != 0) {
    do {
      int remainder = value % base;
      chars[place++] = (char)(remainder > 9 ? remainder - 10 + 'a' : remainder + '0');
      value /= base;
    } while (value != 0);
    if (negative) {
      chars[place++] = '-';
    }
  }
  else {
    chars[place++] = '0';
  }
  for (unsigned int fwd_offset = 0, rev_offset = place - 1; fwd_offset < rev_offset; fwd_offset++, rev_offset--) {
    char temp = chars[fwd_offset];
    chars[fwd_offset] = chars[rev_offset];
    chars[rev_offset] = temp;
  }
  return chars;
}

#undef is_upper
#undef is_lower
#undef is_digit
#undef is_alpha
#undef is_alnum
