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

Environment::Environment(const Path& file_name)
  : curr_file(file_name)
{
  locals.push_front(Map<String, Variant>());
  push_block_scope();
}

Environment::~Environment()
{
}

void Environment::put_global(const String& key, const Variant& value)
{
  globals.insert(Pair<String, Variant>(key, value));
}

void Environment::put_local(const String& key, const Variant& value)
{
  locals.front().insert(Pair<String, Variant>(key, value));
}

Variant& Environment::get(const String& key)
{
  for (Map<String, Variant>& scope : locals) {
    Map<String, Variant>::iterator result = scope.find(key);
    if (result != scope.end()) {
      return result->second;
    }
  }
  return globals.at(key);
}

void Environment::push_block_scope()
{
  locals.push_front(Map<String, Variant>());
}

void Environment::push_func_scope(const Path& file_name, const Token& token)
{
  push_block_scope();
  call_stack.push_front(Pair<const Path&, const Token&>(curr_file, token));
  curr_file = file_name;
}

void Environment::push_incl_scope(const Path& file_name, const Token& token)
{
  call_stack.push_front(Pair<const Path&, const Token&>(curr_file, token));
  curr_file = file_name;
}

void Environment::pop_block_scope()
{
  locals.pop_front();
}

void Environment::pop_func_scope()
{
  pop_block_scope();
  curr_file = call_stack.front().first;
  call_stack.pop_front();
}

void Environment::pop_incl_scope()
{
  curr_file = call_stack.front().first;
  call_stack.pop_front();
}

void Environment::report(const Semantic_error& error) const
{
  String message = curr_file.string() + ":" + error.message + "\n";
  for (const Pair<const Path&, const Token&>& call : call_stack) {
    message += "from " + call.first.string() + ":" + std::to_string(call.second.line) + ":" + std::to_string(call.second.column)
      + "\n";
  }
  std::cerr << message.data();
}
