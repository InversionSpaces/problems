#pragma once

#include <iostream>
#include <cstdio>
#include <vector>

using namespace std;

const vector<const char*> tokens = {"(", ")"};
	
const vector<const char*> functions = { 
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

class Expression
{
private:
	expr_t* root;
	
	void purge_expr(expr_t* ex)
	{
		if (ex->arg1) purge_expr(ex->arg1);
		if (ex->arg2) purge_expr(ex->arg2);
		
		delete ex;
	}
	
	inline void dump_inner(const expr_t* ex, FILE* fp)
	{
		switch (ex->val.type) {
			case TOKEN:
				fprintf(fp, "NODE%p [shape=ellipse label=\"%s\"]\n", 
					ex, tokens[ex->val.id]);
			break;
			case FUNC:
				fprintf(fp, "NODE%p [shape=ellipse label=\"%s\"]\n", 
					ex, functions[ex->val.id]);
			break;
			case NUM:
				fprintf(fp, "NODE%p [shape=ellipse label=\"%lf\"]\n", 
					ex, ex->val.num);
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
	Expression(expr_t* root) :
	root(root)
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
