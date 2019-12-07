#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <map>
#include <cmath>
#include <cassert>
#include <optional>
#include <algorithm>
#include <utility>
#include <functional>

template <typename T> 
int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

using namespace std;

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

void purge_expr(expr_t*);

expr_t* copy_expr(const expr_t*);

expr_t* derivative_expr(const expr_t*, const int);

inline int is_num(const expr_t* ex)
{
	return (ex && ex->val.type == NUM);
}

inline int is_equal(const expr_t* ex, const double val) 
{
	return (is_num(ex) && 
		abs(ex->val.num - val) < numeric_limits<double>::epsilon());
}

inline optional<int> get_id(const char*, const vector<string>&);
inline const string& get_name(const int, const vector<string>&);

inline int op_priority(const token_t& op);

inline expr_t* get_func(const char* str, expr_t* arg1, expr_t* arg2);

inline expr_t* get_num(const double num);

inline expr_t* func_derivative(const expr_t* ex, int id, 
	function<expr_t*(expr_t*)> func)
{
	expr_t* copy = copy_expr(ex->arg1);
	expr_t* deriv = derivative_expr(copy, id);
	
	return get_func("*", func(copy), deriv);
}

#define COUNT(SIGN)														\
if (is_num((*ex)->arg1) && is_num((*ex)->arg2)) {						\
	(*ex)->val.type = NUM;												\
	(*ex)->val.num = (*ex)->arg1->val.num SIGN (*ex)->arg2->val.num;	\
	purge_expr((*ex)->arg1);											\
	purge_expr((*ex)->arg2);											\
	(*ex)->arg1 = nullptr;												\
	(*ex)->arg2 = nullptr;												\
	return 1;															\
}

#define MATH_FUNC(FUNC)							\
if (is_num((*ex)->arg1)) {						\
	double res = FUNC((*ex)->arg1->val.num);	\
	purge_expr(*ex);							\
	*ex = get_num(res);							\
	return 1;									\
}												\
return 0;										\

