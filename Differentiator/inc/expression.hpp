#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>

using namespace std;

const vector<string> tokens = {"(", ")"};
	
const vector<string> functions = { 
	"+", "-", "*", "^", "/", "sin", 
	"cos", "tg", "sh", "ch", "abs" 
};

enum toktype_t
{
	VAR,
	FUNC,
	TOKEN,
	NUM
};

struct token_t
{
	toktype_t type;
	
	union 
	{
		int id;
		double num;
	};
};

struct expr_t
{
	token_t val;
	
	expr_t* arg1;
	expr_t* arg2;
};

void purge_expr(expr_t* ex)
{
	if (ex->arg1) purge_expr(ex->arg1);
	if (ex->arg2) purge_expr(ex->arg2);
	
	delete ex;
}

class Expression
{
private:
	expr_t* root;
	const vector<string> vars;
	
	inline void dump_inner(const expr_t* ex, FILE* fp)
	{
		switch (ex->val.type) {
			case TOKEN:
				fprintf(fp, "NODE%p [shape=ellipse label=\"%s\"]\n", 
					ex, tokens[ex->val.id].c_str());
			break;
			case FUNC:
				fprintf(fp, "NODE%p [shape=ellipse label=\"%s\"]\n", 
					ex, functions[ex->val.id].c_str());
			break;
			case NUM:
				fprintf(fp, "NODE%p [shape=ellipse label=\"%lf\"]\n", 
					ex, ex->val.num);
			break;
			case VAR:
				fprintf(fp, "NODE%p [shape=ellipse label=\"%s\"]\n", 
					ex, vars[ex->val.id].c_str());
			break;
		}
		
		if (ex->arg1) {
			fprintf(fp, "NODE%p -> NODE%p\n", ex, ex->arg1);
			dump_inner(ex->arg1, fp);
		}
		
		if (ex->arg2) {
			fprintf(fp, "NODE%p -> NODE%p\n", ex, ex->arg2);
			dump_inner(ex->arg2, fp);
		}
	}
	
public:
	Expression(expr_t* root, const vector<string>& vars) :
	root(root),
	vars(vars)
	{
	}
	
	int dump(const char* filename) 
	{
		FILE* fp = fopen(filename, "w");
		
		if (!fp) return 0;
		
		fprintf(fp, "digraph Dif {\n");
		dump_inner(root, fp);
		fprintf(fp, "}\n");
		
		fclose(fp);
		
		return 1;
	}
	
	~Expression()
	{
		purge_expr(root);
	}
};
