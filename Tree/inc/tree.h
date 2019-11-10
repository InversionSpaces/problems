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

inline char* copy_str(const char* str)
{
    assert(str);
    
    char* copy = new char[strlen(str) + 1];
	strcpy(copy, str);
    
    return copy;
}

inline bnode* blank_tree()
{
    return nullptr;
}

inline bnode* create_node_copy(const char* data)
{
	assert(data);
	
	return new bnode {copy_str(data), nullptr, nullptr};
}

inline bnode* create_node_from(const char* data)
{
    assert(data);
    
    return new bnode {data, nullptr, nullptr};
}

inline int purge_tree(bnode* tree)
{
	if (!tree) return 0;
	
	if (tree->left) purge_tree(tree->left);
	if (tree->right) purge_tree(tree->right);
	
	delete[] tree->data;
	delete tree;
	
	return 0;
}

inline int split_node(bnode* node, const char* ndata, const char* rdata)
{
	assert(node);
	assert(ndata);
	assert(rdata);
	
	if (node->left || node->right) return 1;
	
	node->left = create_node_from(node->data);
	node->data = copy_str(ndata);
	node->right = create_node_copy(rdata);
	
	return 0;
}

inline int parse_tree(bnode** tree, FILE* fp)
{
    assert(tree);
    assert(fp);
    
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
	
	*tree = create_node_copy(data);
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




