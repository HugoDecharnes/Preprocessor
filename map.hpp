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

#ifndef MAP_HPP
#define MAP_HPP

#include <utility>
#include "exception.hpp"
#include "string.hpp"

//////////////////////////////////////////////////////////// CORE CLASS ////////////////////////////////////////////////////////////

template<class T>
class Map {
private:
  class Node {
  public:
    Node(const String& key, const T& value);
    ~Node();

    std::pair<String, T> entry;
    Map<T>::Node* left;
    Map<T>::Node* right;
    unsigned int weight;
  };

  Map<T>::Node* root_node;
  unsigned int size;

  T& emplace(Map<T>::Node*& node, const String& key);
  T& insert(Map<T>::Node*& node, const String& key, const T& value);

  T& at(Map<T>::Node* const node, const String& key) const;
  T* find(Map<T>::Node* const node, const String& key) const;
  std::pair<String, T>& at(Map<T>::Node* const& node, unsigned int index) const;

  void erase(Map<T>::Node*& node, const String& key);

  void swap(Map<T>::Node*& node_1, Map<T>::Node*& node_2);

  void lr_balance(Map<T>::Node*& node);
  void rl_balance(Map<T>::Node*& node);
  void left_rotate(Map<T>::Node*& node);
  void right_rotate(Map<T>::Node*& node);

  Map<T>::Node*& min_value(Map<T>::Node*& node) const;

public:
  Map();
  Map(const Map<T>& map);
  ~Map();

  Map<T>& operator+=(const Map<T>& rhs);

  Map<T> operator+(const Map<T>& rhs) const;

  T& emplace(const String& key);
  T& insert(const String& key, const T& value);

  T& at(const String& key) const;
  T* find(const String& key) const;
  std::pair<String, T>& at(unsigned int index) const;

  void erase(const String& key);

  unsigned int get_size() const;
};

#endif // MAP_HPP
