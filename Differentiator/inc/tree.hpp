struct token
{
	int is_num;
	
	union 
	{
		double num;
		const char* str;
	};
};

struct expr
{
	token val;
	
	expr* arg1;
	expr* arg2;
};

inline int dump_inner_dot(const expr* ex, FILE* fp)
{
    assert(ex);
    assert(fp);
    
	if (ex->val.is_num)
		fprintf(fp, "NODE%p [shape=ellipse label=\"%lf\"]\n", ex, ex->val.num);
	else 
		fprintf(fp, "NODE%p [shape=ellipse label=\"%s\"]\n", ex, ex->val.str);
    
    if (ex->arg1) {
        fprintf(fp, "NODE%p -> NODE%p\n", ex, ex->arg1);
        
        dump_inner_dot(ex->arg1, fp);
    }
    
    if (ex->arg2) {
		fprintf(fp, "NODE%p -> NODE%p\n", ex, ex->arg2);
		
        dump_inner_dot(ex->arg2, fp);
	}
    
    return 0;
}

inline int dump_expr_dot(const expr* ex, FILE* fp)
{
    assert(ex);
    assert(fp);
    
    fprintf(fp, "digraph Dif {\n");
    int error = dump_inner_dot(ex, fp);
    fprintf(fp, "}\n");
    
    return error;
}

int purge_expr(expr* ex)
{
	if (ex->arg1) purge_expr(ex->arg1);
	if (ex->arg2) purge_expr(ex->arg2);
	
	delete ex;
	
	return 0;
}

const char* tokens[] = { "(", ")", "+", "-", "*", "**", "/", "sin", "cos" };

#define SIZE(x) (sizeof(x) / sizeof(0[x]))

struct parse_data
{
	const char* input;
	
	const char** vars;
	size_t nvars;
};

void print_expr(const expr* ex)
{	
	if (ex->val.is_num)
		printf(" %lf", ex->val.num);
	else
		printf(" %s", ex->val.str);
	
	if (ex->arg1 && ex->arg2) {	
		printf("( ");
		print_expr(ex->arg1);
		print_expr(ex->arg2);
		printf(" )");
	}
} 

int parse(parse_data* pd, expr** ex);

int parse_token(parse_data* pd, token* tk)
{
	assert(pd);
	assert(pd->input);
	assert(tk);
	
	if (*(pd->input) == 0) return 0;
	
	while (isspace(*(pd->input))) pd->input++;
	
	double num = 0;
	int n = 0;
	
	int res = sscanf(pd->input, "%lf%n", &num, &n);
	
	if (res) {
		pd->input += n;
		
		(*tk).is_num = 1;
		(*tk).num = num;
		
		return 1;
	}
	
	for (size_t i = 0; i < SIZE(tokens); ++i) {
		size_t len = strlen(tokens[i]);
		if (strncmp(tokens[i], pd->input, len) == 0) {
			pd->input += len;
			
			(*tk).is_num = 0;
			(*tk).str = tokens[i];
			
			return 1;
		}
	}
	
	return 0;
}

int parse_expr(parse_data* pd, expr** ex)
{
	assert(pd);
	assert(ex);
	
	token tk = {};
	
	if (!parse_token(pd, &tk)) 
		return 0;
	
	if (tk.is_num) {
		*ex = new expr {tk, nullptr, nullptr};
		
		return 1;
	}
	
	// Parse expr in brackets
	if (strcmp(tk.str, "(") == 0) {
		if (!parse(pd, ex)) 
			return 0;
			
		if (!parse_token(pd, &tk)) 
			return 0;
			
		if (strcmp(tk.str, ")") != 0) 
			return 0;
		
		return 1;
	}
	
	// Parse expression after this one token
	// This is for sin, cos etc
	expr* down_ex = nullptr;
	
	if (!parse_expr(pd, &down_ex)) 
		return 0;
	
	*ex = new expr {tk, down_ex, nullptr};
	
	return 1;
}

inline int op_priority(const token* op)
{
	if (strcmp(op->str, "+") == 0) return 1;
	if (strcmp(op->str, "-") == 0) return 1;
	if (strcmp(op->str, "*") == 0) return 2;
	if (strcmp(op->str, "/") == 0) return 2;
	
	return 0; // For "(" and ")"
}

int parse_bin_expr(parse_data* pd, expr** ex, int mpriority)
{
	assert(pd);
	assert(ex);
	
	expr* left = nullptr;
	
	if (!parse_expr(pd, &left)) return 0;
	
	while (1) {
		*ex = left;
		
		token op = {};
		
		if (!parse_token(pd, &op))
			return 1;
		
		int priority = op_priority(&op);
		
		// Priority defines whether we need to continue parsing
		// or to return parsing to upper function
		
		// "(" or ")" will be dropped here too
		// This is not obvious
		if (priority <= mpriority) { 
			pd->input -= strlen(op.str);
			
			return 1;
		}
		
		expr* right = nullptr;
		if (!parse_bin_expr(pd, &right, priority))
			return 0;
		
		left = new expr {op, left, right};
	}
}

int parse(parse_data* pd, expr** ex)
{
	return parse_bin_expr(pd, ex, 0);
}

