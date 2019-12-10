#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <algorithm>
#include <exception>
#include <iterator>

#include "Node.hpp"
#include "Parser.hpp"

using namespace std;

void push_expr(const Node* tree, const vector<string>& vars, ofstream& out);

string get_substr(const Node* tree)
{
	string retval;
	for (const auto c: tree->childs) {
		assert(c->childs.size() == 0);
		
		retval += c->data;
	}
	
	return retval;
}

inline string get_id(const Node* id) 
{
	assert(id->data == "ID");
	
	return get_substr(id);
}

inline string get_num(const Node* num)
{
	assert(num->data == "NUM");
	
	return get_substr(num);
}

inline int get_offset(const Node* tree, const vector<string>& vars)
{
	assert(tree->data == "ID");

	string id = get_id(tree);
			
	auto it = find(vars.begin(), vars.end(), id);
	if (it == vars.end()) 
		throw invalid_argument("Unknown var: " + id);
	
	return distance(vars.begin(), it);
}

inline void get_idlist(const Node* tree, vector<string>& ids)
{
	assert(tree->data == "IDLIST");
	
	for (int i = 0; i < tree->childs.size(); i += 2) {
		assert(	(i == tree->childs.size() - 1) ||
				(tree->childs[i + 1]->data == ","));
		ids.push_back(get_id(tree->childs[i]));
	}
}

inline void count_offset(int offset, ofstream& out)
{
	out << "PUSH CONSTANT " << offset << "\n";
	out << "PUSH REGISTER 0\n";
	out << "ADD\n";
}

inline void pop_to_local(int offset, ofstream& out)
{
	count_offset(offset, out);
	out << "POP LOCAL INDEX\n";
}

inline void push_from_local(int offset, ofstream& out)
{
	count_offset(offset, out);
	out << "PUSH LOCAL INDEX\n";
}

inline void pop_to_idlist(const Node* tree,
	const vector<string>& vars,	ofstream& out)
{
	assert(tree->data == "IDLIST");
	
	for (int i = 0; i < tree->childs.size(); i += 2) {
		assert(	(i == tree->childs.size() - 1) ||
				(tree->childs[i + 1]->data == ","));
		
		pop_to_local(get_offset(tree->childs[i], vars), out);
	}
}

inline void push_num(Node* tree, ofstream& out)
{
	assert(tree->data == "NUM");
	
	out << "PUSH CONSTANT " << get_num(tree) << "\n";
}

inline void push_a_part(const Node* tree,
	const vector<string>& vars,	ofstream& out)
{
	assert(tree->data == "A");
	assert(tree->childs.size() > 0);
	assert(tree->childs.size() < 4);
	
	if (tree->childs.front()->data == "NUM") {
		push_num(tree->childs.front(), out);
		
		return;
	}
	
	if (tree->childs.size() == 3) {
		assert(tree->childs.front()->data == "(");
		assert(tree->childs.back()->data == ")");
		
		push_expr(tree->childs[1], vars, out);
		
		return;
	}
	
	push_from_local(get_offset(tree->childs.back(), vars), out);
	
	if (tree->childs.size() > 1) {
		if (tree->childs.front()->data == "-") {
			out << "PUSH CONSTANT 0\n";
			out << "SUB\n";
		}
		else assert(tree->childs.front()->data == "+");
	}
}

inline void push_t_part(const Node* tree,
	const vector<string>& vars,	ofstream& out)
{
	assert(tree->data == "T");
	assert(tree->childs.size() > 0);
	
	for (int i = 1; i < tree->childs.size(); i += 2) {
		assert(tree->childs.size() > i + 1);
		
		if (tree->childs[i]->data == "/") 
			push_a_part(tree->childs[i + 1], vars, out);
	}
	
	push_a_part(tree->childs.front(), vars, out);
	
	for (int i = 1; i < tree->childs.size(); i += 2) {
		assert(tree->childs.size() > i + 1);
		
		if (tree->childs[i]->data == "/") {
			out << "DIV\n";
		}
		else {
			assert(tree->childs[i]->data == "*");
			
			push_a_part(tree->childs[i + 1], vars, out);
			out << "MUL\n";
		}
	}
}

inline void push_expr(const Node* tree,
	const vector<string>& vars,	ofstream& out)
{
	assert(tree->data == "EXPR");
	assert(tree->childs.size() > 0);
	
	for (int i = 1; i < tree->childs.size(); i += 2) {
		assert(tree->childs.size() > i + 1);
		
		if (tree->childs[i]->data == "-") 
			push_t_part(tree->childs[i + 1], vars, out);
	}
	
	push_t_part(tree->childs.front(), vars, out);
	
	for (int i = 1; i < tree->childs.size(); i += 2) {
		assert(tree->childs.size() > i + 1);
		
		if (tree->childs[i]->data == "-") {
			out << "SUB\n";
		}
		else {
			assert(tree->childs[i]->data == "+");
			
			push_t_part(tree->childs[i + 1], vars, out);
			out << "ADD\n";
		}
	}
}

