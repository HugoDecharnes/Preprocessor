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

#include "map.hpp"

#define WEIGHT(t) ((t) ? (t)->weight : 0)

/////////////////////////////////////////////////////////////// MAP ////////////////////////////////////////////////////////////////

template<class T>
Map<T>::Map()
  : root_node(nullptr), size(0)
{
}

template<class T>
Map<T>::Map(const Map<T>& map)
  : Map()
{
  for (unsigned int index = 0; index < map.size; index++) {
    std::pair<String, T>& entry = map.at(index);
    insert(entry.first, entry.second);
  }
}


template<class T>
Map<T>::~Map()
{
  delete root_node;
}

template<class T>
Map<T>& Map<T>::operator+=(const Map<T>& rhs)
{
  for (unsigned int index = 0; index < rhs.size; index++) {
    std::pair<String, T>& entry = rhs.at(index);
    insert(entry.first, entry.second);
  }
  return *this;
}

template<class T>
Map<T> Map<T>::operator+(const Map<T>& rhs) const
{
  Map<T> result(*this);
  result += rhs;
  return result;
}

template<class T>
T& Map<T>::emplace(const String& key)
{
  return emplace(root_node, key);
}

template<class T>
T& Map<T>::emplace(Map<T>::Node*& node, const String& key)
{
  if (node != nullptr) {
    int comparison = key.compare(node->entry.first);
    if (comparison < 0) {
      T& place = emplace(node->left, key);
      node->weight++;
      lr_balance(node);
      return place;
    }
    else if (comparison > 0) {
      T& place = emplace(node->right, key);
      node->weight++;
      rl_balance(node);
      return place;
    }
    else {
      throw Out_of_range("cannot emplace \"" + key + "\"; key already exists");
    }   
  }
  else {
    node = new typename Map<T>::Node(key, T());
    size++;
    return node->entry.second;
  }
}

template<class T>
T& Map<T>::insert(const String& key, const T& value)
{
  return insert(root_node, key, value);
}

template<class T>
T& Map<T>::insert(Map<T>::Node*& node, const String& key, const T& value)
{
  if (node != nullptr) {
    int comparison = key.compare(node->entry.first);
    if (comparison < 0) {
      T& place = insert(node->left, key, value);
      node->weight++;
      lr_balance(node);
      return place;
    }
    else if (comparison > 0) {
      T& place = insert(node->right, key, value);
      node->weight++;
      rl_balance(node);
      return place;
    }
    else {
      throw Out_of_range("cannot insert \"" + key + "\"; key already exists");
    }   
  }
  else {
    node = new typename Map<T>::Node(key, value);
    size++;
    return node->entry.second;
  }
}

template<class T>
T& Map<T>::at(const String& key) const
{
  return at(root_node, key);
}

template<class T>
T& Map<T>::at(Map<T>::Node* const node, const String& key) const
{
  if (node != nullptr) {
    int comparison = key.compare(node->entry.first);
    if (comparison < 0) {
      return at(node->left, key);
    }
    else if (comparison > 0) {
      return at(node->right, key);
    }
    else {
      return node->entry.second;
    }
  }
  else {
    throw Out_of_range("cannot find \"" + key + "\"; key does not exist");
  }
}

template<class T>
std::pair<String, T>& Map<T>::at(unsigned int index) const
{
  return at(root_node, index + 1);
}

template<class T>
std::pair<String, T>& Map<T>::at(Map<T>::Node* const& node, unsigned int index) const
{
  if (node != nullptr) {
    int comparison = index - WEIGHT(node->left) - 1;
    if (comparison < 0) {
      return at(node->left, index);
    }
    else if (comparison > 0) {
      return at(node->right, comparison);
    }
    else {
      return node->entry;
    }
  }
  else {
    throw Out_of_range("cannot index at \"" + to_string(index) + "\"; entry does not exist");
  }
}

template<class T>
T* Map<T>::find(const String& key) const
{
  return find(root_node, key);
}

template<class T>
T* Map<T>::find(Map<T>::Node* const node, const String& key) const
{
  if (node != nullptr) {
    int comparison = key.compare(node->entry.first);
    if (comparison < 0) {
      return find(node->left, key);
    }
    else if (comparison > 0) {
      return find(node->right, key);
    }
    else {
      return &node->entry.second;
    }
  }
  else {
    return nullptr;
  }
}

template<class T>
void Map<T>::erase(const String& key)
{
  erase(root_node, key);
}

