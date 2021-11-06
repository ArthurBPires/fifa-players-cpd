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

typedef struct HASHTABLE {
	Data *data;
    struct HASHTABLE *next;
}HT;
typedef struct KEYHASHTABLE {
	int key;
	Data *data;
    struct KEYHASHTABLE *next;
}userHT;


enum keyType{fifaId,userId,pos,rating,tag};
//typedef enum Keytype enum keyType;

//Trie
TrieNode* newNode(void);
void insertTrie(TrieNode *root, Data *data);
Data *search(TrieNode *root, const char *key);
int isTrieLeaf(TrieNode *root);
void smartSearchRec(TrieNode *root, char *key);
void smartSearch(TrieNode* root, char *key);
//Hash
void fifaIdinitHT(HT* hashTable, int m);
void userIdinitHT(userHT* hashTable, int m);
unsigned long long hash(int hash, enum keyType type, int m, int p);
void fifaIdinsertHT(HT *hashTable, Data *data, int m);
Data *fifaIdsearchHT(HT *hashTable, Data data, int m);
void userIdInsertHT(userHT *hashTable, Data *data, const unsigned int key, int m);
userHT *userIdsearchHT(userHT *hashTable, Data data, const unsigned int key, int m);
void freeIdHTCell(HT *hashCell);
void freeIdHT(HT *hashTable, int m);
void printData(Data data);
//Geral
int chatoi(char c);
void insertUsers(TrieNode *root, HT *fifaIdHT, userHT *userIdHT, int fifam, int userm, int p, char *fileName);
void insertPlayers(TrieNode *root, HT *fifaIdHT, int m, int p, char *fileName);
