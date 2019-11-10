#include <cstdio>
#include <iostream>

#include "tree.h"
#include "files.h"


int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("## Expected filename as arg\n");
		
		return 0;
	}
	
	bnode* tree = blank_tree();
    
    FILE* in = fopen(argv[1], "r");
	
    int error = parse_tree(&tree, in);
	
    fclose(in);
	
	if (error) {
		printf("## Error parsing\n");
		
		return error;
	}
	
	bnode* node = tree;
	
	while (1) {
		if (!node->right || !node->left) {
			printf("Is it %s?\n", node->data);
			
			char answer = 0;
			scanf(" %c%*s", &answer);
			
			if (answer == 'y') {
				printf("I won!\n");
				
				purge_tree(tree);
				
				return 0;
			}
			
			#define MAXLEN 128
			
			char* name = new char[MAXLEN];
			char* question = new char[MAXLEN];
			
            printf("But what is it?\n");
			scanf(" %s", name);
            
            printf("How is it different from %s?\n", node->data);
			scanf(" %s", question);
            
            printf("Thanks for the game\n");
            
			split_node(node, question, name);
			
			delete[] name;
			delete[] question;
			
			FILE* out = fopen(argv[1], "w");
	
			dump(tree, out);
			
			fclose(out);
			
			purge_tree(tree);
			
			return 0;
		}
		else {
			printf("%s?\n", node->data);
			
			char answer = 0;
			scanf(" %c%*s", &answer);
			
			if (answer == 'y') 
				node = node->right;
			else
				node = node->left;
		}
	}
}
