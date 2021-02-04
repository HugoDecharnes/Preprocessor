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

#include "visitor.hpp"

/////////////////////////////////////////////////////////////// RUN ////////////////////////////////////////////////////////////////

Visitor::Visitor(Path& file_path, Statement* parse_tree, Environment& environment, Vector<Context>& context_list)
  : file_path(file_path), parse_tree(parse_tree), environment(environment), context_list(context_list)
{
}

Visitor::~Visitor()
{
}

String Visitor::visit()
{
  parse_tree->evaluate(this);
  uint error_count = environment.get_error_count();
  if (error_count != 0) {
    if (error_count >= 5 && environment.get_call_depth() != 1) {
      String message = file_path.string() + ": " + std::to_string(error_count - 5) + " more error(s)\n";
      std::cerr << message.data();
    }
    String message = file_path.string() + ": generation failed due to " + std::to_string(error_count) + " error(s)";
    throw Runtime_error(message);
  }
  return output_string;
}

//////////////////////////////////////////////////////////// STATEMENTS ////////////////////////////////////////////////////////////

void Visitor::compound(Compound* node)
{
  for (Statement*& statement : *node->stmt_list) {
    statement->evaluate(this);
  }
}

void Visitor::plain_text(Plain_text* node)
{
  output_string.append(node->token.start, node->token.length);
}

void Visitor::expr_stmt(Expr_stmt* node)
{
  try {
    output_string += node->expression->evaluate(this).to_string();
  }
  catch (const Semantic_error& error) {
    report(error);
  }
  catch (const Bad_variant_access& exception) {
    Semantic_error error(node->token, exception.message);
    report(error);
  }
}

void Visitor::global_var_def(Global_var_def* node)
{
  try {
    Variant value = node->expression->evaluate(this);
    node->storage->global_define(this, value);
  }
  catch (const Semantic_error& error) {
    report(error);
  }
}

void Visitor::local_var_def(Local_var_def* node)
{
  try {
    Variant value = node->expression->evaluate(this);
    node->storage->local_define(this, value);
  }
  catch (const Semantic_error& error) {
    report(error);
  }
}

void Visitor::macro_def(Macro_def* node)
{
  try {
    Variant value = node->macro;
    node->storage->global_define(this, value);
  }
  catch (const Semantic_error& error) {
    report(error);
  }
}

void Visitor::selection(Selection* node)
{
  for (Pair<Expression*, Statement*>& alternative : *node->alternatives) {
    Variant condition = alternative.first->evaluate(this);
    if (condition.get_bool()) {
      environment.push_block_scope();
      alternative.second->evaluate(this);
      environment.pop_block_scope();
      return;
    }
  }
}

void Visitor::iteration(Iteration* node)
{
  try {
    Variant value_list = node->expression->evaluate(this);
    const Vector<Variant>& list = value_list.get_array();
    uint index = 0;
    for (const Variant& item : list) {
      environment.push_block_scope();
      environment.put_local("index", index);
      node->storage->local_define(this, item);
      node->statement->evaluate(this);
      environment.pop_block_scope();
      index++;
    }
  }
  catch (const Semantic_error& error) {
    report(error);
  }
  catch (const Exception& exception) {
    String message = exception.what();
    Semantic_error error(node->token, message);
    report(error);
  }
}

void Visitor::inclusion(Inclusion* node)
{
  try {
    Variant value = node->expression->evaluate(this);
    String incl_file_name = value.get_string();
    Path incl_file_path(file_path.parent_path());
    incl_file_path /= incl_file_name;
    Statement* incl_parse_tree = nullptr;
    for (Context& context : context_list) {
      if (context.file_path == incl_file_path) {
        incl_parse_tree = context.parse_tree;
        break;
      }
    }
    if (incl_parse_tree != nullptr) {
      try {
        environment.push_incl_scope(incl_file_path, node->token);
        Visitor visitor(incl_file_path, incl_parse_tree, environment, context_list);
        visitor.visit();
        environment.pop_incl_scope();
      }
      catch (const Runtime_error& exception) {
        environment.pop_incl_scope();
        String message = "failed to include '" + incl_file_name + "' due to previous error(s)";
        throw Semantic_error(node->token, message);
      }
    }
    else {
      String message = "cannot include '" + incl_file_name + "'; file does not exist";
      throw Semantic_error(node->token, message);
    }
  }
  catch (const Bad_variant_access& exception) {
    Semantic_error error(node->token, exception.message);
    report(error);
  }
  catch (const Semantic_error& error) {
    report(error);
  }
}

