#include <cstdio>
#include <iostream>

#include "tree.h"
#include "files.h"

void clear_input()
{
	int c = 0;
	while ((c = getc(stdin)) != '\n' && c != EOF);
}

int play_akinator(bnode* tree)
{
	assert(tree);
	
	bnode* node = tree;
	
	while (1) {
		if (!node->right || !node->left) {
			printf("## Is it %s?\n", node->data);
			
			char answer = 0;
			scanf(" %c", &answer);
			clear_input();
			
			if (answer == 'y') {
				printf("## I won!\n");
				
				return 0;
			}
			
            size_t size = 0;
            
			char* name = nullptr;
            char* question = nullptr;
			
            printf("## But what is it?\n");
			getline(&name, &size, stdin);
            
            size = 0;
            
            printf("## How is it different from %s?\n", node->data);
			getline(&question, &size, stdin);
            
            printf("## Thanks for the game\n");
            
			split_node(node, question, name);
			
			free(name);
            free(question);
			
			return 1;
		}
		else {
			printf("## %s?\n", node->data);
			
			char answer = 0;
			scanf(" %c%*[^\n]s\n", &answer);
			
			if (answer == 'y') 
				node = node->right;
			else
				node = node->left;
		}
	}
}

int get_tree(bnode** tree, const char* file)
{
	assert(tree);
	assert(file);
	
	FILE* in = fopen(file, "r");
	
	if (!in) return 1;
	
    int error = parse_tree(tree, in);
	
    fclose(in);
    
    return error;
}

int update_tree(bnode* tree, const char* file)
{
	assert(tree);
	assert(file);
	
	FILE* out = fopen(file, "w");
	
	if (!out) return 1;
	
	int error = dump_tree(tree, out);
	
	fclose(out);
	
	return error;
}

int dump_as_dot(bnode* tree, const char* file)
{
	assert(tree);
	assert(file);
	
	FILE* out = fopen(file, "w");
	
	if (!out) return 1;
	
	int error = dump_tree_dot(tree, out);
	
	fclose(out);
	
	return error;
}

int list_objects(bnode* tree)
{
	bn_entry* objs = nullptr;
				
	get_objects(tree, &objs);
	
	bn_entry* tmp = nullptr;
	
	printf("## Objects i have:\n");
	
	while (objs) {
		printf("## |%s|\n", objs->data->data);
		
		tmp = objs;
		
		objs = objs->next;
		
		delete tmp;
	}
	
	return 0;
}

int print_path(bn_entry* path)
{
	bn_entry* tmp = nullptr;
	
	while (path->next) {
		const char* add = path->positive ? "" : "not ";
		
		printf("## %s%s\n", add, path->data->data);
		
		tmp = path;
		path = path->next;
		delete tmp;
	}
	
	delete path;
	
	return 0;
}

int get_object(bnode* tree, char** name, bn_entry** path)
{
	*name = nullptr;
	size_t n = 0;
	
	getline(name, &n, stdin);
	(*name)[strlen(*name) - 1] = 0;
	
	int res = get_path(tree, *name, path);
	
	if (!res) printf("## Object not found, try again!\n");
	
	return res;
}

int show_differences(bnode* tree)
{
	char* fname = nullptr;
	bn_entry* fpath = nullptr;
	
	printf("## Input first object:\n");
	if (!get_object(tree, &fname, &fpath)) return 0;
	
	char* sname = nullptr;
	bn_entry* spath = nullptr;
	
	printf("## Input second object:\n");
	if (!get_object(tree, &sname, &spath)) return 0;
	
	bn_entry* tmp = nullptr;
	
	if (fpath->positive != spath->positive) 
		printf("## There is no similarity!\n");
	else {
		printf("## %s and %s are:\n", fname, sname);
		
		while (fpath && spath && fpath->positive == spath->positive) {
			const char* add = fpath->positive ? "" : "not ";
			
			printf("## %s%s\n", add, fpath->data->data);
			
			tmp = fpath;
			fpath = fpath->next;
			delete tmp;
			
			tmp = spath;
			spath = spath->next;
			delete tmp;
		}
	}
	
	if (spath == nullptr && fpath == nullptr) {
		printf("## There is no differences\n");
	
		return 0;
	}
	
	printf("## Differences are:\n");
	
	printf("## %s is:\n", fname);
	print_path(fpath);
	
	free(fname);
	
	printf("## %s is:\n", sname);
	print_path(spath);
	
	free(sname);
	
	return 0;
}

int characterise_object(bnode* tree)
{
	char* name = nullptr;
	size_t n = 0;
	
	printf("## Input object name:\n");
	
	getline(&name, &n, stdin);
	name[strlen(name) - 1] = 0;
	
	bn_entry* path = nullptr;
	
	if (get_path(tree, name, &path)) {
		printf("## %s is:\n", name);
		
		print_path(path);
	}
	else printf("## No object found, try [l]ist objects");
	
	free(name);
	
	return 0;
}

int dump(bnode* tree)
{
	printf("## Where to dump?\n");

	char* str = nullptr;
	size_t n = 0;
	
	getline(&str, &n, stdin);
	
	return dump_as_dot(tree, str);
}

int play(bnode* tree, const char* fname)
{
	printf("## Let's play!\n");

	if (play_akinator(tree)) {
		printf("## I'll update tree for you..\n");
		
		return update_tree(tree, fname);
	}
	
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("## Hi! You should give me a valid .tr file! =)\n");
		
		return 0;
	}
	
	bnode* tree = blank_tree();
	int error = get_tree(&tree, argv[1]);
	
	if (error) {
		printf("## Error parsing you .tr file! =(\n");
		
		return error;
	}
	
	printf("## I have a tree now!\n");
	
	int working = 1;
	
	while (working) {
		printf("## What do you want to do?\n");
		printf("## [p]lay akinator\n");
		printf("## [c]haracterise some object\n");
		printf("## [s]how difference between objects\n");
		printf("## [l]ist all objects in a tree\n");
		printf("## [d]ump tree as dot\n");
		printf("## [e]xit\n");
		
		char answer = 0;
		scanf("	%c", &answer);
		clear_input();
		
		switch (answer) {
			case 'p':
				play(tree, argv[1]);
				break;
			case 'c':
				characterise_object(tree);
				break;
			case 'l':
				list_objects(tree);
				break;
			case 's':
				show_differences(tree);
				break;
			case 'd':
				dump(tree);
				break;
			case 'e':
				working = 0;
				break;
			default:
				printf("## Unknown option! Try again!\n");
				break;
		}
	}
	
	purge_tree(tree);
	
	return 0;
}
