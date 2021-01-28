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

Context::Context(Path& file_path)
  : file_path(file_path), input_stream(nullptr), parse_tree(nullptr)
{
}

Context::~Context()
{
  delete parse_tree;
  delete[] input_stream;
}

void compile(uint argc, uint thread_count, uint thread_id, Vector<Context>& context_list)
{
  for (uint index = thread_id; index < argc; index += thread_count) {
    try {
      Context& context = context_list.at(index);
      Path& file_path = context.file_path;
      char*& input_stream = context.input_stream;
      Statement*& parse_tree = context.parse_tree;

      Ifstream file_in(file_path);
      if (file_in.is_open()) {
        file_in.seekg(0, std::ios::end);
        size_t length = file_in.tellg();
        if (length == (size_t)(-1)) {
          String message = "error: cannot read " + file_path.string();
          throw Runtime_error(message);
        }
        file_in.seekg(0, std::ios::beg);
        input_stream = new char[length + 1];
        file_in.read(input_stream, length);
        input_stream[length] = '\0';
        file_in.close();

        Lexer lexer(input_stream);
        Parser parser(file_path, lexer);
        String message = "info: compiling " + file_path.string() + "\n";
        std::cout << message.data();
        parse_tree = parser.parse();
      }
      else {
        String message = "error: cannot open " + file_path.string();
        throw Runtime_error(message);
      }
    }
    catch (const Exception& exception) {
      std::cerr << exception.what() << std::endl;
    }
  }
}

void generate(uint argc, uint thread_count, uint thread_id, Vector<Context>& context_list)
{
  for (uint index = thread_id; index < argc; index += thread_count) {
    try {
      Context& context = context_list.at(index);
      Path& file_path = context.file_path;
      Statement*& parse_tree = context.parse_tree;

      Path extension = file_path.extension();
      if (extension == ".src") {
        Path out_file_path = file_path;
        out_file_path.replace_extension();

        if (parse_tree != nullptr) {
          Environment environment(file_path);
          Visitor visitor(file_path, parse_tree, environment, context_list);
          String message = "info: generating " + out_file_path.string() + "\n";
          std::cout << message.data();
          String output_string = visitor.visit();

          const char* out_stream = output_string.data();
          Ofstream file_out(out_file_path);
          if (file_out.is_open()) {
            file_out << out_stream;
            file_out.close();
          }
          else {
            String message = "error: cannot create " + out_file_path.string();
            throw Runtime_error(message);
          }
        }
        else {
          String message = "info: skipping " + out_file_path.string() + " due to previous error(s)";
          throw Runtime_error(message);
        }
      }
    }
    catch (const Exception& exception) {
      std::cerr << exception.what() << std::endl;
    }
  }
}
