#pragma once

#include <cinttypes>
#include <cstdio>
#include <cassert>
#include <cstring>

using namespace std;

#define MAX_DATA_LEN 128

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
	
	if (!counter) return 1;
	
	char data[MAX_DATA_LEN + 1];
	
    counter = 0;
	fscanf(fp, " \"%[^\"]\" %n", data, &counter); // FIXME: MEMORY CORRUPTION
	
	*tree = create_node_copy(data);
	
#define PARSE_CHILD(CHILD)	 					\
	error = parse_tree(&((*tree)->CHILD), fp);	\
	if (error) return error;
    
    int error = 0;
	PARSE_CHILD(left)
	PARSE_CHILD(right)
	
	counter = 0;
	fscanf(fp, " } %n", &counter);
	
	if (!counter) return 2;

    return 0;
}

inline int dump_inner_dot(const bnode* node, FILE* fp)
{
    assert(node);
    assert(fp);
    
    if (!node->left && !node->right) {
        fprintf(fp, "NODE%p [shape=ellipse label=\"%s\"]\n", 
			node, node->data);
        
        return 0;
    }
    
    if (node->left && node->right) {
        fprintf(fp, "NODE%p [shape=box label=\"%s\"]\n", 
			node, node->data);
        
        fprintf(fp, "NODE%p -> NODE%p [label=\"no\"]\n", 
            node, node->left);
            
        fprintf(fp, "NODE%p -> NODE%p [label=\"yes\"]\n", 
            node, node->right);
        
        int error = dump_inner_dot(node->left, fp);
        if (error) return error;
        
        error = dump_inner_dot(node->right, fp);
        return error;
    }
    
    return 1;
}

inline int dump_tree_dot(const bnode* tree, FILE* fp)
{
    assert(tree);
    assert(fp);
    
    fprintf(fp, "digraph Tree {\n");
    int error = dump_inner_dot(tree, fp);
    fprintf(fp, "}\n");
    
    return error;
}

inline int dump_tree(const bnode* tree, FILE* fp)
{	
	if (!tree) {
		fprintf(fp, " nil ");
		
		return 0;
	}
	
	fprintf(fp, " { \"%s\" ", tree->data);
	
	if (tree->left) dump_tree(tree->left, fp);
	else fprintf(fp, " nil ");
	
	if (tree->right) dump_tree(tree->right, fp);
	else fprintf(fp, " nil ");
	
	fprintf(fp, " } ");
	
	return 0;
}

struct bn_entry
{
	int positive;
	
	bnode* data;
	
	bn_entry* next;
};

inline int get_path(bnode* tree, const char* data, bn_entry** path)
{
	assert(data);
	
	if (!tree) return 0;
	
	if (strcmp(tree->data, data) == 0) {
		*path = new bn_entry {-1, tree, nullptr};
		
		return 1;
	}
	
	bn_entry* dpath = nullptr;
	
	if (get_path(tree->left, data, &dpath)) {
		*path = new bn_entry {0, tree, dpath};
		
		return 1;
	}
	
	if (get_path(tree->right, data, &dpath)) {
		*path = new bn_entry {1, tree, dpath};
		
		return 1;
	}
	
	return 0;
}

inline int get_objects(bnode* tree, bn_entry** objs)
{
	assert(tree);
	assert(objs);
	
	if (!tree->left && !tree->right) {
		*objs = new bn_entry {0, tree, nullptr};
		
		return 1;
	}
	
	get_objects(tree->left, objs);
	
	bn_entry* end = *objs;
	while (end->next) end = end->next;
	
	get_objects(tree->right, &(end->next));
	
	return 1;
}




