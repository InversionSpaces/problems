#include <cstdio>
#include <cstring>
#include <cassert>
#include <cctype>

//#include <tree.hpp>
#include "expression.hpp"
#include "parser.hpp"


using namespace std;

int main()
{
	Parser p("1 + 2 + sin(1) / cos( 5 * 8 )");
	
	auto ex = p.process();
	
	(*ex)->dump("dif.dot");
	
	delete *ex;
}
