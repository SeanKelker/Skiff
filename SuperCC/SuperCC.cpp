#include "stdafx.h"
#include <iostream>
#include <string>
#include <map>
#include <stack>
#include "types.h"
#include "statement.h"
#include "parsers.h"
#include "utils.h"

using std::string;
using std::map;
using std::stack;

scope env;

stack<block_heading *> blocks;

int main()
{
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

