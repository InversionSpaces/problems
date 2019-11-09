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

inline bnode* create(const char* data)
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
	node->right = create(rdata);
	
	return 0;
}

inline bnode* dparse(const char* dump, int* delta)
{
	int count = 0;	
	sscanf(dump, " nil %n", &count);
	
	if (count) {
		*delta = count;
		
		return nullptr;
	}
	
	sscanf(dump, " { %n", &count);
	
	if (!count) {
		printf("## Error parsing: expected nil or { \n");
		
		return nullptr;
	}
	
	int len = 0;
	char* data = new char[MAX_DATA_LEN + 1];
	
	sscanf(dump + count, " %s %n", data, &len);
	
	if (len > MAX_DATA_LEN) {
		printf("## Error parsing: too large string \n");
		
		delete[] data;
		
		return nullptr;
	}
	
	bnode* node = create(data);
	delete[] data;
	
	count += len;
	
	int cdelta = 0;
	
#define PARSE_CHILD(CHILD)							\
	cdelta = 0;										\
	node->CHILD = dparse(dump + count, &cdelta);	\
	if (!cdelta) {									\
		*delta = 0;									\
		purge(node);								\
		return nullptr;								\
	}												\
	count += cdelta;							
	
	PARSE_CHILD(left)
	PARSE_CHILD(right)
	
	int tmp = 0;
	sscanf(dump + count, " } %n", &tmp);
	
	if (!tmp) {
		printf("## Error parsing: expected } \n");
		
		purge(node);
		
		return 0;
	}
	
	count += tmp;
	
	*delta = count;
	
	return node;
}

inline bnode* parse(const char* dump)
{
	int dummy = 0;
	
	return dparse(dump, &dummy);
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




