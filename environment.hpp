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

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

class Environment;

#include "exception.hpp"
#include "filesystem.hpp"
#include "list.hpp"
#include "map.hpp"
#include "string.hpp"
#include "utility.hpp"
#include "variant.hpp"

class Environment {
public:
  Environment(const Path& file_name);
  ~Environment();

  void put_global(const String& key, const Variant& value);
  void put_local(const String& key, const Variant& value);

  Variant& get(const String& key);

  void push_block_scope();
  void push_func_scope(const Path& file_name, const Token& token);
  void push_incl_scope(const Path& file_name, const Token& token);
  void pop_block_scope();
  void pop_func_scope();
  void pop_incl_scope();

  void report(const Semantic_error& error) const;

private:
  List<Map<String, Variant>> locals;
  Map<String, Variant> globals;

  Path curr_file;
  List<Pair<const Path, const Token>> call_stack;
};

#endif // ENVIRONMENT_HPP
