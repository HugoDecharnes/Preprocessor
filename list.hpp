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

#ifndef FWD_LIST_HPP
#define FWD_LIST_HPP

#include <exception>
#include <type_traits>
#include <utility>
#include "exception.hpp"
#include "string.hpp"

template<class T>
class List {
private:
  class Node {
  public:
    Node(const T& data);
    ~Node();

    T data;
    List<T>::Node* next;
  };

  List<T>::Node* head;
  List<T>::Node* tail;

public:
  class Iterator {
  public:
    Iterator();
    Iterator(List<T>::Node* node);

    T& operator*();
    T& operator->();
    
    List<T>::Iterator& operator++();
    List<T>::Iterator operator++(int);

    bool operator==(const List<T>::Iterator& rhs) const;
    bool operator!=(const List<T>::Iterator& rhs) const;

  private:
    List<T>::Node* node;
  };

  List();
  List(const List<T>& rhs);
  ~List();

  unsigned int size;

  List<T>& operator=(const List<T>& rhs);
  List<T>& operator+=(const List<T>& rhs);
  
  List<T> operator+(const List<T>& rhs) const;

  void push_front(const T& data);
  void push_back(const T& data);
  void pop_front();
  void clear();

  bool empty() const;

  T& front() const;
  T& back() const;
  T& at(int rhs) const;

  List<T>::Iterator begin() const;
  List<T>::Iterator end() const;
};

#endif // FWD_LIST_HPP