#define PLUS_MINUS_DER(SIGN)										\
expr_t* left = derivative_expr(ex->arg1, id);						\
expr_t* right = ex->arg2 ? derivative_expr(ex->arg2, id) : nullptr;	\
return get_func(#SIGN, left, right);


const map<string, pair<
	function<int(expr_t**)>, 
	function<expr_t*(const expr_t*, int)> 
	>
> functions = { 
	{"+", {	
	[] (expr_t** ex) -> int 
	{
		COUNT(+)
		
		if (is_equal((*ex)->arg1, 0.)) {
			purge_expr((*ex)->arg1);
			
			expr_t* tmp = (*ex)->arg2;
			
			delete *ex;
			
			*ex = tmp;
			
			return 0;
		}
		
		if (is_equal((*ex)->arg2, 0.)) {
			purge_expr((*ex)->arg2);
			
			expr_t* tmp = (*ex)->arg1;
			
			delete *ex;
			
			*ex = tmp;
			
			return 0;
		}
		
		return 0;
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{ 
		PLUS_MINUS_DER(+)
	} 
	} },
	{"-", {	
	[] (expr_t** ex) -> int 
	{ 
		COUNT(-)
		
		if (is_equal((*ex)->arg1, 0.)) {
			purge_expr((*ex)->arg1);
			
			expr_t* tmp = (*ex)->arg2;
			
			delete *ex;
			
			*ex = get_func("-", tmp, nullptr);
			
			return 0;
		}
		
		if (is_equal((*ex)->arg2, 0.)) {
			purge_expr((*ex)->arg2);
			
			expr_t* tmp = (*ex)->arg1;
			
			delete *ex;
			
			*ex = tmp;
			
			return 0;
		}
		
		return 0;
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{
		PLUS_MINUS_DER(-)
	} 
	} },
	{"*", {	
	[] (expr_t** ex) -> int 
	{ 
		COUNT(*)
		
		if (is_equal((*ex)->arg1, 0.) || is_equal((*ex)->arg2, 0.)) {
			purge_expr(*ex);
			
			*ex = get_num(0.);
			
			return 1;
		}
		
		if (is_equal((*ex)->arg1, 1.)) {
			purge_expr((*ex)->arg1);
			
			expr_t* tmp = (*ex)->arg2;
			
			delete *ex;
			
			*ex = tmp;
			
			return 0;
		}
		
		if (is_equal((*ex)->arg2, 1.)) {
			purge_expr((*ex)->arg2);
			
			expr_t* tmp = (*ex)->arg1;
			
			delete *ex;
			
			*ex = tmp;
			
			return 0;
		}
		
		return 0;
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{ 
		expr_t* copy1 = copy_expr(ex->arg1);
		expr_t* copy2 = copy_expr(ex->arg2);
		
		expr_t* d1 = derivative_expr(ex->arg1, id);
		expr_t* d2 = derivative_expr(ex->arg2, id);
		
		expr_t* left = get_func("*", d1, copy2);
		expr_t* right = get_func("*", d2, copy1);
		
		return get_func("+", left, right);
	} 
	} },
	{"^", {	
	[] (expr_t** ex) -> int 
	{ 
		if (is_num((*ex)->arg1) && is_num((*ex)->arg2)) {					
			(*ex)->val.type = NUM;											
			(*ex)->val.num = pow((*ex)->arg1->val.num, (*ex)->arg2->val.num);	
			
			purge_expr((*ex)->arg1);										
			purge_expr((*ex)->arg2);	
												
			(*ex)->arg1 = nullptr;											
			(*ex)->arg2 = nullptr;											
			
			return 1;														
		}
		
		if (is_equal((*ex)->arg1, 1.)) {
			purge_expr(*ex);
			
			*ex = get_num(1.);
			
			return 1;
		}
		
		if (is_equal((*ex)->arg1, 0.)) {
			purge_expr(*ex);
			
			*ex = get_num(0.);
			
			return 1;
		}
		
		if (is_equal((*ex)->arg2, 1.)) {
			purge_expr((*ex)->arg2);
			
			expr_t* tmp = (*ex)->arg1;
			
			delete *ex;
			
			*ex = tmp;
		
			return 0;
		}
		
		if (is_equal((*ex)->arg2, 0.)) {
			purge_expr(*ex);
			
			*ex = get_num(1.);
		
			return 1;
		}
		
		return 0;
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{ 
		expr_t* copy = copy_expr(ex);
		
		expr_t* logu = get_func("log", ex->arg1, nullptr);
		expr_t* logu_v = get_func("*", logu, ex->arg2);
		
		expr_t* deriv = derivative_expr(logu_v, id);
		
		delete logu;
		delete logu_v;
		
		return get_func("*", copy, deriv);
	} 
	} },
	{"/", {	
	[] (expr_t** ex) -> int 
	{ 
		COUNT(/);
		
		if (is_equal((*ex)->arg1, 0.)) {
			purge_expr(*ex);
			
			*ex = get_num(0.);
			
			return 1;
		}
		
		if (is_equal((*ex)->arg2, 1.)) {
			purge_expr((*ex)->arg2);
			
			expr_t* tmp = (*ex)->arg1;
			
			delete *ex;
			
			*ex = tmp;
		
			return 0;
		}
		
		return 0;
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{ 
		expr_t* copy1 = copy_expr(ex->arg1);
		expr_t* copy2 = copy_expr(ex->arg2);
		
		expr_t* d1 = derivative_expr(ex->arg1, id);
		expr_t* d2 = derivative_expr(ex->arg2, id);
		
		expr_t* left = get_func("*", d1, copy2);
		expr_t* right = get_func("*", d2, copy1);
		
		expr_t* top = get_func("-", left, right);
		
		expr_t* bottom = get_func("^", copy_expr(ex->arg2), get_num(2));
		
		return get_func("/", top, bottom); 
	} 
	} },
	{"sin", {	
	[] (expr_t** ex) -> int { 
		MATH_FUNC(sin)
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{ 
		return func_derivative(ex, id, [] (expr_t* copy) 
		-> expr_t* {
			return get_func("cos", copy, nullptr);
		});
	} 
	} },
	{"cos", {	
	[] (expr_t** ex) -> int { 
		MATH_FUNC(cos)
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{ 
		return func_derivative(ex, id, [] (expr_t* copy) 
		->  expr_t* {
			expr_t* sin = get_func("sin", copy, nullptr);
			return get_func("-", sin, nullptr);
		});
	} 
	} },
	{"tan", 	{	
	[] (expr_t** ex) -> int { 
		MATH_FUNC(tan)
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{
		return func_derivative(ex, id, [] (expr_t* copy) 
		->  expr_t* {
			expr_t* cos = get_func("cos", copy, nullptr);
			expr_t* power = get_func("^", cos, get_num(2));
			return get_func("/", get_num(1), power);
		});
	} 
	} },
	{"sinh", {	
	[] (expr_t** ex) -> int { 
		MATH_FUNC(sinh)
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{ 
		return func_derivative(ex, id, [] (expr_t* copy) 
		->  expr_t* {
			return get_func("cosh", copy, nullptr);
		});
	} 
	} },
	{"cosh", {	
	[] (expr_t** ex) -> int { 
		MATH_FUNC(cosh)
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{ 
		return func_derivative(ex, id, [] (expr_t* copy) 
		->  expr_t* {
			return get_func("sinh", copy, nullptr);
		});
	} 
	} },
	{"abs", {	
	[] (expr_t** ex) -> int { 
		MATH_FUNC(abs)
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{
		return func_derivative(ex, id, [] (expr_t* copy) 
		->  expr_t* {
			return get_func("sign", copy, nullptr);
		});
	} 
	} },
	{"log10", {	
	[] (expr_t** ex) -> int { 
		MATH_FUNC(log10)
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{ 
		return func_derivative(ex, id, [] (expr_t* copy) 
		->  expr_t* {
			expr_t* log_10 = get_func("log", get_num(10), nullptr);
			expr_t* mul = get_func("*", copy, log_10);
			return get_func("/", get_num(1), mul);
		});
	} 
	} },
	{"log", {	
	[] (expr_t** ex) -> int { 
		MATH_FUNC(log)
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{ 
		return func_derivative(ex, id, [] (expr_t* copy) 
		->  expr_t* {
			return get_func("/", get_num(1), copy);
		});
	} 
	} },
	{"exp", 	{	
	[] (expr_t** ex) -> int { 
		MATH_FUNC(exp)
	}, 
	[] (const expr_t* ex, int id) -> expr_t* 
	{
		return func_derivative(ex, id, [] (expr_t* copy) 
		->  expr_t* {
			return get_func("exp", copy, nullptr);
		});
	} 
	} }
};

#undef PLUS_MINUS
#undef MUL_DIV
#undef MATH_FUNC

#undef PLUS_MINUS_DER

const vector<string> tokens = {"(", ")"};

const vector<string> funcs = 
([] (const map<string, auto>& m) -> vector<string> 
{
	vector<string> retval(m.size());
	
	int i = 0;
	for (auto it = m.begin(); it != m.end(); ++it) {
		retval[i] = it->first;
		++i;
	}
	
	return retval;
})
(functions);

inline expr_t* get_func(const char* str, expr_t* arg1, expr_t* arg2) {
	token_t tok = {};
		
	tok.type = FUNC;
	tok.id = *get_id(str, funcs);
	
	return new expr_t {tok, arg1, arg2};
}

inline expr_t* get_num(const double num)
{
	token_t tok = {};
	
	tok.type = NUM;
	tok.num = num;
	
	return new expr_t {tok, nullptr, nullptr};
}

inline optional<int> get_id(const char* str, const vector<string>& vec)
{
	auto it = find(vec.begin(), vec.end(), str);
	
	if (it != vec.end())
		return distance(vec.begin(), it);
	
	return nullopt;
}

inline const string& get_name(const int id, const vector<string>& vec)
{
	auto it = vec.begin();
	advance(it, id);
	
	return *it;
}


void purge_expr(expr_t* ex)
{
	if (ex->arg1) purge_expr(ex->arg1);
	if (ex->arg2) purge_expr(ex->arg2);
	
	delete ex;
}

inline int reduce_expr(expr_t** ex)
{
	assert(ex);
	
	if ((*ex)->val.type == NUM) return 1;
	if ((*ex)->val.type == VAR) return 0;
	
	if ((*ex)->arg1) reduce_expr(&((*ex)->arg1));
	if ((*ex)->arg2) reduce_expr(&((*ex)->arg2));
	
	string name = get_name((*ex)->val.id, funcs);
	
	return functions.at(name).first(ex);
}

inline void substitute_expr(expr_t* ex, const int id, const double val)
{
	assert(ex);
	
	if (ex->arg1) substitute_expr(ex->arg1, id, val);
	if (ex->arg2) substitute_expr(ex->arg2, id, val);
	
	if (ex->val.type == VAR && ex->val.id == id) {
		ex->val.type = NUM;
		ex->val.num = val;
	}
}

expr_t* copy_expr(const expr_t* ex)
{
	if (ex == nullptr) return nullptr;
	
	expr_t* retval = new expr_t(*ex);
	
	retval->arg1 = ex->arg1 ? copy_expr(ex->arg1) : nullptr;
	retval->arg2 = ex->arg2 ? copy_expr(ex->arg2) : nullptr;
	
	return retval;
}

expr_t* derivative_expr(const expr_t* ex, const int id)
{
	assert(ex);
	
	if (ex->val.type == NUM) {
		return get_num(0);
	}
	
	if (ex->val.type == VAR) {
		if (ex->val.id == id) {
			return get_num(1);
		}
		else {
			// TODO
			return new expr_t {ex->val, nullptr, nullptr};
		}
	}
	
	string name = get_name(ex->val.id, funcs);
	
	return functions.at(name).second(ex, id);
}

inline int op_priority(const token_t& op)
{
	if (op.type == FUNC) {
		if (op.id == get_id("+", funcs)) return 1;
		if (op.id == get_id("-", funcs)) return 1;
		if (op.id == get_id("*", funcs)) return 2;
		if (op.id == get_id("/", funcs)) return 2;
		if (op.id == get_id("^", funcs)) return 3;
	}
	
	return 0; // For tokens, functions and nums
}

inline int roundable(const double num) {
	if (abs(num - roundl(num)) < numeric_limits<double>::epsilon())
		return 1;
	return 0;
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
	
	inline void dump_expr_dot(const expr_t* ex, FILE* fp)
	{
		switch (ex->val.type) {
			case TOKEN:
				fprintf(fp, "NODE%p [shape=ellipse label=\"%s\"]\n", 
					ex, get_name(ex->val.id, tokens).c_str());
			break;
			case FUNC:
				fprintf(fp, "NODE%p [shape=ellipse label=\"%s\"]\n", 
					ex, get_name(ex->val.id, funcs).c_str());
			break;
			case NUM:
				if (roundable(ex->val.num)) 
					fprintf(fp, "NODE%p [shape=ellipse label=\"%ld\"]\n", 
						ex, lround(ex->val.num));
				else 
					fprintf(fp, "NODE%p [shape=ellipse label=\"%g\"]\n", 
						ex, ex->val.num);
			break;
			case VAR:
				fprintf(fp, "NODE%p [shape=ellipse label=\"%s\"]\n", 
					ex, get_name(ex->val.id, vars).c_str());
			break;
		}
		
		if (ex->arg1) {
			fprintf(fp, "NODE%p -> NODE%p [label=\"arg1\"]\n", 
				ex, ex->arg1);
			dump_expr_dot(ex->arg1, fp);
		}
		
		if (ex->arg2) {
			fprintf(fp, "NODE%p -> NODE%p [label=\"arg2\"]\n", 
				ex, ex->arg2);
			dump_expr_dot(ex->arg2, fp);
		}
	}
	
	int dump_dot(FILE* fp)
	{
		assert(fp);
		
		fprintf(fp, "digraph Dif {\n");
		dump_expr_dot(root, fp);
		fprintf(fp, "}\n");
		
		return 0;
	}
	
	void dump_expr_latex(const expr_t* ex, int priority, FILE* fp)
	{
		switch (ex->val.type) {
			case TOKEN:
				fprintf(fp, "%s", get_name(ex->val.id, tokens).c_str());
			break;
			case FUNC:
			{
				int id = ex->val.id;
				
				if (ex->arg1 && ex->arg2) {
					int tpriority = op_priority(ex->val);
					
					if (tpriority <= priority)
						fprintf(fp, " \\left( ");
					
					if (id == get_id("+", funcs)) {
						dump_expr_latex(ex->arg1, tpriority, fp);
						fprintf(fp, " + ");
						dump_expr_latex(ex->arg2, tpriority, fp);
					}
					else if (id == get_id("-", funcs)) {
						dump_expr_latex(ex->arg1, tpriority, fp);
						fprintf(fp, " - ");
						dump_expr_latex(ex->arg2, tpriority, fp);
					}
					else if (id == get_id("/", funcs)) {
						fprintf(fp, " \\dfrac{ ");
						dump_expr_latex(ex->arg1, tpriority, fp);
						fprintf(fp, " }{ ");
						dump_expr_latex(ex->arg2, tpriority, fp);
						fprintf(fp, " } ");
					}
					else if (id == get_id("*", funcs)) {
						dump_expr_latex(ex->arg1, tpriority, fp);
						fprintf(fp, " \\cdot ");
						dump_expr_latex(ex->arg2, tpriority, fp);
					}
					else if (id == get_id("^", funcs)) {
						dump_expr_latex(ex->arg1, tpriority, fp);
						fprintf(fp, " ^{ ");
						dump_expr_latex(ex->arg2, tpriority, fp);
						fprintf(fp, " } ");
					}
					else {
						//TODO
					}
					
					if (tpriority <= priority)
						fprintf(fp, " \\right) ");
				}
			
				else if (ex->arg1) {
					fputs(get_name(ex->val.id, funcs).c_str(), fp);
					fprintf(fp, " \\left( ");
					dump_expr_latex(ex->arg1, 0, fp);
					fprintf(fp, " \\right) ");
				}
			}
			break;
			case NUM:
				if (roundable(ex->val.num))
					fprintf(fp, "%ld", lround(ex->val.num));
				else
					fprintf(fp, "%lf", ex->val.num);
			break;
			case VAR:
				fprintf(fp, "%s", get_name(ex->val.id, vars).c_str());
			break;
		}
	}
	
	int dump_latex(FILE* fp)
	{
		assert(fp);
		
		fprintf(fp, "\
			\\documentclass[a4paper,12pt]{article}\
			\\usepackage[utf8x]{inputenc}\
			\\usepackage[english,russian]{babel}\
			\\usepackage{amsmath}\
			\\usepackage{breqn}\
			\\usepackage{amsbsy}\
			\\usepackage{graphicx}\
			\\usepackage[normalem]{ulem}\
			\\begin{document}\
			\\begin{dmath*}\
			$$");
		dump_expr_latex(root, 0, fp);
		fprintf(fp, "$$\
			\\end{dmath*}\
			\\end{document}");
		
		return 0;
	}
	
	int dump_expr(const expr_t* ex, int priority, FILE* fp)
	{
		switch (ex->val.type) {
			case TOKEN:
				fprintf(fp, "%s", get_name(ex->val.id, tokens).c_str());
			break;
			case FUNC:
			{
				int id = ex->val.id;
				
				if (ex->arg1 && ex->arg2) {
					int tpriority = op_priority(ex->val);
					
					if (tpriority <= priority)
						fprintf(fp, "(");
					
					dump_expr(ex->arg1, tpriority, fp);
					fputs(get_name(ex->val.id, funcs).c_str(), fp);
					dump_expr(ex->arg2, tpriority, fp);
					
					if (tpriority <= priority)
						fprintf(fp, ")");
				}
				else if (ex->arg1) {
					fputs(get_name(ex->val.id, funcs).c_str(), fp);
					fprintf(fp, "(");
					dump_expr(ex->arg1, 0, fp);
					fprintf(fp, ")");
				}
			}
			break;
			case NUM:
				if (roundable(ex->val.num))
					fprintf(fp, "%ld", lround(ex->val.num));
				else
					fprintf(fp, "%lf", ex->val.num);
			break;
			case VAR:
				fprintf(fp, "%s", get_name(ex->val.id, vars).c_str());
			break;
		}
		
		return 0;
	}
	
	int dump(FILE* fp) 
	{
		assert(fp);
		
		return dump_expr(root, 0, fp);
	}
	
	int reduce()
	{
		return reduce_expr(&root);
	}
	
	Expression* derivative(const string& var)
	{
		auto it = find(vars.begin(), vars.end(), var);
		return new Expression(
			derivative_expr(root, it - vars.begin()), 
			vars
		);
	}
	
	void substitute(const string& var, const double val)
	{
		auto it = find(vars.begin(), vars.end(), var);
		
		if (it != vars.end()) {
			substitute_expr(root, it - vars.begin(), val);
		}
	}
	
	~Expression()
	{
		purge_expr(root);
	}
};
