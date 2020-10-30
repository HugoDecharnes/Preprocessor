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

#include "list.hpp"

/////////////////////////////////////////////////////////////// LIST ///////////////////////////////////////////////////////////////

template<class T>
List<T>::List()
  : head(nullptr), tail(nullptr), size(0)
{
}

template<class T>
List<T>::List(const List<T>& rhs)
  : head(nullptr), tail(nullptr), size(0)
{
  for (const T& data : rhs) {
    push_back(data);
  }
}

template<class T>
List<T>::~List()
{
  clear();
}

template<class T>
List<T>& List<T>::operator=(const List<T>& rhs)
{
  clear();
  for (const T& data : rhs) {
    push_back(data);
  }
  return *this;
}

template<class T>
List<T>& List<T>::operator+=(const List<T>& rhs)
{
  for (const T& data : rhs) {
    push_back(data);
  }
  return *this;
}

template<class T>
List<T> List<T>::operator+(const List<T>& rhs) const
{
  List<T> result(*this);
  result += rhs;
  return result;
}

template<class T>
void List<T>::push_front(const T& data)
{
  List<T>::Node* new_node = new List<T>::Node(data);
  if (head != nullptr) {
    new_node->next = head;
    head = new_node;
  }
  else {
    head = tail = new_node;
  }
  size++;
}

template<class T>
void List<T>::push_back(const T& data)
{
  List<T>::Node* new_node = new List<T>::Node(data);
  if (head != nullptr) {
    tail = tail->next = new_node;
  }
  else {
    head = tail = new_node;
  }
  size++;
}

template<class T>
void List<T>::pop_front()
{
  if (head != nullptr) {
    List<T>::Node* prev_node = head;
    head = head->next;
    delete prev_node;
    size--;
  }
  else {
    String message = "attempting to pop an empty list";
    throw Out_of_range(message);
  }
}

template<class T>
void List<T>::clear()
{
  List<T>::Node* curr_node = head;
  while (curr_node != nullptr) {
    List<T>::Node* next_node = curr_node->next;
    delete curr_node;
    curr_node = next_node;
  }
  head = tail = nullptr;
  size = 0;
}

template<class T>
bool List<T>::empty() const
{
  return head == nullptr;
}

template<class T>
T& List<T>::front() const
{
  return head->data;
}

template<class T>
T& List<T>::back() const
{
  return tail->data;
}

template<class T>
T& List<T>::at(unsigned int rhs) const
{
  if (rhs < size) {
    List<T>::Node* node = head;
    for (unsigned int i = 0; i < rhs; i++) {
      node = node->next;
    }
    return node->data;
  }
  else {
    String message = "list index '" + to_string(rhs) + "' is out of range";
    throw Out_of_range(message);
  }
}

template<class T>
typename List<T>::Iterator List<T>::begin() const
{
  return List<T>::Iterator(head);
}

template<class T>
typename List<T>::Iterator List<T>::end() const
{
  return List<T>::Iterator();
}

/////////////////////////////////////////////////////////////// LIST ///////////////////////////////////////////////////////////////

template<class T>
List<T>::Node::Node(const T& data)
  : data(data), next(nullptr)
{
}

template<class T>
List<T>::Node::~Node()
{
}

///////////////////////////////////////////////////////////// ITERATOR /////////////////////////////////////////////////////////////

template<class T>
List<T>::Iterator::Iterator()
  : node(nullptr)
{
}

template<class T>
List<T>::Iterator::Iterator(List<T>::Node* node)
  : node(node)
{
}

template<class T>
T& List<T>::Iterator::operator*()
{
  return node->data;
}

template<class T>
T& List<T>::Iterator::operator->()
{
  return node->data;
}

template<class T>
typename List<T>::Iterator& List<T>::Iterator::operator++()
{
  node = node->next;
  return *this;
}

template<class T>
typename List<T>::Iterator List<T>::Iterator::operator++(int)
{
  List<T>::Iterator iter = *this;
  ++*this;
  return iter;
}

template<class T>
bool List<T>::Iterator::operator==(const List<T>::Iterator& rhs) const
{
  return node == rhs.node;
}

template<class T>
bool List<T>::Iterator::operator!=(const List<T>::Iterator& rhs) const
{
  return node != rhs.node;
}

//////////////////////////////////////////////////////////// INSTANCES /////////////////////////////////////////////////////////////

#include "map.hpp"
#include "string.hpp"
#include "token.hpp"
#include "tree.hpp"
#include "variant.hpp"

template class List<std::pair<Expression*, Statement*>>;
template class List<std::pair<Expression*, Expression*>>;
template class List<std::pair<Identifier*, Variant>>;

template class List<Statement*>;
template class List<Expression*>;
template class List<Identifier*>;

template class List<String>;
template class List<Variant>;

template class List<Map<Variant>>;

template class List<List<Map<Variant>>>;
template class List<std::pair<const String&, const Token&>>;
