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

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

class Context;

#include "environment.hpp"
#include "fstream.hpp"
#include "lexer.hpp"
#include "map.hpp"
#include "parser.hpp"
#include "thread.hpp"
#include "tree.hpp"
#include "utility.hpp"
#include "visitor.hpp"

class Context {
public:
  Context();
  ~Context();
  char* char_stream;
  Statement* parse_tree;
};

void compile(uint argc, uint thread_count, uint thread_id, Map<String, Context>& context_list);
void generate(uint argc, uint thread_count, uint thread_id, Map<String, Context>& context_list);

#endif // CONTEXT_HPP
