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
	
	char* str = read_file_str(argv[1]);
	
	bnode* tree = parse(str);
	
	free(str);
	
	if (!tree) {
		printf("## Error parsing\n");
		
		return 0;
	}
	
	bnode* node = tree;
	
	while (1) {
		if (!node->right || !node->left) {
			printf("Is it %s?\n", node->data);
			
			char c = 0;
			scanf(" %c ", &c);
			
			if (c == 'y') {
				printf("I won!\n");
				
				purge(tree);
				
				return 0;
			}
			
			printf("But what is it? How is it different from %s?\n", node->data);
			
			#define MAXLEN 128
			
			char* name = new char[MAXLEN];
			char* question = new char[MAXLEN];
			
			scanf("%s %s", name, question);
			
			split(node, question, name);
			
			printf("|%s|\n", node->data);
			
			delete[] name;
			delete[] question;
			
			FILE* out = fopen(argv[1], "w");
	
			dump(tree, out);
			
			fclose(out);
			
			purge(tree);
			
			return 0;
		}
		else {
			printf("%s?\n", node->data);
			
			int c = getchar();
			
			if (c == 'y') 
				node = node->right;
			else
				node = node->left;
		}
	}
}
