#include <cstdio>
#include <cstring>
#include <cassert>
#include <cctype>

#include <tree.hpp>

using namespace std;

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

int main()
{
	const char* vars[] = {"x", "delta"};
	parse_data pd = {"cos(1) + sin(1) + 1 * (5 + 2) / 35 + x / delta ^ 2", vars, SIZE(vars)};
	expr* ex = nullptr;
	
	parse(&pd, &ex);
	
	//print_expr(ex);
	
	FILE* out = fopen("dif.dot", "w");
	
	dump_expr_dot(ex, out);
	
	fclose(out);
	
	purge_expr(ex);
}
