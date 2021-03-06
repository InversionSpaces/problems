#pragma once

#include <optional>
#include <vector>
#include <string>
#include <cstring>
#include <exception>

#include "expression.hpp"

using namespace std;

class Parser
{
private:
	const char* input;
	
	vector<string> vars;
	
	inline int get_token_len(const token_t& tk)
	{
		switch (tk.type) {
			case TOKEN:
				return get_name(tk.id, tokens).size();
			case FUNC:
				return get_name(tk.id, funcs).size();
			case VAR:
				return get_name(tk.id, vars).size();
			default:
				throw invalid_argument("Can't get len of token");
				break;
		}
	}

	inline optional<int> find_in(const vector<string>& arr)
	{
		for (int i = 0; i < arr.size(); ++i) {
			int len = arr[i].size();
			if (strncmp(arr[i].c_str(), input, len) == 0) {
				input += len;
				
				return i;
			}
		}
		
		return nullopt;
	}

	inline optional<token_t> parse_token()
	{		
		while (isspace(*input)) input++;
		
		if (*input == 0) return nullopt;
		
		double num = 0;
		int n = 0;
		
		int res = sscanf(input, "%lf%n", &num, &n);
		
		if (res) {
			input += n;
			
			token_t ret = {};
			ret.type = NUM;
			ret.num = num;
			
			return  ret;
		}
		
#define FIND_IN_ARRAY(arr, match)	\
if (auto i = find_in(arr)) {		\
	token_t ret = {};				\
	ret.type = match;				\
	ret.id = *i;					\
	return ret;						\
}
		
		FIND_IN_ARRAY(tokens, TOKEN)
		FIND_IN_ARRAY(funcs, FUNC)
		FIND_IN_ARRAY(vars, VAR)
		
		return nullopt;
	}

	inline optional<expr_t*> parse_expr()
	{	
		auto tk = parse_token();
		
		if (!tk) 
			return nullopt;
		
		if (tk->type == NUM || tk->type == VAR) {
			return new expr_t {*tk, nullptr, nullptr};
		}
		
		// Parse expr in brackets
		if (tk->type == TOKEN && tk->id == get_id("(", tokens)) {
			auto ex = parse();
			if (!ex) 
				return nullopt;
			
			tk = parse_token();
			if (!tk) 
				return nullopt;
				
			if (tk->type != TOKEN || tk->id != get_id(")", tokens)) 
				return nullopt;
			
			return ex;
		}
		
		// Parse expression after this one token
		// This is for sin, cos etc
		auto down_ex = parse_expr();
		
		if (!down_ex) 
			return nullopt;
		
		return new expr_t {*tk, *down_ex, nullptr};
	}

	inline optional<expr_t*> parse_bin_expr(int mpriority)
	{
		auto left = parse_expr();
		
		if (!left) 
			return nullopt;
		
		while (1) {
			auto op = parse_token();
			
			if (!op)
				return left;
			
			int priority = op_priority(*op);
			
			// Priority defines whether we need to continue parsing
			// or to return parsing to upper function
			
			// "(" or ")" will be dropped here too
			// This is not obvious
			if (priority <= mpriority) {
				input -= get_token_len(*op);
				
				return left;
			}
			
			auto right = parse_bin_expr(priority);
			if (!right) {
				purge_expr(*left);
				
				return nullopt;
			}
			
			left = new expr_t {*op, *left, *right};
		}
	}
	
	inline optional<expr_t*> parse()
	{
		return parse_bin_expr(0);
	}
	
public:
	Parser(const char* input) :
	input(input)
	{
	}

	optional<Expression*> process() 
	{
		auto ex = parse();
		if (!ex) return nullopt;
		
		return new Expression(*ex, move(vars));
	}
	
	void reset_input(const char* str)
	{
		input = str;
	}
	
	void reset_vars()
	{
		vars.clear();
	}

	void add_var(const char* str)
	{
		vars.emplace_back(str);
	}
};
