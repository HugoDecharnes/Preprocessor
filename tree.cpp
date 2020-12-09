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

#include "tree.hpp"
#include "visitor.hpp"

///////////////////////////////////////////////////// BASE CLASSES CONSTRUCTOR /////////////////////////////////////////////////////

Statement::Statement()
{
}

Directive::Directive(const Token& token)
  : token(token)
{
}

Expression::Expression(const Token& token)
  : token(token)
{
}

Binary_expr::Binary_expr(const Token& token, Expression* left_expr, Expression* right_expr)
  : Expression(token), left_expr(left_expr), right_expr(right_expr)
{
}

Unary_expr::Unary_expr(const Token& token, Expression* expression)
  : Expression(token), expression(expression)
{
}

Primary_expr::Primary_expr(const Token& token)
  : Expression(token)
{
}

Location::Location(const Token& token)
  : Expression(token)
{
}

Storage::Storage(const Token& token)
  : Location(token)
{
}

Macro::Macro(const Path& file_name, List<Identifier*>* parameters, Statement* statement)
  : file_name(file_name), parameters(parameters), statement(statement)
{
}

////////////////////////////////////////////////// STATEMENT CLASSES CONSTRUCTOR ///////////////////////////////////////////////////

Compound::Compound(List<Statement*>* stmt_list)
  : stmt_list(stmt_list)
{
}

Plain_text::Plain_text(const Token& token)
  : token(token)
{
}

Expr_stmt::Expr_stmt(const Token& token, Expression* expression)
  : Directive(token), expression(expression)
{
}

Local_var_def::Local_var_def(const Token& token, Storage* storage, Expression* expression)
  : Directive(token), storage(storage), expression(expression)
{
}

Global_var_def::Global_var_def(const Token& token, Storage* storage, Expression* expression)
  : Directive(token), storage(storage), expression(expression)
{
}

Macro_def::Macro_def(const Token& token, Storage* storage, Macro* macro)
  : Directive(token), storage(storage), macro(macro)
{
}

Selection::Selection(const Token& token, List<Pair<Expression*, Statement*>>* alternatives)
  : Directive(token), alternatives(alternatives)
{
}

Iteration::Iteration(const Token& token, Storage* storage, Expression* expression, Statement* statement)
  : Directive(token), storage(storage), expression(expression), statement(statement)
{
}

Inclusion::Inclusion(const Token& token, Expression* expression)
  : Directive(token), expression(expression)
{
}

////////////////////////////////////////////////// EXPRESSION CLASSES CONSTRUCTOR //////////////////////////////////////////////////

Ternary::Ternary(const Token& token, Expression* condition, Expression* true_branch,
  Expression* false_branch)
  : Expression(token), condition(condition), true_branch(true_branch), false_branch(false_branch)
{
}

