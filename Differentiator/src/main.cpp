#include <cstdio>
#include <cstring>
#include <cassert>
#include <cctype>

#include "expression.hpp"
#include "parser.hpp"

using namespace std;

int main()
{
	Parser p("x^0.5");
	p.add_var("x");
	//p.add_var("y");
	
	auto ex = p.process();
	
	if (!ex) {
		cout << "Error parsing\n";
		
		return 0;
	}
	
	//(*ex)->substitute("x", 9.0);
	//(*ex)->reduce();
	//(*ex)->dump_dot("ex.dot");
	//(*ex)->dump_latex("ex.latex");
	auto d = (*ex)->derivative("x");
	(*d)->reduce();
	(*d)->dump(stdout);
	//(*d)->dump_latex("dif.latex");
	
	delete *d;
	delete *ex;
}
