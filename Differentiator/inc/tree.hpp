enum token_type
{
	VAR,
	FUNC,
	TOKEN,
	NUM
};

struct token
{
	token_type type;
	
	union 
	{
		int id;
		double num;
	};
};

struct expr
{
	token val;
	
	expr* arg1;
	expr* arg2;
};

#define SIZE(x) (sizeof(x) / sizeof(0[x]))

const char* tokens[] = {"(", ")"};
const char* functions[] = { "+", "-", "*", 
							"^", "/", "sin", 
							"cos", "tg", "sh", 
							"ch", "abs" };

inline int get_token_id(const char* str)
{
	for (int i = 0; i < SIZE(tokens); ++i)
		if (strcmp(str, tokens[i]) == 0) return i;
	
	return -1;
}

inline int get_function_id(const char* str)
{
	for (int i = 0; i < SIZE(functions); ++i)
		if (strcmp(str, functions[i]) == 0) return i;
	
	return -1;
}

struct parse_data
{
	const char* input;
	
	const char** vars;
	size_t nvars;
};

inline int dump_inner_dot(const expr* ex, FILE* fp)
{
    assert(ex);
    assert(fp);
    
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

int parse(parse_data* pd, expr** ex);

int find_in_array(parse_data* pd, token* tk, const char** arr, size_t n)
{
	for (size_t i = 0; i < n; ++i) {
		size_t len = strlen(arr[i]);
		if (strncmp(arr[i], pd->input, len) == 0) {
			pd->input += len;
			
			tk->id = i;
			
			return 1;
		}
	}
	
	return 0;
}

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
		tk->type = NUM;
		tk->num = num;
		
		return 1;
	}
	
	
	if (find_in_array(pd, tk, functions, SIZE(functions))) {
		tk->type = FUNC;
		
		return 1;
	}
	
	if (find_in_array(pd, tk, tokens, SIZE(tokens))) {
		tk->type = TOKEN;
		
		return 1;
	}
	
	if (find_in_array(pd, tk, pd->vars, pd->nvars)) {
		tk->type = VAR;
		
		return 1;
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
	
	if (tk.type == NUM || tk.type == VAR) {
		*ex = new expr {tk, nullptr, nullptr};
		
		return 1;
	}
	
	// Parse expr in brackets
	if (tk.type == TOKEN && tk.id == 0) { // "(" id
		if (!parse(pd, ex)) 
			return 0;
			
		if (!parse_token(pd, &tk)) 
			return 0;
			
		if (tk.type != TOKEN || tk.id != 1) // ")" id 
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
	if (op->type == FUNC) {
		if (op->id == get_function_id("+")) return 1;
		if (op->id == get_function_id("-")) return 1;
		if (op->id == get_function_id("*")) return 2;
		if (op->id == get_function_id("/")) return 2;
		if (op->id == get_function_id("^")) return 3;
	}
	
	return 0; // For tokens
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
			pd->input -= strlen(op.type == TOKEN ? tokens[op.id] : functions[op.id]);
			
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

