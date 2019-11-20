#include <cstdio>
#include <cstring>
#include <cassert>
#include <cctype>

#include "expression.hpp"
#include "parser.hpp"

using namespace std;

int main()
{
	Parser p(" sin(cos(tan(x ^ x))) ");
	p.add_var("x");
	p.add_var("y");
	
	auto ex = p.process();
	
	if (!ex) {
		cout << "Error parsing\n";
		
		return 0;
	}
	
	(*ex)->reduce();
	(*ex)->dump("dif.dot");
	
	delete *ex;
}
