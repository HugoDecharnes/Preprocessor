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

#include "context.hpp"

Context::Context()
  : char_stream(nullptr), parse_tree(nullptr)
{
}

Context::~Context()
{
  delete parse_tree;
  delete[] char_stream;
}

void compile(unsigned int argc, unsigned int thread_count, unsigned int thread_id, Map<Context>& context_list)
{
  for (unsigned int index = thread_id; index < argc; index += thread_count) {
    try {
      std::pair<String, Context>& pair = context_list.at(index);
      String& file_name = pair.first;
      char*& char_stream = pair.second.char_stream;
      Statement*& parse_tree = pair.second.parse_tree;

      std::ifstream file_in(file_name.data());
      if (file_in.is_open()) {
        file_in.seekg(0, std::ios::end);
        size_t length = file_in.tellg();
        if (length == (size_t)(-1)) {
          String message = "error: cannot read " + file_name;
          throw Runtime_error(message);
        }
        file_in.seekg(0, std::ios::beg);
        char_stream = new char[length + 1];
        file_in.read(char_stream, length);
        char_stream[length] = '\0';
        file_in.close();

        Lexer lexer((const char*)char_stream);
        Parser parser(file_name, lexer);
        String message = "info: compiling " + file_name + "\n";
        std::cout << message.data();
        parse_tree = parser.parse();
      }
      else {
        String message = "error: cannot open " + file_name;
        throw Runtime_error(message);
      }
    }
    catch (const Runtime_error& error) {
      String message = error.message + "\n";
      std::cerr << message.data();
    }
  }
}

void generate(unsigned int argc, unsigned int thread_count, unsigned int thread_id, Map<Context>& context_list)
{
  for (unsigned int index = thread_id; index < argc; index += thread_count) {
    try {
      std::pair<String, Context>& pair = context_list.at(index);
      String& file_name = pair.first;
      char*& char_stream = pair.second.char_stream;
      Statement*& parse_tree = pair.second.parse_tree;

      String extension = file_name.extension();
      if (extension == ".pp") {
        String out_file_name = file_name;
        out_file_name.remove_extension();

        if (parse_tree != nullptr) {
          Environment environment(file_name);
          Visitor visitor(file_name, parse_tree, environment, context_list);
          String message = "info: generating " + out_file_name + "\n";
          std::cout << message.data();
          String out_string = visitor.visit();

          const char* out_stream = out_string.data();
          std::ofstream file_out(out_file_name.data());
          if (file_out.is_open()) {
            file_out << out_stream;
            file_out.close();
          }
          else {
            String message = "error: cannot create " + out_file_name;
            throw Runtime_error(message);
          }
        }
        else {
          String message = "info: skipping " + out_file_name + " due to previous error(s)";
          throw Runtime_error(message);
        }
      }
    }
    catch (const Runtime_error& error) {
      String message = error.message + "\n";
      std::cerr << message.data();
    }
  }
}
