#include <cstdio>
#include <cstring>
#include <cassert>
#include <cctype>

#include <tree.hpp>

using namespace std;

int main()
{
	const char* vars[] = {"x", "delta"};
	parse_data pd = {"1 + 2 * sin(3) * cos(5 - 4)", vars, SIZE(vars)};
	expr* ex = nullptr;
	
	parse(&pd, &ex);
	
	FILE* out = fopen("dif.dot", "w");
	
	dump_expr_dot(ex, out);
	
	fclose(out);
	
	purge_expr(ex);
}
