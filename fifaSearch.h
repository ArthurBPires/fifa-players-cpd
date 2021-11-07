#ifndef _WIN32
#define system("cls") system("clear")
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>

#define ALPHABET_SIZE 27 //26 + space
#define NAME_SIZE 80
#define TAG_SIZE 36

typedef struct DATA
{
	unsigned long sofifa_id;
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

typedef struct HASHTABLE {
	Data *data;
    struct HASHTABLE *next;
}HT;
typedef struct TEXTHASHTABLE {
	char text[TAG_SIZE];
	Data *data;
    struct TEXTHASHTABLE *next;
}textHT;


enum keyType{fifaId,userId,tag,pos};

/*	Trie	*/

TrieNode* newNode(void);
void insertTrie(TrieNode *root, Data *data);
Data* search(TrieNode *root, const char *key);
int isTrieLeaf(TrieNode *root);
void smartSearchRec(TrieNode *root, char *key);
void smartSearch(TrieNode* root, char *key);

/*	Hash	*/

void initHT(HT* hashTable, const unsigned long m);
void textInitHT(textHT* hashTable, const unsigned long m);
unsigned long hash(char *text, const enum keyType type, const unsigned long m, const int p);
void fifaIdinsertHT(HT *hashTable, Data *data, const unsigned long m);
Data* fifaIdsearchHT(HT *hashTable, Data data, const unsigned long m);
void userIdInsertHT(HT *hashTable, Data *data, const unsigned long key);
HT *userIdsearchHT(HT *hashTable, const unsigned long key);
void tagInsertHT(textHT *hashTable, char *key, Data *data, const unsigned long m);
textHT *tagsearchHT(textHT *hashTable, char *key, const unsigned long m);
void multTagsearchHT(textHT *hashTable, char **tagList, const unsigned long m, const int n);
void freeHTCell(HT *hashCell);
void freeHT(HT *hashTable, const unsigned long m);
void printTableTop();
void printData(const Data data);

/*	Geral	*/

int chatoi(const char c);
void insertUsers(TrieNode *root, HT *fifaIdHT, HT *userIdHT, const unsigned long *m, const char *fileName);
void insertPlayers(TrieNode *root, HT *fifaIdHT, const unsigned long *m, const char *fileName);
void insertTags(TrieNode *root, HT *fifaIdHT, textHT *tagHT, const unsigned long *m, const char *fileName);
unsigned long* fineTune(char fileNames[][100]);
void thanoSnap(TrieNode *root, HT *fifaIdHT, HT *userIdHT, const unsigned long *m);
void argOpt(const int argc, char **argv, unsigned long *m, char fileNames[][100]);
