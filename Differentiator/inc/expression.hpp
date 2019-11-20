#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <cmath>

using namespace std;

const vector<string> tokens = {"(", ")"};
	
const vector<string> functions = { 
	"+", "-", "*", "^", "/", "sin", 
	"cos", "tan", "sinh", "cosh", "abs", 
	"log", "log10", "exp"
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

inline int reduce_expr(expr_t* ex)
{
	assert(ex);
	
	if (ex->val.type == NUM) return 1;
	if (ex->val.type == VAR) return 0;
	
	int reduced = 1;
	if (ex->arg1) reduced = reduce_expr(ex->arg1) && reduced;
	if (ex->arg2) reduced = reduce_expr(ex->arg2) && reduced;
	
	string name = functions[ex->val.id];
	
	if (!reduced) return 0;
	
#define PLUS_MINUS(SIGN)										\
if (name == #SIGN) {											\
	double res = 0;												\
	if (ex->arg1 && ex->arg2) {									\
		res = ex->arg1->val.num SIGN ex->arg2->val.num;			\
		purge_expr(ex->arg1);									\
		purge_expr(ex->arg2);									\
		ex->arg1 = nullptr;										\
		ex->arg2 = nullptr;										\
	}															\
	else {														\
		res = SIGN ex->arg1->val.num;							\
		purge_expr(ex->arg1);									\
		ex->arg1 = nullptr;										\
	}															\
	ex->val.type = NUM;											\
	ex->val.num = res;											\
	return 1;													\
}

	PLUS_MINUS(-)
	PLUS_MINUS(+)
	
	if (name == "*") {
		double res = ex->arg1->val.num * ex->arg2->val.num;
			
		purge_expr(ex->arg1);
		purge_expr(ex->arg2);
		
		ex->arg1 = nullptr;
		ex->arg2 = nullptr;
		
		ex->val.type = NUM;
		ex->val.num = res;
		
		return 1;
	}
	
	if (name == "/") {
		double res = ex->arg1->val.num / ex->arg2->val.num;
			
		purge_expr(ex->arg1);
		purge_expr(ex->arg2);
		
		ex->arg1 = nullptr;
		ex->arg2 = nullptr;
		
		ex->val.type = NUM;
		ex->val.num = res;
		
		return 1;
	}
	
	if (name == "^") {
		double res = pow(ex->arg1->val.num, ex->arg2->val.num);
			
		purge_expr(ex->arg1);
		purge_expr(ex->arg2);
		
		ex->arg1 = nullptr;
		ex->arg2 = nullptr;
		
		ex->val.type = NUM;
		ex->val.num = res;
		
		return 1;
	}
	
#define MATH_FUNC(FUNC)						\
if (name == #FUNC) {						\
	double res = FUNC(ex->arg1->val.num);	\
	purge_expr(ex->arg1);					\
	ex->arg1 = nullptr;						\
	ex->val.type = NUM;						\
	ex->val.num = res;						\
	return 1;								\
}
	
	MATH_FUNC(sin)
	MATH_FUNC(cos)
	MATH_FUNC(sinh)
	MATH_FUNC(cosh)
	MATH_FUNC(tan)
	MATH_FUNC(tanh)
	MATH_FUNC(abs)
	MATH_FUNC(exp)
	MATH_FUNC(log)
	MATH_FUNC(log10)
	
	return 0;
}

expr_t* copy_expr(const expr_t* ex)
{
	if (ex == nullptr) return nullptr;
	
	expr_t* retval = new expr_t(*ex);
	
	retval->arg1 = ex->arg1 ? copy_expr(ex->arg1) : nullptr;
	retval->arg2 = ex->arg2 ? copy_expr(ex->arg2) : nullptr;
	
	return retval;
}

class Expression
{
private:
	expr_t* root;
	const vector<string> vars;
	
public:
	Expression(const Expression& other) :
	vars(other.vars),
	root(copy_expr(other.root))
	{
	}

	Expression(expr_t* root, const vector<string>& vars) :
	root(root),
	vars(vars)
	{
	}
	
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
	
	int reduce()
	{
		return reduce_expr(root);
	}
	
	Expression* derivative()
	{
		
	}
	
	~Expression()
	{
		purge_expr(root);
	}
};
