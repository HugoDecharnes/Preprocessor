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

#ifndef VISITOR_HPP
#define VISITOR_HPP

class Visitor;

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <iostream>
#include <utility>
#include "context.hpp"
#include "environment.hpp"
#include "exception.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "string.hpp"
#include "tree.hpp"
#include "variant.hpp"

class Visitor {
public:
  Visitor(const String& file_name, Statement* parse_tree, Environment& environment, Map<Context>& context_list);
  ~Visitor();

private:
  const String& file_name;
  Statement* const parse_tree;
  Environment& environment;
  Map<Context>& context_list;
  String out;
  unsigned int error_count;

public:
  String visit();

  void compound(Compound* node);
  void block(Block* node);
  void plain_text(Plain_text* node);
  void expr_stmt(Expr_stmt* node);
  void variable_def(Variable_def* node);
  void function_def(Function_def* node);
  void func_return(Func_return* node);
  void mutate(Mutate* node);
  void accumulation(Accumulation* node);
  void selection(Selection* node);
  void iteration(Iteration* node);
  void inclusion(Inclusion* node);

  Variant ternary(Ternary* node);
  Variant logical_or(Logical_or* node);
  Variant logical_and(Logical_and* node);
  Variant bitwise_or(Bitwise_or* node);
  Variant bitwise_xor(Bitwise_xor* node);
  Variant bitwise_and(Bitwise_and* node);
  Variant equal(Equal* node);
  Variant not_equal(Not_equal* node);
  Variant strict_super(Strict_super* node);
  Variant loose_super(Loose_super* node);
  Variant strict_infer(Strict_infer* node);
  Variant loose_infer(Loose_infer* node);
  Variant inside(Inside* node);
  Variant left_shift(Left_shift* node);
  Variant right_shift(Right_shift* node);
  Variant addition(Addition* node);
  Variant subtraction(Subtraction* node);
  Variant multiplication(Multiplication* node);
  Variant division(Division* node);
  Variant modulo(Modulo* node);
  Variant exponentiation(Exponentiation* node);
  Variant unary_plus(Unary_plus* node);
  Variant unary_minus(Unary_minus* node);
  Variant bitwise_not(Bitwise_not* node);
  Variant logical_not(Logical_not* node);
  Variant interpolate(Interpolate* node);
  Variant log2_bif(Log2_bif* node);
  Variant size_bif(Size_bif* node);
  Variant integer(Integer* node);
  Variant true_const(True_const* node);
  Variant false_const(False_const* node);
  Variant string_literal(String_literal* node);
  Variant escape_seq(Escape_seq* node);
  Variant quotation(Quotation* node);
  Variant array(Array* node);
  Variant dictionary(Dictionary* node);
  Variant function_call(Function_call* node);
  Variant eval_subscript(Subscript* node);
  Variant eval_identifier(Identifier* node);
  Variant eval_indirection(Indirection* node);

  Variant& ref_subscript(Subscript* node);
  Variant& ref_identifier(Identifier* node);
  Variant& ref_indirection(Indirection* node);

  void global_id_def(Identifier* node, const Variant& value);
  void local_id_def(Identifier* node, const Variant& value);
  void global_ind_def(Indirection* node, const Variant& value);
  void local_ind_def(Indirection* node, const Variant& value);

private:
  void report(const Semantic_error& error);
};

/////////////////////////////////////////////////////// EXCEPTION CLASSES ////////////////////////////////////////////////////////

class Return_exc : public std::exception {
public:
  explicit Return_exc(const Variant& result);
  Variant result;
};

#endif // VISITOR_HPP
