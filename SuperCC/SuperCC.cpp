#include "stdafx.h"
#include <iostream>
#include <string>
#include <map>
#include <stack>
#include <vector>
#include "types.h"
#include "statement.h"
#include "parsers.h"
#include "utils.h"
#include "builtin.h"

using std::string;
using std::map;
using std::stack;
using std::vector;

scope env;

stack<block_heading *> blocks;

template<class T>
type_class define_native_builtins(string name)
{
	type_class t = type_class(name);
	(*t.get_operators())[string(1, '+')] = function("add", vector<function::function_parameter>(), type_class(), &env, navite_builtin::create_add<T>());
	(*t.get_operators())[string(1, '-')] = function("sub", vector<function::function_parameter>(), type_class(), &env, navite_builtin::create_sub<T>());
	(*t.get_operators())[string(1, '*')] = function("mul", vector<function::function_parameter>(), type_class(), &env, navite_builtin::create_mul<T>());
	(*t.get_operators())[string(1, '/')] = function("div", vector<function::function_parameter>(), type_class(), &env, navite_builtin::create_div<T>());
	t.get_scope()->define_function("to_string", function("to_string", vector<function::function_parameter>(), type_class(), &env, navite_builtin::create_to_string<T>()));
	t.get_scope()->define_function("clone", function("clone", vector<function::function_parameter>(), type_class(), &env, navite_builtin::create_clone<T>()));
	return t;
}

template<class T>
type_class define_native_fixpoint_builtins(string name)
{
	type_class t = define_native_builtins<T>(name);
	(*t.get_operators())[string(1, '%')] = function("mod", vector<function::function_parameter>(), type_class(), &env, navite_builtin::create_mod<T>());
	return t;
}

void load_standards()
{
	env.define_type("Char", define_native_fixpoint_builtins<char>("Char"));
	env.define_type("Int", define_native_fixpoint_builtins<int>("Int"));
	env.define_type("Long", define_native_fixpoint_builtins<long>("Long"));
	env.define_type("Float", define_native_builtins<float>("Float"));
	env.define_type("Double", define_native_builtins<double>("Double"));
	env.define_type("String", type_class("String"));
}

int main()
{
	load_standards();
	bool running = true;
	while (running)
	{
		string input = string();
		std::cout << ">> ";
		bool parsing_string = false;
		char c;
		do {
			c = getchar();
			if ((c == '"' || c == '\'') && input[input.length() - 1] != '\\')
			{
				parsing_string = !parsing_string;
			}
			if (parsing_string || (c != ';' && c != '{' && c != '}'))
			{
				input += c;
			}
		} while ((c != ';' && c != '{' && c != '}') || parsing_string);
		input = remove_pad(input);
		statement * stmt = parse_statement(input, &env);
		if (!blocks.empty())
		{
			blocks.top()->add_statement(stmt);
		}
		else
		{
			if (c != '{')
			{
				std::cout << stmt->parse_string() << std::endl;
				stmt->eval(&env);
			}
		}
		if (c == '{')
		{
			blocks.push((block_heading *)stmt);
			std::cout << "start new scope" << std::endl;
		}
		else if (c == '}')
		{
			statement * s = blocks.top();
			blocks.pop();
			std::cout << s->parse_string() << std::endl;
			s->eval(&env);
			std::cout << "end scope" << std::endl;
		}
		else
		{

		}
		if (input == "exit()")
		{
			running = false;
		}
	}
}