Logical_or::Logical_or(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Logical_and::Logical_and(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Bitwise_or::Bitwise_or(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Bitwise_xor::Bitwise_xor(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Bitwise_and::Bitwise_and(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Equal::Equal(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Not_equal::Not_equal(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Strict_super::Strict_super(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Loose_super::Loose_super(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Strict_infer::Strict_infer(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Loose_infer::Loose_infer(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Inside::Inside(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Left_shift::Left_shift(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Right_shift::Right_shift(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Addition::Addition(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Subtraction::Subtraction(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Multiplication::Multiplication(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Division::Division(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Modulo::Modulo(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Exponentiation::Exponentiation(const Token& token, Expression* left_expr, Expression* right_expr)
  : Binary_expr(token, left_expr, right_expr)
{
}

Unary_plus::Unary_plus(const Token& token, Expression* expression)
  : Unary_expr(token, expression)
{
}

Unary_minus::Unary_minus(const Token& token, Expression* expression)
  : Unary_expr(token, expression)
{
}

Bitwise_not::Bitwise_not(const Token& token, Expression* expression)
  : Unary_expr(token, expression)
{
}

Logical_not::Logical_not(const Token& token, Expression* expression)
  : Unary_expr(token, expression)
{
}

Interpolate::Interpolate(const Token& token, Expression* expression)
  : Unary_expr(token, expression)
{
}

Log2_bif::Log2_bif(const Token& token, Expression* expression)
  : Unary_expr(token, expression)
{
}

Size_bif::Size_bif(const Token& token, Expression* expression)
  : Unary_expr(token, expression)
{
}


Integer::Integer(const Token& token)
  : Primary_expr(token)
{
}

True_const::True_const(const Token& token)
  : Primary_expr(token)
{
}

False_const::False_const(const Token& token)
  : Primary_expr(token)
{
}

String_literal::String_literal(const Token& token)
  : Primary_expr(token)
{
}

Escape_seq::Escape_seq(const Token& token)
  : Primary_expr(token)
{
}

Quotation::Quotation(const Token& token, List<Expression*>* expr_list)
  : Expression(token), expr_list(expr_list)
{
}

Array::Array(const Token& token, List<Pair<Expression*, Expression*>>* range_list)
  : Expression(token), range_list(range_list)
{
}

Dictionary::Dictionary(const Token& token, List<Pair<Expression*, Expression*>>* elements)
  : Expression(token), elements(elements)
{
}

Macro_call::Macro_call(const Token& token, Expression* left_expr, List<Expression*>* expr_list)
  : Expression(token), left_expr(left_expr), expr_list(expr_list)
{
}

Subscript::Subscript(const Token& token, Expression* left_expr, Expression* right_expr)
  : Location(token), left_expr(left_expr), right_expr(right_expr)
{
}

Identifier::Identifier(const Token& token)
  : Storage(token)
{
}

Indirection::Indirection(const Token& token, Expression* expression)
  : Storage(token), expression(expression)
{
}

///////////////////////////////////////////////////// BASE CLASSES DESTRUCTOR //////////////////////////////////////////////////////

Statement::~Statement()
{
}

Directive::~Directive()
{
}

Expression::~Expression()
{
}

Binary_expr::~Binary_expr()
{
  delete left_expr;
  delete right_expr;
}

Unary_expr::~Unary_expr()
{
  delete expression;
}

Primary_expr::~Primary_expr()
{
}

Location::~Location()
{
}

Storage::~Storage()
{
}

Macro::~Macro()
{
  for (Identifier* parameter : *parameters) {
    delete parameter;
  }
  delete parameters;
  delete statement;
}

/////////////////////////////////////////////////// STATEMENT CLASSES DESTRUCTOR ///////////////////////////////////////////////////

Compound::~Compound()
{
  for (Statement*& statement : *stmt_list) {
    delete statement;
  }
  delete stmt_list;
}

Plain_text::~Plain_text()
{
}

Expr_stmt::~Expr_stmt()
{
  delete expression;
}

Local_var_def::~Local_var_def()
{
  delete storage;
  delete expression;
}

Global_var_def::~Global_var_def()
{
  delete storage;
  delete expression;
}

Macro_def::~Macro_def()
{
  delete storage;
  delete macro;
}

Selection::~Selection()
{
  for (Pair<Expression*, Statement*>& alternative : *alternatives) {
    delete alternative.first;
    delete alternative.second;
  }
  delete alternatives;
}

Iteration::~Iteration()
{
  delete storage;
  delete expression;
  delete statement;
}

Inclusion::~Inclusion()
{
  delete expression;
}

////////////////////////////////////////////////// EXPRESSION CLASSES DESTRUCTOR ///////////////////////////////////////////////////

Ternary::~Ternary()
{
  delete condition;
  delete true_branch;
  delete false_branch;
}

Logical_or::~Logical_or()
{
}

Logical_and::~Logical_and()
{
}

Bitwise_or::~Bitwise_or()
{
}

Bitwise_xor::~Bitwise_xor()
{
}

Bitwise_and::~Bitwise_and()
{
}

Equal::~Equal()
{
}

Not_equal::~Not_equal()
{
}

Strict_super::~Strict_super()
{
}

Loose_super::~Loose_super()
{
}

Strict_infer::~Strict_infer()
{
}

Loose_infer::~Loose_infer()
{
}

Inside::~Inside()
{
}

Left_shift::~Left_shift()
{
}

Right_shift::~Right_shift()
{
}

Addition::~Addition()
{
}

Subtraction::~Subtraction()
{
}

Multiplication::~Multiplication()
{
}

Division::~Division()
{
}

Modulo::~Modulo()
{
}

Exponentiation::~Exponentiation()
{
}

Unary_plus::~Unary_plus()
{
}

Unary_minus::~Unary_minus()
{
}

Bitwise_not::~Bitwise_not()
{
}

Logical_not::~Logical_not()
{
}

Interpolate::~Interpolate()
{
}

Log2_bif::~Log2_bif()
{
}

Size_bif::~Size_bif()
{
}

Integer::~Integer()
{
}

True_const::~True_const()
{
}

False_const::~False_const()
{
}

String_literal::~String_literal()
{
}

Escape_seq::~Escape_seq()
{
}

Quotation::~Quotation()
{
  for (Expression*& expression : *expr_list) {
    delete expression;
  }
  delete expr_list;
}

Array::~Array()
{
  for (Pair<Expression*, Expression*>& range : *range_list) {
    delete range.first;
    delete range.second;
  }
  delete range_list;
}

Dictionary::~Dictionary()
{
  for (Pair<Expression*, Expression*>& range : *elements) {
    delete range.first;
    delete range.second;
  }
  delete elements;
}

Macro_call::~Macro_call()
{
  delete left_expr;
  for (Expression*& expression : *expr_list) {
    delete expression;
  }
  delete expr_list;
}

Identifier::~Identifier()
{
}

Subscript::~Subscript()
{
  delete left_expr;
  delete right_expr;
}

Indirection::~Indirection()
{
  delete expression;
}

/////////////////////////////////////////////////// STATEMENT CLASSES EVALUATION ///////////////////////////////////////////////////

void Compound::evaluate(Visitor* visitor)
{
  visitor->compound(this);
}

void Plain_text::evaluate(Visitor* visitor)
{
  visitor->plain_text(this);
}

void Expr_stmt::evaluate(Visitor* visitor)
{
  visitor->expr_stmt(this);
}

void Local_var_def::evaluate(Visitor* visitor)
{
  visitor->local_var_def(this);
}

void Global_var_def::evaluate(Visitor* visitor)
{
  visitor->global_var_def(this);
}

void Macro_def::evaluate(Visitor* visitor)
{
  visitor->macro_def(this);
}

void Selection::evaluate(Visitor* visitor)
{
  visitor->selection(this);
}

void Iteration::evaluate(Visitor* visitor)
{
  visitor->iteration(this);
}

void Inclusion::evaluate(Visitor* visitor)
{
  visitor->inclusion(this);
}

/////////////////////////////////////////////////// EXPRESSION CLASSES EVALUATION ///////////////////////////////////////////////////

Variant Ternary::evaluate(Visitor* visitor)
{
  return visitor->ternary(this);
}

Variant Logical_or::evaluate(Visitor* visitor)
{
  return visitor->logical_or(this);
}

Variant Logical_and::evaluate(Visitor* visitor)
{
  return visitor->logical_and(this);
}

Variant Bitwise_or::evaluate(Visitor* visitor)
{
  return visitor->bitwise_or(this);
}

Variant Bitwise_xor::evaluate(Visitor* visitor)
{
  return visitor->bitwise_xor(this);
}

Variant Bitwise_and::evaluate(Visitor* visitor)
{
  return visitor->bitwise_and(this);
}

Variant Equal::evaluate(Visitor* visitor)
{
  return visitor->equal(this);
}

Variant Not_equal::evaluate(Visitor* visitor)
{
  return visitor->not_equal(this);
}

Variant Strict_super::evaluate(Visitor* visitor)
{
  return visitor->strict_super(this);
}

Variant Loose_super::evaluate(Visitor* visitor)
{
  return visitor->loose_super(this);
}

Variant Strict_infer::evaluate(Visitor* visitor)
{
  return visitor->strict_infer(this);
}

Variant Loose_infer::evaluate(Visitor* visitor)
{
  return visitor->loose_infer(this);
}

Variant Inside::evaluate(Visitor* visitor)
{
  return visitor->inside(this);
}

Variant Left_shift::evaluate(Visitor* visitor)
{
  return visitor->left_shift(this);
}

Variant Right_shift::evaluate(Visitor* visitor)
{
  return visitor->right_shift(this);
}

Variant Addition::evaluate(Visitor* visitor)
{
  return visitor->addition(this);
}

Variant Subtraction::evaluate(Visitor* visitor)
{
  return visitor->subtraction(this);
}

Variant Multiplication::evaluate(Visitor* visitor)
{
  return visitor->multiplication(this);
}

Variant Division::evaluate(Visitor* visitor)
{
  return visitor->division(this);
}

Variant Modulo::evaluate(Visitor* visitor)
{
  return visitor->modulo(this);
}

Variant Exponentiation::evaluate(Visitor* visitor)
{
  return visitor->exponentiation(this);
}

Variant Unary_plus::evaluate(Visitor* visitor)
{
  return visitor->unary_plus(this);
}

Variant Unary_minus::evaluate(Visitor* visitor)
{
  return visitor->unary_minus(this);
}

Variant Bitwise_not::evaluate(Visitor* visitor)
{
  return visitor->bitwise_not(this);
}

Variant Logical_not::evaluate(Visitor* visitor)
{
  return visitor->logical_not(this);
}

Variant Interpolate::evaluate(Visitor* visitor)
{
  return visitor->interpolate(this);
}

Variant Log2_bif::evaluate(Visitor* visitor)
{
  return visitor->log2_bif(this);
}

Variant Size_bif::evaluate(Visitor* visitor)
{
  return visitor->size_bif(this);
}

Variant Integer::evaluate(Visitor* visitor)
{
  return visitor->integer(this);
}

Variant True_const::evaluate(Visitor* visitor)
{
  return visitor->true_const(this);
}

Variant False_const::evaluate(Visitor* visitor)
{
  return visitor->false_const(this);
}

Variant String_literal::evaluate(Visitor* visitor)
{
  return visitor->string_literal(this);
}

Variant Escape_seq::evaluate(Visitor* visitor)
{
  return visitor->escape_seq(this);
}

Variant Quotation::evaluate(Visitor* visitor)
{
  return visitor->quotation(this);
}

Variant Array::evaluate(Visitor* visitor)
{
  return visitor->array(this);
}

Variant Dictionary::evaluate(Visitor* visitor)
{
  return visitor->dictionary(this);
}

Variant Macro_call::evaluate(Visitor* visitor)
{
  return visitor->macro_call(this);
}

Variant Identifier::evaluate(Visitor* visitor)
{
  return visitor->eval_identifier(this);
}

Variant Subscript::evaluate(Visitor* visitor)
{
  return visitor->eval_subscript(this);
}

Variant Indirection::evaluate(Visitor* visitor)
{
  return visitor->eval_indirection(this);
}

/////////////////////////////////////////////////////// LOCATIONS REFERENCE ////////////////////////////////////////////////////////

Variant& Identifier::reference(Visitor* visitor)
{
  return visitor->ref_identifier(this);
}

Variant& Subscript::reference(Visitor* visitor)
{
  return visitor->ref_subscript(this);
}

Variant& Indirection::reference(Visitor* visitor)
{
  return visitor->ref_indirection(this);
}

////////////////////////////////////////////////////////// STORAGE DEFINE //////////////////////////////////////////////////////////

void Identifier::global_define(Visitor* visitor, const Variant& value)
{
  visitor->global_id_def(this, value);
}

void Identifier::local_define(Visitor* visitor, const Variant& value)
{
  visitor->local_id_def(this, value);
}

void Indirection::global_define(Visitor* visitor, const Variant& value)
{
  visitor->global_ind_def(this, value);
}

void Indirection::local_define(Visitor* visitor, const Variant& value)
{
  visitor->local_ind_def(this, value);
}
