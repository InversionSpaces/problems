#include <iostream>
#include <cassert>
#include <cstdio>
#include <cctype>
#include <string>
#include <cstring>
#include <utility>
#include <map>

#include "expression.hpp"
#include "parser.hpp"

using namespace std;

char* command = nullptr;
size_t csize = 0;

Parser parser("");
map<string, Expression*> exprs;

int set_command(const char* cmd)
{
	const int cmd_len = strlen(cmd);
	int skip = 0;
	char name[cmd_len + 1] = {};
	
	sscanf(cmd, " %s = %n ", name, &skip);
	
	if (!skip) {
		printf("## Expected name and '='\n");
		
		return 1;
	}
	
	cmd += skip;
	skip = 0;
	
	char vname[cmd_len + 1] = {};
	int num = 0;
	
	sscanf(cmd, "%i derivative of %n", &num, &skip);
	if (skip) {
		if (num < 1) {
			printf("## Derivative must be of 1 or bigger order\n");
			
			return 1;
		}
		
		cmd += skip;
		skip = 0;
		
		sscanf(cmd, " %s %n", vname, &skip);
		if (!skip) {
			printf("## Expected name\n");
			
			return 1;
		}
		cmd += skip;
		skip = 0;
		
		auto it = exprs.find(vname);
		if (it == exprs.end()) {
			printf("## No expression with name %s\n", vname);
			
			return 1;
		}
		
		sscanf(cmd, " by %s %n", vname, &skip);
		if (!skip) {
			printf("## Expected variable name\n");
			
			return 1;
		}
		
		Expression* deriv = it->second->derivative(vname);
		for (int i = 0; i < num - 1; ++i) {
			Expression* tmp = deriv->derivative(vname);
			delete deriv;
			deriv = tmp;
		}
		
		it = exprs.find(name);
		if (it != exprs.end()) {
			delete it->second;
			it->second = deriv;
		}
		else {
			it = exprs.insert(make_pair(string(name), deriv)).first;
		}
		
		it->second->reduce();
		
		printf("## %s: ", name);
		it->second->dump(stdout);
		printf("\n");
		
		return 1;
	}
	
	sscanf(cmd, "%i parameters expression: %n", &num, &skip);
	if (skip) {
		cmd += skip;
		parser.reset_vars();
		
		for (int i = 0; i < num; ++i) {
			if (!sscanf(cmd, " %s %n", vname, &skip)) {
				printf("## Too few parameters given\n");
				
				return 1;
			}
			
			parser.add_var(vname);
			cmd += skip;
		}
		
		parser.reset_input(cmd);
		
		auto ex = parser.process();
		if (!ex) {
			printf("## Could not parse expression\n");
			
			return 1;
		}
		
		auto it = exprs.find(name);
		if (it != exprs.end()) {
			delete it->second;
			it->second = *ex;
		}
		else {
			it = exprs.insert(make_pair(string(name), *ex)).first;
		}
		
		it->second->reduce();
		
		printf("## %s: ", name);
		it->second->dump(stdout);
		printf("\n");
		
		return 1;
	}
	
	sscanf(cmd, " %s %n", vname, &skip);
	if (skip) {
		auto right = exprs.find(vname);
		if (right == exprs.end()) {
			printf("## No expression with name %s\n", vname);
			
			return 1;
		}
		
		auto left = exprs.find(name);
		if (left != exprs.end()) {
			delete left->second;
			left->second = new Expression(*right->second);
		}
		else
			exprs[name] = new Expression(*right->second);
		
		return 1;
	} 
	
	printf("## Unrecognized parameters fot set command\n");
	
	return 1;
}

enum pmode {
	DOT,
	PRINT,
	LATEX
};

int print_command(const char* cmd, pmode mode)
{
	size_t cmd_len = strlen(cmd);
	char vname[cmd_len + 1] = {};
	
	if (!sscanf(cmd, " %s ", vname)) {
		printf("## Expected name\n");
		
		return 1;
	} 
	
	auto it = exprs.find(vname);
	if (it == exprs.end()) {
		printf("## No expression with name %s\n", vname);
		
		return 1;
	}
	
	switch (mode) {
		case DOT: {
			FILE* fp = fopen("/tmp/expr.dot", "w");
			
			if (!fp) {
				printf("## Could not dump\n");
				
				return 1;
			}
			
			it->second->dump_dot(fp);
			
			fclose(fp);
			
			system("dot -Tpng -o /tmp/expr.png /tmp/expr.dot > /dev/null");
			system("bash -c \"xdg-open /tmp/expr.png\" 2>/dev/null");
		}
		break;
		case LATEX: {
			FILE* fp = fopen("/tmp/expr.tex", "w");
			
			if (!fp) {
				printf("## Could not dump\n");
				
				return 1;
			}
			
			it->second->dump_latex(fp);
			
			fclose(fp);
			
			system("pdflatex -output-directory=/tmp /tmp/expr.tex > /dev/null");
			system("bash -c \"xdg-open /tmp/expr.pdf\" 2>/dev/null");
		}
		break;
		case PRINT: {
			printf("## %s: ", vname);
			it->second->dump(stdout);
			printf("\n");
		}
		break;
	}
	
	return 1;
}

int process_command()
{
	if (getline(&command, &csize, stdin) <= 0) {
		return 1;
	}
	
	int n = 0;
	sscanf(command, " set %n", &n);
	if (n) {
		return set_command(command + n);
	}
	
	sscanf(command, " print %n", &n);
	if (n) {
		return print_command(command + n, PRINT);
	}
	
	sscanf(command, " latex %n", &n);
	if (n) {
		return print_command(command + n, LATEX);
	}
	
	sscanf(command, " tree %n", &n);
	if (n) {
		return print_command(command + n, DOT);
	}
	
	sscanf(command, " exit %n", &n);
	if (n) {
		return 0;
	}
	
	printf("## Unrecognized command\n");
	
	return 1;
}

int main()
{
	printf("## Differentiator by InversionSpaces\n");
	
	do {
		printf("##: ");
	} while (process_command());
	
	printf("## Exiting\n");
	
	if (csize) free(command);
	
	for (auto it = exprs.begin(); it != exprs.end(); it++) 
		delete it->second;
	
	return 0;
}
