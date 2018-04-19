#include "stdafx.h"
#include <queue>
#include "../Core/types.h"
#include "../Core/statement.h"
#include "../Core/parsers.h"
#include "../Core/utils.h"
#include "../Core/builtin.h"
#include "../Core/modes.h"

int main()
{
	skiff::types::scope env;

	skiff::builtin::load::load_standards(&env);
	std::queue<skiff::statements::statement *> test_statements = 
		skiff::modes::parse_file("test.su");
	skiff::modes::print_parse(test_statements);
	system("pause");
}

