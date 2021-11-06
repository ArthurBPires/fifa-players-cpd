#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ALPHABET_SIZE 27 //26 + space
#define NAME_SIZE 80

typedef struct DATA
{
	unsigned int sofifa_id;
	char name[NAME_SIZE];
	char player_pos[3][4];
	float rating;
	unsigned int count;
}Data;

typedef struct TRIENODE
{
	struct TRIENODE *children[ALPHABET_SIZE];

	Data *data;
}TrieNode;

TrieNode* newNode(void);
int chatoi(char c);
void insert(TrieNode *root, Data *data);
Data *search(TrieNode *root, const char *key);
int isTrieLeaf(TrieNode *root);
void smartSearchRec(TrieNode *root, char *key);
void smartSearch(TrieNode* root, char *key);
void insertData(TrieNode *root, char *fileName);