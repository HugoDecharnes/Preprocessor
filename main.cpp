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

#include "main.hpp"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main(int argc, char* argv[])
{
  String message = "Preprocessor v1.0.0. Copyright (C) 2020, Hugo Decharnes, Bryan Aggoun. All rights reserved.\n";
  std::cout << message.data();

  Map<Context> context_list;
  for (unsigned int arg = 1; arg < (unsigned int)argc; arg++) {
    String file_name = argv[arg];
    String file_extension = file_name.extension();
    if (file_extension == ".pp" || file_extension == ".pph") {
      context_list.emplace(file_name);
    }
    else {
      String message = "warning: skipping " + file_name + " due to file extension; use '.pp' for source files and '.pph' for headers\n";
      std::cout << message.data();
    }
  }

  const unsigned int hard_concur = 1;
  const unsigned int thread_count = MIN(hard_concur, context_list.get_size());
  std::thread* thread_list = new std::thread[thread_count];

  for (unsigned int thread_id = 0; thread_id < thread_count; thread_id++) {
    thread_list[thread_id] = std::thread(compile, context_list.get_size(), thread_count, thread_id, std::ref(context_list));
  }
  for (unsigned int thread_id = 0; thread_id < thread_count; thread_id++) {
    thread_list[thread_id].join();
  }

  for (unsigned int thread_id = 0; thread_id < thread_count; thread_id++) {
    thread_list[thread_id] = std::thread(generate, context_list.get_size(), thread_count, thread_id, std::ref(context_list));
  }
  for (unsigned int thread_id = 0; thread_id < thread_count; thread_id++) {
    thread_list[thread_id].join();
  }

  delete[] thread_list;
  std::cout << "info: finished\n";
  return 0;
}

#undef MAX
#undef MIN
