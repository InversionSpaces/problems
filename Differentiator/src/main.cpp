#include <cstdio>
#include <cstring>
#include <cassert>
#include <cctype>

#include <tree.hpp>


using namespace std;

int main()
{
	parse_data pd = {"cos(1) + sin(1)"};
	expr* ex = nullptr;
	
	parse(&pd, &ex);
	
	//print_expr(ex);
	
	FILE* out = fopen("dif.dot", "w");
	
	dump_expr_dot(ex, out);
	
	fclose(out);
	
	purge_expr(ex);
}
