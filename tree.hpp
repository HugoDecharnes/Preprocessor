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

#ifndef TREE_HPP
#define TREE_HPP

#include <utility>

#include "string.hpp"
#include "token.hpp"
#include "variant.hpp"

class Visitor;

////////////////////////////////////////////////////////// BASE CLASSES //////////////////////////////////////////////////////////

class Statement {
public:
  Statement();
  Statement(Statement&) = default;
  virtual ~Statement();
  virtual void evaluate(Visitor* visitor) = 0;
};

class Directive : public Statement {
public:
  Directive(const Token& token);
  ~Directive();
  Token token;
};

class Expression {
public:
  explicit Expression(const Token& token);
  virtual ~Expression();
  Token token;
  virtual Variant evaluate(Visitor* visitor) = 0;
};

class Binary_expr : public Expression {
public:
  Binary_expr(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Binary_expr();
  Expression* const left_expr;
  Expression* const right_expr;
};

class Unary_expr : public Expression {
public:
  Unary_expr(const Token& token, Expression* expression);
  ~Unary_expr();
  Expression* const expression;
};

class Primary_expr : public Expression {
public:
  explicit Primary_expr(const Token& token);
  ~Primary_expr();
};

class Location : public Expression {
public:
  Location(const Token& token);
  ~Location();
  virtual Variant& reference(Visitor* visitor) = 0;
};

class Storage : public Location {
public:
  Storage(const Token& token);
  ~Storage();
  virtual void global_define(Visitor* visitor, const Variant& value) = 0;
  virtual void local_define(Visitor* visitor, const Variant& value) = 0;
};

/////////////////////////////////////////////////////// EXPRESSION CLASSES ///////////////////////////////////////////////////////

class Ternary : public Expression {
public:
  Ternary(const Token& token, Expression* condition, Expression* true_branch, Expression* false_branch);
  ~Ternary();
  Expression* condition;
  Expression* true_branch;
  Expression* false_branch;
  Variant evaluate(Visitor* visitor) override;
};

class Logical_or : public Binary_expr {
public:
  Logical_or(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Logical_or();
  Variant evaluate(Visitor* visitor) override;
};

class Logical_and : public Binary_expr {
public:
  Logical_and(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Logical_and();
  Variant evaluate(Visitor* visitor) override;
};

class Bitwise_or : public Binary_expr {
public:
  Bitwise_or(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Bitwise_or();
  Variant evaluate(Visitor* visitor) override;
};

class Bitwise_xor : public Binary_expr {
public:
  Bitwise_xor(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Bitwise_xor();
  Variant evaluate(Visitor* visitor) override;
};

class Bitwise_and : public Binary_expr {
public:
  Bitwise_and(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Bitwise_and();
  Variant evaluate(Visitor* visitor) override;
};

class Equal : public Binary_expr {
public:
  Equal(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Equal();
  Variant evaluate(Visitor* visitor) override;
};

class Not_equal : public Binary_expr {
public:
  Not_equal(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Not_equal();
  Variant evaluate(Visitor* visitor) override;
};

class Strict_super : public Binary_expr {
public:
  Strict_super(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Strict_super();
  Variant evaluate(Visitor* visitor) override;
};

class Loose_super : public Binary_expr {
public:
  Loose_super(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Loose_super();
  Variant evaluate(Visitor* visitor) override;
};

class Strict_infer : public Binary_expr {
public:
  Strict_infer(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Strict_infer();
  Variant evaluate(Visitor* visitor) override;
};

class Loose_infer : public Binary_expr {
public:
  Loose_infer(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Loose_infer();
  Variant evaluate(Visitor* visitor) override;
};

class Inside : public Binary_expr {
public:
  Inside(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Inside();
  Variant evaluate(Visitor* visitor) override;
};

class Left_shift : public Binary_expr {
public:
  Left_shift(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Left_shift();
  Variant evaluate(Visitor* visitor) override;
};

class Right_shift : public Binary_expr {
public:
  Right_shift(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Right_shift();
  Variant evaluate(Visitor* visitor) override;
};

class Addition : public Binary_expr {
public:
  Addition(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Addition();
  Variant evaluate(Visitor* visitor) override;
};

class Subtraction : public Binary_expr {
public:
  Subtraction(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Subtraction();
  Variant evaluate(Visitor* visitor) override;
};

class Multiplication : public Binary_expr {
public:
  Multiplication(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Multiplication();
  Variant evaluate(Visitor* visitor) override;
};

class Division : public Binary_expr {
public:
  Division(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Division();
  Variant evaluate(Visitor* visitor) override;
};

class Modulo : public Binary_expr {
public:
  Modulo(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Modulo();
  Variant evaluate(Visitor* visitor) override;
};

class Exponentiation : public Binary_expr {
public:
  Exponentiation(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Exponentiation();
  Variant evaluate(Visitor* visitor) override;
};

class Unary_plus : public Unary_expr {
public:
  Unary_plus(const Token& token, Expression* expression);
  ~Unary_plus();
  Variant evaluate(Visitor* visitor) override;
};

class Unary_minus : public Unary_expr {
public:
  Unary_minus(const Token& token, Expression* expression);
  ~Unary_minus();
  Variant evaluate(Visitor* visitor) override;
};

class Bitwise_not : public Unary_expr {
public:
  Bitwise_not(const Token& token, Expression* expression);
  ~Bitwise_not();
  Variant evaluate(Visitor* visitor) override;
};

class Logical_not : public Unary_expr {
public:
  Logical_not(const Token& token, Expression* expression);
  ~Logical_not();
  Variant evaluate(Visitor* visitor) override;
};

class Interpolate : public Unary_expr {
public:
  Interpolate(const Token& token, Expression* expression);
  ~Interpolate();
  Variant evaluate(Visitor* visitor) override;
};

class Log2_bif : public Unary_expr {
public:
  Log2_bif(const Token& token, Expression* expression);
  ~Log2_bif();
  Variant evaluate(Visitor* visitor) override;
};

class Size_bif : public Unary_expr {
public:
  Size_bif(const Token& token, Expression* expression);
  ~Size_bif();
  Variant evaluate(Visitor* visitor) override;
};

class Integer : public Primary_expr {
public:
  explicit Integer(const Token& token);
  ~Integer();
  Variant evaluate(Visitor* visitor) override;
};

class True_const : public Primary_expr {
public:
  explicit True_const(const Token& token);
  ~True_const();
  Variant evaluate(Visitor* visitor) override;
};

class False_const : public Primary_expr {
public:
  explicit False_const(const Token& token);
  ~False_const();
  Variant evaluate(Visitor* visitor) override;
};

class String_literal : public Primary_expr {
public:
  explicit String_literal(const Token& token);
  ~String_literal();
  Variant evaluate(Visitor* visitor) override;
};

class Escape_seq : public Primary_expr {
public:
  explicit Escape_seq(const Token& token);
  ~Escape_seq();
  Variant evaluate(Visitor* visitor) override;
};

class Quotation : public Expression {
public:
  Quotation(const Token& token, List<Expression*>* expr_list);
  ~Quotation();
  List<Expression*>* const expr_list;
  Variant evaluate(Visitor* visitor) override;
};

class Array : public Expression {
public:
  Array(const Token& token, List<std::pair<Expression*, Expression*>>* range_list);
  ~Array();
  List<std::pair<Expression*, Expression*>>* range_list;
  Variant evaluate(Visitor* visitor) override;
};

class Dictionary : public Expression {
public:
  Dictionary(const Token& token, List<std::pair<Expression*, Expression*>>* elements);
  ~Dictionary();
  List<std::pair<Expression*, Expression*>>* elements;
  Variant evaluate(Visitor* visitor) override;
};

class Macro_call : public Expression {
public:
  Macro_call(const Token& token, Expression* left_expr, List<Expression*>* expr_list);
  ~Macro_call();
  Expression* const left_expr;
  List<Expression*>* const expr_list;
  Variant evaluate(Visitor* visitor) override;
};

class Subscript : public Location {
public:
  Subscript(const Token& token, Expression* left_expr, Expression* right_expr);
  ~Subscript();
  Expression* const left_expr;
  Expression* const right_expr;
  Variant& reference(Visitor* visitor) override;
  Variant evaluate(Visitor* visitor) override;
};

class Identifier : public Storage {
public:
  explicit Identifier(const Token& token);
  ~Identifier();
  void global_define(Visitor* visitor, const Variant& value) override;
  void local_define(Visitor* visitor, const Variant& value) override;
  Variant& reference(Visitor* visitor) override;
  Variant evaluate(Visitor* visitor) override;
};

class Indirection : public Storage {
public:
  Indirection(const Token& token, Expression* expression);
  ~Indirection();
  Expression* const expression;
  void global_define(Visitor* visitor, const Variant& value) override;
  void local_define(Visitor* visitor, const Variant& value) override;
  Variant& reference(Visitor* visitor) override;
  Variant evaluate(Visitor* visitor) override;
};

/////////////////////////////////////////////////////// STATEMENT CLASSES ////////////////////////////////////////////////////////

class Compound : public Statement {
public:
  Compound(List<Statement*>* stmt_list);
  ~Compound();
  List<Statement*>* stmt_list;
  void evaluate(Visitor* visitor) override;
};

class Plain_text : public Statement {
public:
  Plain_text(const Token& token);
  ~Plain_text();
  Token token;
  void evaluate(Visitor* visitor) override;
};

class Block : public Directive {
public:
  Block(const Token& token, Statement* statement);
  ~Block();
  Statement* const statement;
  void evaluate(Visitor* visitor) override;
};

class Expr_stmt : public Directive {
public:
  Expr_stmt(const Token& token, Expression* expression);
  ~Expr_stmt();
  Expression* const expression;
  void evaluate(Visitor* visitor) override;
};

class Local_var_def : public Directive {
public:
  Local_var_def(const Token& token, Storage* storage, Expression* expression);
  ~Local_var_def();
  Storage* const storage;
  Expression* const expression;
  void evaluate(Visitor* visitor) override;
};

class Global_var_def : public Directive {
public:
  Global_var_def(const Token& token, Storage* storage, Expression* expression);
  ~Global_var_def();
  Storage* const storage;
  Expression* const expression;
  void evaluate(Visitor* visitor) override;
};

class Macro {
public:
  Macro(const String& file_name, List<Identifier*>* parameters, Statement* statement);
  ~Macro();
  const String& file_name;
  List<Identifier*>* const parameters;
  Statement* const statement;
};

class Macro_def : public Directive {
public:
  Macro_def(const Token& token, Storage* storage, Macro* macro);
  ~Macro_def();
  Storage* const storage;
  Macro* const macro;
  void evaluate(Visitor* visitor) override;
};

class Func_return : public Directive {
public:
  Func_return(const Token& token, Expression* expression);
  ~Func_return();
  Expression* const expression;
  void evaluate(Visitor* visitor) override;
};

class Mutate : public Directive {
  public:
  Mutate(const Token& token, Location* location, Expression* expression);
  ~Mutate();
  Location* const location;
  Expression* const expression;
  void evaluate(Visitor* visitor) override;
};

class Accumulation : public Directive {
  public:
  Accumulation(const Token& token, Location* location, Expression* expression);
  ~Accumulation();
  Location* const location;
  Expression* const expression;
  void evaluate(Visitor* visitor) override;
};

class Selection : public Directive {
public:
  Selection(const Token& token, List<std::pair<Expression*, Statement*>>* alternatives);
  ~Selection();
  List<std::pair<Expression*, Statement*>>* alternatives;
  void evaluate(Visitor* visitor) override;
};

class Iteration : public Directive {
public:
  Iteration(const Token& token, Storage* key_storage, Storage* val_storage, Expression* expression, Statement* statement);
  ~Iteration();
  Storage* const key_storage;
  Storage* const val_storage;
  Expression* const expression;
  Statement* const statement;
  void evaluate(Visitor* visitor) override;
};

class Inclusion : public Directive {
public:
  Inclusion(const Token& token, Expression* expression);
  ~Inclusion();
  Expression* expression;
  void evaluate(Visitor* visitor) override;
};

#endif // TREE_HPP