inline void push_expr_list(const Node* tree,
	const vector<string>& vars,	ofstream& out)
{
	assert(tree->data == "EXPRLIST");
	
	for (int i = 0; i < tree->childs.size(); i += 2) {
		assert(	(i == tree->childs.size() - 1) ||
				(tree->childs[i + 1]->data == ","));
		push_expr(tree->childs[i], vars, out);
	}
}

inline void generate_call_asm(const Node* tree, 
	const vector<string>& vars,	ofstream& out)
{
	assert(tree->data == "CALL");
}

inline void generate_statement_asm(const Node* tree, 
	const vector<string>& vars,	ofstream& out)
{
	assert(tree->data == "STATEMENT");
	assert(tree->childs.size() == 4);
	assert(tree->childs[1]->data == "=");
	assert(tree->childs[3]->data == ";");
	
	push_expr_list(tree->childs[2], vars, out);
	pop_to_idlist(tree->childs[0], vars, out);
}

inline void generate_return_asm(const Node* tree, 
	const vector<string>& vars,	ofstream& out)
{
	assert(tree->data == "RETSTATEMENT");
}

inline void generate_if_asm(const Node* tree, 
	const vector<string>& vars,	ofstream& out)
{
	assert(tree->data == "IFSTATEMENT");
}

inline void generate_block_asm(const Node* tree, 
	const vector<string>& vars,	ofstream& out)
{
	assert(tree->childs.front()->data == "{");
	assert(tree->childs.back()->data == "}");
	
	cout << "BLOCK\n";
	
	for (int i = 1; i < tree->childs.size() - 1; ++i) {
		if (tree->childs[i]->data == "CALL") {
			cout << "CALL\n";
			
			generate_call_asm(tree->childs[i], vars, out);
		}
		else if (tree->childs[i]->data == "STATEMENT") {
			cout << "STATEMENT\n";
			
			generate_statement_asm(tree->childs[i], vars, out);
		}
		else if (tree->childs[i]->data == "IFSTATEMENT") {
			cout << "IFSTATEMENT\n";
			
			generate_if_asm(tree->childs[i], vars, out);
		}
		else if (tree->childs[i]->data == "RETSTATEMENT") {
			cout << "RETSTATEMENT\n";
			
			generate_return_asm(tree->childs[i], vars, out);
		}
		else assert(0); // shouldn't be here
	}
}

inline void generate_func_asm(const Node* tree, ofstream& out) 
{
	assert(tree);
	assert(tree->data == "FUNCTION");
	assert(tree->childs.size() >= 5);
	assert(tree->childs[0]->data == "def");
	
	string id = get_id(tree->childs[1]);
	
	cout << "FUNCTION " + id + "\n";
	
	assert(tree->childs[2]->data == "(");
	
	vector<string> args;
	int skip = 0;
	
	if (tree->childs[3]->data == "IDLIST") {
		assert(tree->childs.size() >= 6);
		
		get_idlist(tree->childs[3], args);
		skip = 1;
	}
	
	assert(tree->childs[3 + skip]->data == ")");
	
	vector<string> vars;
	if (tree->childs[4 + skip]->data == ":") {
		assert(tree->childs.size() >= 7 + skip);
		assert(tree->childs[5 + skip]->data == "IDLIST");
		
		get_idlist(tree->childs[5 + skip], vars);
		skip += 2;
	}
	
	assert(tree->childs[4 + skip]->data == "BLOCK");
	
	out << "LABEL " << id << ":\n";
	for (int i = 0; i < args.size(); ++i) {
		out << "; arg - " << args[i] << "\n";
		pop_to_local(i, out);
		out << "; ---------\n";
	}
	
	vars.insert(vars.begin(), args.begin(), args.end());
	
	generate_block_asm(tree->childs[4 + skip], vars, out);
	
	out << "RETURN\n";
}

inline void generate_asm(const Node* tree, ofstream& out) 
{
	for (auto& func: tree->childs) {
		generate_func_asm(func, out);
	}
}

int main() {
	ifstream in("prog.cn");
	
	string tmp, data;
	while (getline(in, tmp)) data += tmp;
	
	in.close();
	
	Parser parser(data.c_str());
	auto tree = parser.parsePROGRAM();
	if (tree) {
		ofstream out_dot("tree.dot");
		dump_tree(*tree, out_dot);
		out_dot.close();
		
		ofstream out_asm("prog.asm");
		generate_asm(*tree, out_asm);
		out_asm.close();
		
		purge_tree(*tree);
	}
	else {
		cout << "Oh shit oh fuck" << endl;
	}
}
