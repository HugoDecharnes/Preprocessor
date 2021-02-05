// Copyright (C) 2020-2021, Hugo Decharnes. All rights reserved.
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

#include "main.hpp"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main(int argc, char* argv[])
{
  String message = "Preprocessor version 1.0.0 (build date: ";
  message += __DATE__;
  message += ").\n";
  std::cout << message.data();

  Vector<Context> context_list;
  for (uint arg = 1; arg < (uint)argc; arg++) {
    Path file_name = argv[arg];
    Path file_extension = file_name.extension();
    if (file_extension == ".src" || file_extension == ".dat") {
      Context context(file_name);
      context_list.push_back(context);
    }
    else {
      String message = "warning: skipping " + file_name.string() + " due to file extension; use '.src' for source files and '.dat' for headers\n";
      std::cout << message.data();
    }
  }

  const uint hard_concur = 1;
  const uint context_size = context_list.size();
  const uint thread_count = MIN(hard_concur, context_size);
  Thread* thread_list = new Thread[thread_count];

  for (uint thread_id = 0; thread_id < thread_count; thread_id++) {
    thread_list[thread_id] = Thread(compile, context_size, thread_count, thread_id, std::ref(context_list));
  }
  for (uint thread_id = 0; thread_id < thread_count; thread_id++) {
    thread_list[thread_id].join();
  }

  for (uint thread_id = 0; thread_id < thread_count; thread_id++) {
    thread_list[thread_id] = Thread(generate, context_size, thread_count, thread_id, std::ref(context_list));
  }
  for (uint thread_id = 0; thread_id < thread_count; thread_id++) {
    thread_list[thread_id].join();
  }

  delete[] thread_list;
  std::cout << "info: finished\n";
  return 0;
}

#undef MAX
#undef MIN
