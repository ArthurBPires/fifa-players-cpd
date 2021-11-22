#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#ifdef _WIN32
#include <processthreadsapi.h>
#endif

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

typedef struct EVAL{
	unsigned long sofifa_id;
	float rating;
}Eval;

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

typedef struct FLOATHASHTABLE {
	float userRating;
	Data *data;
    struct FLOATHASHTABLE *next;
}floatHT;


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
void floatInitHT(floatHT* hashTable, const unsigned long m);
unsigned long hash(char *text, const enum keyType type, const unsigned long m, const int p);
void fifaIdinsertHT(HT *hashTable, Data *data, const unsigned long m);
Data* fifaIdsearchHT(HT *hashTable, Data data, const unsigned long m);
Data *fifaIdsearchHTbysofifaID(HT *hashTable, unsigned long sofifa_id, unsigned long m);
void userIdInsertHT(floatHT *hashTable, Data *data, const unsigned long key, float userRating);
floatHT *userIdsearchHT(floatHT *hashTable, const unsigned long key);
void tagInsertHT(textHT *hashTable, char *key, Data *data, const unsigned long m);
textHT *tagsearchHT(textHT *hashTable, char *key, const unsigned long m);
void multTagsearchHT(textHT *hashTable, char **tagList, const unsigned long m, const int n);
void freeHTCell(HT *hashCell);
void freeHT(HT *hashTable, const unsigned long m);
void textFreeHTCell(textHT *hashCell);
void textFreeHT(textHT *hashTable, const unsigned long m);
void floatFreeHTCell(floatHT *hashCell);
void floatFreeHT(floatHT *hashTable, const unsigned long m);
void printTableTop(int breakLine);
void printData(const Data data, int breakLine);

/*	Geral	*/

int chatoi(const char c);
void insertUsers(TrieNode *root, HT *fifaIdHT, floatHT *userIdHT, const unsigned long *m, const char *fileName);
void insertPlayers(TrieNode *root, HT *fifaIdHT, const unsigned long *m, const char *fileName);
void insertTags(TrieNode *root, HT *fifaIdHT, textHT *tagHT, const unsigned long *m, const char *fileName);
//unsigned long* fineTune(char fileNames[][100]);
void thanoSnap(TrieNode *root, HT *fifaIdHT, floatHT *userIdHT, textHT *tagHT, const unsigned long *m);
void argOpt(const int argc, char **argv, unsigned long *m, char fileNames[][100]);
void quicksort(Data **data, int low, int high);
void quickSortEval(Eval evals[], int low, int high);
int hasTag(Data* data, char* tag);
void stringUpperCase(char* str);