template<class T>
void Map<T>::erase(Map<T>::Node*& node, const String& key)
{
  if (node != nullptr) {
    int comparison = key.compare(node->entry.first);
    if (comparison < 0) {
      erase(node->left, key);
      node->weight--;
      rl_balance(node);
    }
    else if (comparison > 0) {
      erase(node->right, key);
      node->weight--;
      lr_balance(node);
    }
    else {
      if (node->left != nullptr) {
        if (node->right != nullptr) {
          Map<T>::Node*& successor = min_value(node->right);
          swap(node, successor);
          erase(node->right, key);
          node->weight--;
          lr_balance(node);
        }
        else {
          Map<T>::Node* victim = node;
          node = node->left;
          victim->left = nullptr;
          delete victim;
          size--;
        }
      }
      else {
        if (node->right != nullptr) {
          Map<T>::Node* victim = node;
          node = node->right;
          victim->right = nullptr;
          delete victim;
          size--;
        }
        else {
          delete node;
          node = nullptr;
          size--;
        }
      }
    }
  }
  else {
    throw Out_of_range("cannot erase \"" + key + "\"; key does not exist");
  }
}

template<class T>
unsigned int Map<T>::get_size() const
{
  return size;
}

template<class T>
void Map<T>::lr_balance(Map<T>::Node*& node)
{
  if (node != nullptr) {
    if (node->left != nullptr) {
      unsigned int right_weight      = WEIGHT(node->right);
      unsigned int left_left_weight  = WEIGHT(node->left->left);
      unsigned int left_right_weight = WEIGHT(node->left->right);
      if (right_weight < left_right_weight) {
        left_rotate(node->left);
        right_rotate(node);
      }
      else if (right_weight < left_left_weight) {
        right_rotate(node);
      }
    }
  }
}

template<class T>
void Map<T>::rl_balance(Map<T>::Node*& node)
{
  if (node != nullptr) {
    if (node->right != nullptr) {
      unsigned int left_weight        = WEIGHT(node->left);
      unsigned int right_left_weight  = WEIGHT(node->right->left);
      unsigned int right_right_weight = WEIGHT(node->right->right);
      if (left_weight < right_left_weight) {
        right_rotate(node->right);
        left_rotate(node);
      }
      else if (left_weight < right_right_weight) {
        left_rotate(node);
      }
    }
  }
}

template<class T>
void Map<T>::left_rotate(Map<T>::Node*& node)
{
  Map<T>::Node* pivot = node->right;
  node->right = pivot->left;
  pivot->left = node;

  node->weight = WEIGHT(node->left) + WEIGHT(node->right) + 1;
  pivot->weight = node->weight + WEIGHT(pivot->right) + 1;

  node = pivot;
}

template<class T>
void Map<T>::right_rotate(Map<T>::Node*& node)
{
  Map<T>::Node* pivot = node->left;
  node->left = pivot->right;
  pivot->right = node;

  node->weight = WEIGHT(node->left) + WEIGHT(node->right) + 1;
  pivot->weight = WEIGHT(pivot->left) + node->weight + 1;

  node = pivot;
}

template<class T>
typename Map<T>::Node*& Map<T>::min_value(Map<T>::Node*& node) const
{
  if (node->left != nullptr) {
    return min_value(node->left);
  }
  else {
    return node;
  }
}

template<class T>
void Map<T>::swap(Map<T>::Node*& node_1, Map<T>::Node*& node_2)
{
  unsigned int node_1_weight = node_1->weight;
  node_1->weight = node_2->weight;
  node_2->weight = node_1_weight;

  Map<T>::Node* node_1_root = node_1;
  node_1 = node_2;
  node_2 = node_1_root;

  Map<T>::Node* node_1_left = node_1->left;
  node_1->left = node_2->left;
  node_2->left = node_1_left;

  Map<T>::Node* node_1_right = node_1->right;
  node_1->right = node_2->right;
  node_2->right = node_1_right;
}

///////////////////////////////////////////////////////////// MAP NODE /////////////////////////////////////////////////////////////

template<class T>
Map<T>::Node::Node(const String& key, const T& value)
  : entry(key, value), left(nullptr), right(nullptr), weight(1)
{
}

template<class T>
Map<T>::Node::~Node()
{
  delete left;
  delete right;
}

////////////////////////////////////////////////////////// INSTANTIATIONS //////////////////////////////////////////////////////////

#include "context.hpp"
#include "token.hpp"
#include "variant.hpp"

template class Map<Context>;
template class Map<Variant>;
template class Map<Token::Type>;

#undef WEIGTH
