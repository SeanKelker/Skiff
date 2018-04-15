#include "stdafx.h"
#include "modes.h"
#include "parsers.h"
#include "utils.h"
#include <fstream>
#include <iostream>

void interactive_mode()
{
	queue<statement *> statements;

	bool running = true;
	while (running)
	{
		string input = string();
		std::cout << ">> ";
		bool parsing_string = false;
		char c;
		do {
			c = getchar();
			if ((c == '"' || c == '\'') && (input.length() == 0 || input[input.length() - 1] != '\\'))
			{
				parsing_string = !parsing_string;
			}
			if (parsing_string || (c != ';' && c != '{' && c != '}'))
			{
				input += c;
			}
		} while ((c != ';' && c != '{' && c != '}') || parsing_string);
		running = handle_line(input, c, &statements);
	}

	size_t indent = 0;
	while (!statements.empty())
	{
		statement * s = statements.front();
		statements.pop();
		std::cout << generate_indent(indent) << s->parse_string() << std::endl;
		indent += s->indent_mod();
	}
}

queue<statement *> parse_file(string infile)
{
	queue<statement *> statements;

	std::fstream fin(infile, std::fstream::in);
	bool running = true;
	string input = string();
	stack<char> braces;
	//bool parsing_string = false;
	char c = '\0';
	while (fin >> std::noskipws >> c && running) {
		if (!braces.empty() || (c != ';' && c != '{' && c != '}'))
		{
			input += c;
		}
		if ((c == ';' || c == '{' || c == '}') && braces.empty())
		{
			running = handle_line(input, c, &statements);
			input = string();
			c = '\0';
		}
		track_braces(input.length() == 0 ? '\0' : input[input.length() - 1], c, &braces);
	}
	return statements;
}

void print_parse(queue<statement *> statements)
{
	size_t indent = 0;
	while (!statements.empty())
	{
		statement * s = statements.front();
		statements.pop();
		std::cout << generate_indent(indent) << s->parse_string() << std::endl;
		indent += s->indent_mod();
	}
}

void translate_c(queue<statement *> statements)
{
	size_t indent = 0;
	while (!statements.empty())
	{
		statement * s = statements.front();
		statements.pop();
		std::cout << generate_indent(indent) << s->eval_c();
		indent += s->indent_mod();
		if (s->indent_mod() == 0)
		{
			std::cout << ";" << std::endl;
		}
		else if (s->indent_mod() == 1)
		{
			std::cout << std::endl << "{" << std::endl;
		}
		else
		{
			std::cout << "}" << std::endl;
		}
	}
}

void evaluate(scope * env, queue<statement *> statements)
{

	size_t indent = 0;
	while (!statements.empty())
	{
		statement * s = statements.front();
		statements.pop();
		std::cout << s->eval(env) << std::endl;
		indent += s->indent_mod();
	}
}