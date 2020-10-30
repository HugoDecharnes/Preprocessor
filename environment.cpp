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

#include "environment.hpp"

Environment::Environment(const String& file_name)
  : curr_file(&file_name)
{
  locals.push_front(List<Map<Variant>>());
  push_block_scope();
}

Environment::~Environment()
{
}

void Environment::put_global(const String& key, const Variant& value)
{
  globals.insert(key, value);
}

void Environment::put_local(const String& key, const Variant& value)
{
  locals.front().front().insert(key, value);
}

Variant& Environment::get(const String& key)
{
  for (Map<Variant>& scope : locals.front()) {
    Variant* result = scope.find(key);
    if (result != nullptr) {
      return *result;
    }
  }
  return globals.at(key);
}

void Environment::push_block_scope()
{
  locals.front().push_front(Map<Variant>());
}

void Environment::push_func_scope(const String& file_name, const Token& token)
{
  locals.push_front(List<Map<Variant>>());
  push_block_scope();
  call_stack.push_front(std::pair<const String&, const Token&>(*curr_file, token));
  curr_file = &file_name;
}

void Environment::push_incl_scope(const String& file_name, const Token& token)
{
  call_stack.push_front(std::pair<const String&, const Token&>(*curr_file, token));
  curr_file = &file_name;
}

void Environment::pop_block_scope()
{
  locals.front().pop_front();
}

void Environment::pop_func_scope()
{
  locals.pop_front();
  curr_file = &call_stack.front().first;
  call_stack.pop_front();
}

void Environment::pop_incl_scope()
{
  curr_file = &call_stack.front().first;
  call_stack.pop_front();
}

void Environment::report(const Semantic_error& error) const
{
  String message = *curr_file + ":" + error.message + "\n";
  for (const std::pair<const String&, const Token&>& call : call_stack) {
    message += "from " + call.first + ":" + to_string(call.second.line) + ":" + to_string(call.second.column) + "\n";
  }
  std::cerr << message.data();
}
