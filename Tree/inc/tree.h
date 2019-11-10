#pragma once

#include <cinttypes>
#include <cstdio>
#include <cassert>
#include <cstring>

using namespace std;

#define MAX_DATA_LEN 64

struct bnode 
{
	const char* data;

	bnode* left;
	bnode* right;
};

inline bnode* create_node(const char* data)
{
	assert(data);
	
	char* copy = new char[strlen(data) + 1];
	strcpy(copy, data);
	
	return new bnode {copy, nullptr, nullptr};
}

inline int purge(bnode* node)
{
	if (!node) return 0;
	
	if (node->left) purge(node->left);
	if (node->right) purge(node->right);
	
	delete[] node->data;
	delete node;
	
	return 0;
}

inline int split(bnode* node, const char* ndata, const char* rdata)
{
	assert(node);
	assert(ndata);
	assert(rdata);
	
	if (node->left || node->right) return 1;
	
	char* copy = new char[strlen(ndata) + 1];
	strcpy(copy, ndata);
	
	node->left = new bnode {node->data, nullptr, nullptr};
	node->data = copy;
	node->right = create_node(rdata);
	
	return 0;
}

inline int parse_tree(bnode** tree, FILE* fp)
{
    assert(tree);
    assert(file);
    
	int counter = 0;
	fscanf(fp, " nil %n", &counter);
	
	if (counter) {
        *tree = nullptr;
        
        return 0;
    }
	
    counter = 0;
	fscanf(fp, " { %n", &counter);
	
	if (!counter) {
		printf("## Error parsing: expected nil or { \n");
		
		return 1;
	}
	
	char* data = new char[MAX_DATA_LEN + 1];
	
    counter = 0;
	fscanf(fp, " %s %n", data, &counter);
	
	if (counter > MAX_DATA_LEN) {
		printf("## Error parsing: too large string \n");
		
		delete[] data;
		
		return 1;
	}
	
	*tree = create_node(data);
	delete[] data;
	
#define PARSE_CHILD(CHILD)	 					\
	error = parse_tree(&((*tree)->CHILD), fp);	\
	if (error) return error; 					\
    
    int error = 0;
	PARSE_CHILD(left)
	PARSE_CHILD(right)
	
	counter = 0;
	fscanf(fp, " } %n", &counter);
	
	if (!counter) {
		printf("## Error parsing: expected } \n");
		
		return 2;
	}

    return 0;
}

inline int dump(const bnode* node, FILE* fp)
{	
	if (!node) {
		fprintf(fp, " nil ");
		
		return 0;
	}
	
	fprintf(fp, " { %s ", node->data);
	
	if (node->left) dump(node->left, fp);
	else fprintf(fp, " nil ");
	
	if (node->right) dump(node->right, fp);
	else fprintf(fp, " nil ");
	
	fprintf(fp, " } ");
	
	return 0;
}




