#pragma once
#include <string>
#include <vector>
#include <map>
#include <queue>
#include "types.h"

using std::string;
using std::vector;
using std::map;
using std::queue;

class object;
class scope;

class statement
{
public:
	statement();
	statement(string raw);
	virtual string eval_c();
	virtual object * eval(scope * env);
	virtual string parse_string();
	virtual int indent_mod();
private:
	string raw;
};

class value : public statement
{
public:
	value(string val);
	string parse_string();
private:
	string val;
	type_class typ;
};

class variable : public statement
{
public:
	variable(string name);
	string parse_string();
private:
	string name;
};

class modifier_base : public statement
{
public:
	modifier_base(statement * on);
	int indent_mod();
protected:
	statement * on;
};

class list_accessor : public statement
{
public:
	list_accessor(statement * list, statement * index);
	string parse_string();
protected:
	statement * list;
	statement * index;
};

class annotation_tag : public modifier_base
{
public:
	annotation_tag(string name, vector<statement *> params, statement * on);
	string parse_string();
private:
	string name;
	std::vector<statement *> params;
};

class math_statement : public statement
{
public:
	math_statement(queue<statement *> operands, queue<char> operators);
	string parse_string();
private:
	queue<statement*> operands;
	queue<char> operators;
	static void eval_single_op(object * s1, char op, object * s2);
};

class compund_statement : public statement
{
public:
	compund_statement(vector<statement *> operations);
	string parse_string();
private:
	vector<statement *> operations;
};

class assignment : public statement
{
public:
	assignment(statement * name, statement * value);
	string parse_string();
private:
	statement * name;
	statement * val;
};

class decleration : public statement
{
public:
	decleration(string name, type_class type);
	string parse_string();
private:
	string name;
	type_class type;
};

class decleration_with_assignment : public statement
{
public:
	decleration_with_assignment(statement * name, type_class type, statement * val);
	string parse_string();
private:
	statement * name;
	type_class type;
	statement * value;
};

class function_call : public statement
{
public:
	function_call(string name, vector<statement *> params);
	string parse_string();
private:
	string name;
	std::vector<statement *> params;
};

class block_heading : public statement
{
public:
	block_heading() {};
	block_heading(string raw);
	virtual string eval_c();
	virtual object * eval(scope * env);
	virtual string parse_string() = 0;
	int indent_mod();
private:
	string raw;
};

class end_block_statement : public statement
{
public:
	string parse_string();
	int indent_mod();
};

class flow_statement : public statement
{
public:
	enum type { BREAK, NEXT };
	flow_statement(type typ);
	string parse_string();
private:
	flow_statement::type typ;
};

class if_heading : public block_heading
{
public:
	if_heading(statement * condition);
	string parse_string();
private:
	statement * condition;
};

class else_heading : public block_heading
{
public:
	else_heading() : else_heading(nullptr) {};
	else_heading(block_heading * wrapping);
	string parse_string();
private:
	block_heading * wrapping;
};

class for_classic_heading : public block_heading
{
public:
	for_classic_heading(statement * init, statement * condition, statement * tick);
	string parse_string();
private:
	statement * init;
	statement * condition;
	statement * tick;
};

class for_itterator_heading : public block_heading
{
public:
	for_itterator_heading(statement * val, statement * list);
	string parse_string();
private:
	statement * val;
	statement * list;
};

class while_heading : public block_heading
{
public:
	while_heading(statement * condition);
	string parse_string();
private:
	statement * condition;
};

class switch_heading : public block_heading
{
public:
	enum type { SWITCH, MATCH };
	switch_heading(switch_heading::type typ, statement * on);
	string parse_string();
private:
	switch_heading::type typ;
	statement * on;
};

class class_heading : public block_heading
{
public:
	enum class_type { CLASS, STRUCT, ANNOTATION };
	struct heading_generic 
	{
		string t_name;
		type_class extends;
	};
	class_heading(class_heading::class_type type, string name);
	class_heading(class_heading::class_type type, string name,
		vector<heading_generic> generic_types);
	class_heading(class_heading::class_type type, string name, type_class extends);
	class_heading(class_heading::class_type type, string name, 
		vector<heading_generic> generic_types, type_class extends);
	string parse_string();
	string get_name();
	static class_heading::heading_generic generate_generic_heading(string t_name, 
		type_class extends);
private:
	string name;
	class_heading::class_type type;
	type_class extends;
	vector<heading_generic> generic_types;
};

class enum_heading : public block_heading
{
public:
	enum_heading(string name);
	enum_heading(string name, class_heading * basetype);
	string parse_string();
private:
	string name;
	statement * basetype;
};

class modifier : public modifier_base
{
public:
	enum modifier_type { STATIC, PRIVATE };
	modifier(modifier::modifier_type type, statement * modof);
	string parse_string();
private:
	modifier::modifier_type type;
};

class self_modifier : public modifier_base
{
public:
	enum modifier_type { PLUS, MINUS };
	enum modifier_time { PRE, POST };
	self_modifier(self_modifier::modifier_type type, 
		self_modifier::modifier_time time, statement * on);
	string parse_string();
private:
	self_modifier::modifier_type type;
	self_modifier::modifier_time time;
};

class return_statement : public statement
{
public:
	return_statement(statement * returns);
	string parse_string();
private:
	statement * returns;
};

class import_statement : public statement
{
public:
	import_statement(string import_name);
	string parse_string();
private:
	string import_name;
};

class throw_statement : public statement
{
public:
	throw_statement(statement * throws);
	string parse_string();
private:
	statement * throws;
};

class new_object_statement : public statement
{
public:
	new_object_statement(type_class type, vector<statement *> params);
	string parse_string();
private:
	type_class type;
	vector<statement *> params;
};

class function_heading : public block_heading
{
public:
	function_heading(string name, vector<function::function_parameter> params, 
		type_class returns);
	string parse_string();
private:
	string name;
	vector<function::function_parameter> params;
	type_class returns;
	string function_parameter_sig(function::function_parameter);
	string function_parameter_c_sig(function::function_parameter);
};

class comparison : public statement
{
public:
	enum comparison_type { Equal, NotEqual, LessThan, LessThanEqualTo, GreaterThan, 
		GreaterThanEqualTo };
	comparison(statement * s1, comparison::comparison_type typ, statement * s2);
	string parse_string();
private:
	statement * s1;
	statement * s2;
	comparison_type typ;
	string comparison_string();
};

class invert : public statement
{
public:
	invert(statement * value);
	string parse_string();
private:
	statement * val;
};

class bitinvert : public statement
{
public:
	bitinvert(statement * value);
	string parse_string();
private:
	statement * val;
};

class bitwise : public statement
{
public:
	enum operation { And, Or, Xor, ShiftLeft, ShiftRight };
	bitwise(statement * s1, bitwise::operation op, statement * s2);
	string parse_string();
private:
	statement * s1;
	statement * s2;
	bitwise::operation op;
	string operation_string();
};

class boolean_conjunction : public statement
{
public:
	enum conjunction_type { And, Or };
	boolean_conjunction(statement * s1, boolean_conjunction::conjunction_type conj, 
		statement * s2);
	string parse_string();
private:
	statement * s1;
	statement * s2;
	boolean_conjunction::conjunction_type conj;
	string conj_string();
};