/////////////////////////////////////////////////////////// EXPRESSIONS ////////////////////////////////////////////////////////////

Variant Visitor::ternary(Ternary* node)
{
  try {
    bool condition = node->condition->evaluate(this).get_bool();
    if (condition) {
      return node->true_branch->evaluate(this);
    }
    else {
      return node->false_branch->evaluate(this);
    }
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::logical_or(Logical_or* node)
{
  try {
    return node->left_expr->evaluate(this) || node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::logical_and(Logical_and* node)
{
  try {
    return node->left_expr->evaluate(this) && node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::bitwise_or(Bitwise_or* node)
{
  try {
    return node->left_expr->evaluate(this) | node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::bitwise_xor(Bitwise_xor* node)
{
  try {
    return node->left_expr->evaluate(this) ^ node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::bitwise_and(Bitwise_and* node)
{
  try {
    return node->left_expr->evaluate(this) & node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::equal(Equal* node)
{
  try {
    return node->left_expr->evaluate(this) == node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::not_equal(Not_equal* node)
{
  try {
    return node->left_expr->evaluate(this) != node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::strict_super(Strict_super* node)
{
  try {
    return node->left_expr->evaluate(this) > node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::loose_super(Loose_super* node)
{
  try {
    return node->left_expr->evaluate(this) >= node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::strict_infer(Strict_infer* node)
{
  try {
    return node->left_expr->evaluate(this) < node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::loose_infer(Loose_infer* node)
{
  try {
    return node->left_expr->evaluate(this) <= node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::inside(Inside* node)
{
  try {
    Variant left_value = node->left_expr->evaluate(this);
    Variant right_val_list = node->right_expr->evaluate(this);
    for (const Variant& right_value : right_val_list.get_array()) {
      Variant comparison = left_value == right_value;
      if (comparison.get_bool()) {
        return true;
      }
    }
    return false;
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::left_shift(Left_shift* node)
{
  try {
    return node->left_expr->evaluate(this) << node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::right_shift(Right_shift* node)
{
  try {
    return node->left_expr->evaluate(this) >> node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::addition(Addition* node)
{
  try {
    return node->left_expr->evaluate(this) + node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::subtraction(Subtraction* node)
{
  try {
    return node->left_expr->evaluate(this) - node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::multiplication(Multiplication* node)
{
  try {
    return node->left_expr->evaluate(this) * node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::division(Division* node)
{
  try {
    return node->left_expr->evaluate(this) / node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::modulo(Modulo* node)
{
  try {
    return node->left_expr->evaluate(this) % node->right_expr->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::exponentiation(Exponentiation* node)
{
  try {
    Variant base = node->left_expr->evaluate(this);
    Variant exponent = node->right_expr->evaluate(this);
    return base.pow(exponent);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::unary_plus(Unary_plus* node)
{
  try {
    return +node->expression->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::unary_minus(Unary_minus* node)
{
  try {
    return -node->expression->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::bitwise_not(Bitwise_not* node)
{
  try {
    return ~node->expression->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::logical_not(Logical_not* node)
{
  try {
    return !node->expression->evaluate(this);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::interpolate(Interpolate* node)
{
  Statement* parse_tree = nullptr;
  try {
    Variant value = node->expression->evaluate(this);
    const char* input_stream = value.get_string().data();
    Lexer lexer(input_stream);
    Parser parser(file_path, lexer);
    parse_tree = parser.parse();
    Visitor visitor(file_path, parse_tree, environment, context_list);
    String output_string = visitor.visit();
    delete parse_tree;
    return output_string;
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
  catch (const Runtime_error& error) {
    delete parse_tree;
    String message = "interpolation failed due to previous errors";
    throw Semantic_error(node->token, message);
  }
}

Variant Visitor::log2_bif(Log2_bif* node)
{
  try {
    Variant value = node->expression->evaluate(this);
    return value.log2();
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::clog2_bif(Clog2_bif* node)
{
  try {
    Variant value = node->expression->evaluate(this);
    return value.clog2();
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::max_bif(Max_bif* node)
{
  try {
    Variant result = INT_MIN;
    for (Expression*& expression : *node->expr_list) {
      Variant value = expression->evaluate(this);
      Variant comparison = value > result;
      if (comparison.get_bool()) {
        result = value;
      }
    }
    return result;
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::min_bif(Min_bif* node)
{
  try {
    Variant result = INT_MAX;
    for (Expression*& expression : *node->expr_list) {
      Variant value = expression->evaluate(this);
      Variant comparison = value < result;
      if (comparison.get_bool()) {
        result = value;
      }
    }
    return result;
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::size_bif(Size_bif* node)
{
  try {
    Variant value = node->expression->evaluate(this);
    return (uint)value.get_array().size();
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::integer(Integer* node)
{
  String string(node->token.start, node->token.length);
  int integer = std::stoi(string);
  return integer;
}

Variant Visitor::true_const(True_const* node)
{
  return Variant(true);
}

Variant Visitor::false_const(False_const* node)
{
  return Variant(false);
}

Variant Visitor::string_literal(String_literal* node)
{
  return Variant(node->token.get_text());
}

Variant Visitor::escape_seq(Escape_seq* node)
{
  switch (*node->token.start) {
  case '\'':
    return Variant(String("\'"));
  case '\"':
    return Variant(String("\""));
  case '\\':
    return Variant(String("\\"));
  case 'a':
    return Variant(String("\a"));
  case 'b':
    return Variant(String("\b"));
  case 'f':
    return Variant(String("\f"));
  case 'n':
    return Variant(String("\n"));
  case 'r':
    return Variant(String("\r"));
  case 't':
    return Variant(String("\t"));
  case 'v':
    return Variant(String("\v"));
  default:
    String message = "unkown escaped character '" + String(node->token.start) + "\'";
    throw Semantic_error(node->token, message);
  }
}

Variant Visitor::quotation(Quotation* node)
{
  Variant string = String();
  for (Expression*& expression : *node->expr_list) {
    string += expression->evaluate(this).to_string();
  }
  return string;
}

Variant Visitor::array(Array* node)
{
  try {
    Vector<Variant> list;
    for (Pair<Expression*, Expression*>& range : *node->range_list) {
      if (range.second != nullptr) {
        int first_value = range.first->evaluate(this).get_int();
        int second_value = range.second->evaluate(this).get_int();
        int step_value;
        int stop_value;
        if (first_value < second_value) {
          step_value = 1;
          stop_value = second_value + 1;
        }
        else {
          step_value = -1;
          stop_value = second_value - 1;
        }
        for (int value = first_value; value != stop_value; value += step_value) {
          list.push_back(Variant(value));
        }
      }
      else {
        list.push_back(range.first->evaluate(this));
      }
    }
    return list;
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::dictionary(Dictionary* node)
{
  try {
    Map<String, Variant> map;
    for (Pair<Expression*, Expression*>& element : *node->elements) {
      Variant key = element.first->evaluate(this);
      Variant value = element.second->evaluate(this);
      map.insert(Pair<String, Variant>(key.get_string(), value));
    }
    return map;
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::macro_call(Macro_call* node)
{
  try {
    Variant base = node->left_expr->evaluate(this);
    Macro* macro = base.get_macro();
    if (node->expr_list->size() == macro->parameters->size()) {
      List<Pair<Identifier*, Variant>> param_value_list;
      List<Identifier*>::iterator param_iter = macro->parameters->begin();
      List<Expression*>::iterator expr_iter = node->expr_list->begin();
      for (; param_iter != macro->parameters->end(); param_iter++, expr_iter++) {
        Variant value = (*expr_iter)->evaluate(this);
        param_value_list.push_back(Pair<Identifier*, Variant>(*param_iter, value));
      }
      environment.push_func_scope(macro->file_path, node->token);
      for (Pair<Identifier*, Variant>& param_value : param_value_list) {
        param_value.first->local_define(this, param_value.second);
      }
      Variant result;
      macro->statement->evaluate(this);
      environment.pop_func_scope();
      return result;
    }
    else {
      String message = "mismatched macro parameters; expecting " + std::to_string(macro->parameters->size()) + " got "
        + std::to_string(node->expr_list->size());
      throw Semantic_error(node->token, message);
    }
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant Visitor::eval_identifier(Identifier* node)
{
  String key = node->token.get_text();
  try {
    return environment.get(key);
  }
  catch (const Out_of_range& error) {
    String message = "cannot find '" + key + "'; identifier undefined";
    throw Semantic_error(node->token, message);
  }
}

Variant Visitor::eval_subscript(Subscript* node)
{
  try {
    String key = node->token.get_text();
    Variant left_value = node->left_expr->evaluate(this);
    Variant right_value = node->right_expr->evaluate(this);
    return left_value[right_value];
  }
  catch (const Out_of_range& error) {
    String message = "index is out of range";
    throw Semantic_error(node->token, message);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }  
}

Variant Visitor::eval_indirection(Indirection* node)
{
  Variant name = node->expression->evaluate(this);
  String key;
  try {
    key = name.get_string();
    return environment.get(key);
  }
  catch (const Out_of_range& error) {
    String message = "cannot find '" + key + "'; identifier undefined";
    throw Semantic_error(node->token, message);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

/////////////////////////////////////////////////////// LOCATIONS REFERENCE ////////////////////////////////////////////////////////

Variant& Visitor::ref_identifier(Identifier* node)
{
  String key = node->token.get_text();
  try {
    return environment.get(key);
  }
  catch (const Out_of_range& error) {
    String message = "cannot find '" + key + "'; identifier undefined";
    throw Semantic_error(node->token, message);
  }
}

Variant& Visitor::ref_subscript(Subscript* node)
{
  String key = node->token.get_text();
  try {
    Location* left_location = (Location*)node->left_expr;
    Variant& left_value = left_location->reference(this);
    Variant right_value = node->right_expr->evaluate(this);
    return left_value[right_value];
  }
  catch (const Out_of_range& error) {
    String message = "index is out of range";
    throw Semantic_error(node->token, message);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

Variant& Visitor::ref_indirection(Indirection* node)
{
  Variant name = node->expression->evaluate(this);
  String key;
  try {
    key = name.get_string();
    return environment.get(key);
  }
  catch (const Out_of_range& error) {
    String message = "cannot find '" + key + "'; identifier undefined";
    throw Semantic_error(node->token, message);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

////////////////////////////////////////////////////////// STORAGE DEFINE //////////////////////////////////////////////////////////

void Visitor::global_id_def(Identifier* node, const Variant& value)
{
  String key = node->token.get_text();
  try {
    environment.put_global(key, value);
  }
  catch (const Out_of_range& error) {
    String message = "cannot define '" + key + "'; identifier already defined";
    throw Semantic_error(node->token, message);
  }
}

void Visitor::local_id_def(Identifier* node, const Variant& value)
{
  String key = node->token.get_text();
  try {
    environment.put_local(key, value);
  }
  catch (const Out_of_range& error) {
    String message = "cannot define '" + key + "'; identifier already defined";
    throw Semantic_error(node->token, message);
  }
}

void Visitor::global_ind_def(Indirection* node, const Variant& value)
{
  Variant name = node->expression->evaluate(this);
  String key;
  try {
    key = name.get_string();
    environment.put_global(key, value);
  }
  catch (const Out_of_range& error) {
    String message = "cannot find '" + key + "'; identifier undefined";
    throw Semantic_error(node->token, message);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

void Visitor::local_ind_def(Indirection* node, const Variant& value)
{
  Variant name = node->expression->evaluate(this);
  String key;
  try {
    key = name.get_string();
    environment.put_local(key, value);
  }
  catch (const Out_of_range& error) {
    String message = "cannot find '" + key + "'; identifier undefined";
    throw Semantic_error(node->token, message);
  }
  catch (const Bad_variant_access& exception) {
    throw Semantic_error(node->token, exception.message);
  }
}

///////////////////////////////////////////////////////////// HANDLES //////////////////////////////////////////////////////////////

void Visitor::report(const Semantic_error& error)
{
  environment.report(error);
}
