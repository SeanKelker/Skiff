#include "modes.h"

namespace skiff
{
    namespace modes
    {
        using ::std::string;
        using ::std::vector;
        using ::std::queue;
        using ::std::stack;
        using statements::statement;
        using statements::braced_block;
        using environment::scope;

        void interactive_mode()
        {
            queue<statement *> stmts;

            bool running = true;
            while (running)
            {
                string input = string();
                std::cout << ">> ";
                bool parsing_string = false;
                char c;
                do {
                    c = getchar();
                    if ((c == '"' || c == '\'') && (input.length() == 0 ||
                        input[input.length() - 1] != '\\'))
                    {
                        parsing_string = !parsing_string;
                    }
                    if (parsing_string || (c != ';' && c != '{' && c != '}'))
                    {
                        input += c;
                    }
                } while ((c != ';' && c != '{' && c != '}') || parsing_string);
                // running = handle_line(input, c, &stmts);
            }

            size_t indent = 0;
            while (!stmts.empty())
            {
                statement * s = stmts.front();
                stmts.pop();
                std::cout << utils::generate_indent(indent) << s->parse_string() << std::endl;
                indent += s->indent_mod();
            }
        }

        queue<statement *> parse_file(string infile, bool debug)
        {
            std::fstream fin(infile, std::fstream::in);
            char c = '\0';
            string file;
            string line;
            while(std::getline(fin, line))
            {
                file += line + "\n";
            }
            vector<tokenizer::token> token_sequence = tokenizer::tokenize(file);
//            std::cout << tokenizer::sequencetostring(token_sequence) << std::endl;
            vector<statement *> statements = parser(token_sequence).parse();
            for(statement * st : statements)
            {
                std::cout << st->parse_string() << std::endl;
            }

            // stack<statements::braced_block *> blocks;
            // blocks.push(new braced_block());

            // bool running = true;
            // string input = string();
            // stack<char> braces;
            // char c = '\0';
            // while (fin >> std::noskipws >> c && running) {
            // 	if (!braces.empty() || (c != ';' && c != '{' && c != '}'))
            // 	{
            // 		input += c;
            // 	}
            // 	if ((c == ';' || c == '{' || c == '}') && braces.empty())
            // 	{
            // 		running = handle_line(input, c, &blocks, debug);
            // 		input = string();
            // 		c = '\0';
            // 	}
            // 	utils::track_braces(input.length() == 0 ? '\0' : input[input.length() - 1], c,
            // 		&braces);
            // }
            queue<statement *> stmts;
            // stmts.push(blocks.top());
            return stmts;
        }

        void print_parse(queue<statement *> statements)
        {
            size_t indent = 0;
            while (!statements.empty())
            {
                statement * s = statements.front();
                statements.pop();
                std::cout << utils::generate_indent(indent) << s->parse_string() << std::endl;
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
                std::cout << utils::generate_indent(indent) << s->eval_c();
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
            while (!statements.empty())
            {
                statement * s = statements.front();
                statements.pop();
                s->eval(env);
            }
        }
    }
}
