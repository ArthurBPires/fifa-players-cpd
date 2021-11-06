#include "fifaSearch.h"

/*	TRIE	*/

//Retorna um ponteiro para um novo nodo na Trie
TrieNode* newNode(void)
{
	TrieNode *newNode = NULL;

	newNode = (TrieNode *)malloc(sizeof(TrieNode));

	if (newNode)
	{
		newNode->data = NULL;
		for (int i = 0; i < ALPHABET_SIZE; i++)
			newNode->children[i] = NULL;
	}

	return newNode;
}

//Realiza a inserção na Trie
void insertTrie(TrieNode *root, Data *data)
{
	TrieNode *finder = root;
	char key[NAME_SIZE];
	int index,i,keySize = strlen(data->name);

	strcpy(key,data->name);

	for (i = 0; i < keySize; i++) key[i] = tolower(key[i]);

	for (i = 0; i < keySize; i++)
	{
		index = chatoi(key[i]);
		if (!finder->children[index])
			finder->children[index] = newNode();

		finder = finder->children[index];
	}
	finder->data = data;
}

// Retorna 0(NULL) se não encontrar ou o endereço dos dados se encontrar
Data *search(TrieNode *root, const char *key)
{
	TrieNode *finder = root;
	int index;

	for (int i = 0; i < strlen(key); i++)
	{
		index = chatoi(key[i]);
		if (!finder->children[index])
			return 0;

		finder = finder->children[index];
	}
	return finder->data;
}
int isTrieLeaf(TrieNode *root)
{
    for (int i = 0; i < ALPHABET_SIZE; i++)
        if (root->children[i])
            return 0;
    return 1;
}
void smartSearchRec(TrieNode *root, char *key)
{
    if (root->data) printData(*(root->data));
 
    if (isTrieLeaf(root))
        return;
 
    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if (root->children[i])
        {	
			char cToStr[2];
			cToStr[1] = '\0';
			cToStr[0] = (char)(i+(int)'a');
			if(i==26) cToStr[0] = ' ';
            strcat(key,cToStr);
 
            smartSearchRec(root->children[i], key);

			key[strlen(key)-1] = '\0';
        }
    }
}
void smartSearch(TrieNode* root, char *key)
{
    TrieNode  *finder = root;
	int index,isLeaf = isTrieLeaf(finder);

    for (int i = 0; i < strlen(key); i++)
    {
        index = chatoi(key[i]);
 
        if (!finder->children[index])
			return;
 
        finder = finder->children[index];
    }
	printf("sofifa_id\tname\t\t\t\t\t\tplayer_positions\trating\t\tcount\n");
    if ((finder->data == NULL) && isLeaf)
		printf("%s\t\n",key);
    else if (!isLeaf)
    	smartSearchRec(finder, key);
}

/*	HASH	*/

void fifaIdinitHT(HT* hashTable, int m) {
	for(int i = 0; i<m; i++)
	{
		hashTable[i].next = NULL;
		hashTable[i].data = NULL;
	}
}
void userIdinitHT(userHT* hashTable, int m) {
	for(int i = 0; i<m; i++)
	{
		hashTable[i].next = NULL;
		hashTable[i].data = NULL;
		hashTable[i].key = 0;
	}
}
//Função geral de hashing
unsigned long long hash(int hash, enum keyType type, int m, int p) {
	switch (type)
	{
		case fifaId:
		case userId:
		break;
		case pos:
		break;
		case rating:
		break;
		case tag:
		break;
		default:
			printf("Erro ao iniciar HashTable\n");
			abort();
	}
	return hash % m;
}
void fifaIdInsertHT(HT *hashTable, Data *data, int m) {
	HT *x = &hashTable[  hash(data->sofifa_id,fifaId,m,0) ];
	while (x->next != NULL && x->data && (x->data->sofifa_id != data->sofifa_id)) x = x->next;
	if (x->next != NULL) return;
	else if (!x->data) x->data = data;
	else {
		x->next = (HT*)malloc(sizeof(HT));
		x->next->data = data;
		x->next->next = NULL;
	}
}
Data *fifaIdsearchHT(HT *hashTable, Data data, int m) {
	HT *x = &hashTable[ hash(data.sofifa_id,fifaId,m,0) ];
	for (int n = 0; x->next != NULL && (x->data->sofifa_id != data.sofifa_id); x = x->next, n++);
	if(x->data->sofifa_id != data.sofifa_id) return NULL;
	return x->data;
}
void userIdInsertHT(userHT *hashTable, Data *data, const unsigned int key, int m) {
	userHT *x = &hashTable[  hash(key,userId,m,0) ];
	while (x->next != NULL && (x->key != key)) x = x->next;
	if (!x->key)
	{
		x->key = key;
		x->data = data;
	}
	else 
	{
		//printf("Searching for %u\n",key);
		while((x->next != NULL) && (x->next->key == key))
		{
			//printf("%u\n",x->key);
			x = x->next;
		}
		userHT *y = x->next;
		x->next = (userHT*)malloc(sizeof(userHT));
		x->next->data = data;
		x->next->key = key;
		x->next->next = y;
	}
}
userHT *userIdsearchHT(userHT *hashTable, Data data, const unsigned int key, int m) {
	userHT *x = &hashTable[ hash(key,userId,m,0) ];
	for (int n = 0; x->next != NULL && (x->key != key); x = x->next, n++);
	if (x->key != key) return NULL;
	return x;
}
void freeIdHTCell(HT *hashCell) {
  if(hashCell->next != NULL) freeIdHTCell(hashCell->next);
  free(hashCell);
}
void freeIdHT(HT *hashTable, int m) {
  for(int j=0; j<m; j++)
    if(hashTable[j].next != NULL) freeIdHTCell(hashTable[j].next);
  free(hashTable);
}

/*	Uso Geral	*/

//Transforma char em inteiro
int chatoi(char c)
{
	if (c >= 'a' && c <= 'z') return ((int)c - (int)'a');
	else return (ALPHABET_SIZE-1);
}
//Printa os dados de forma tabelada
void printData(Data data)
{
	printf("%-12d\t",data.sofifa_id);
	printf("%-40s\t",data.name);
	if((data.player_pos[1][0] != 0) && (data.player_pos[2][0] != 0)) printf("%s, %s, %-10s\t",data.player_pos[0],data.player_pos[1],data.player_pos[2]);
	else if(data.player_pos[1][0] != 0) printf("%s, %-12s\t",data.player_pos[0],data.player_pos[1]);
	else printf("%-18s\t",data.player_pos[0]);
	printf("%1.6f\t",(float)(data.rating/data.count));
	printf("%d\n",data.count);
}
void insertUsers(TrieNode *root, HT *fifaIdHT, userHT *userIdHT, int fifam, int userm, int p, char *fileName)
{
	Data *data;
	FILE *users = fopen(fileName,"r");
	char line[256];
	char *first,*second,*third;

	fgets(line, 256, users);
    while (fgets(line, 256, users))
    {
		Data search;

		first = strtok(line,",");
        second = strtok(NULL,",");
		third = strtok(NULL,",");

		*--second = *--third = '\0';
		second++;
		third++;
		third[4] = '\0';

		search.sofifa_id = atoi(second);

		data = fifaIdsearchHT(fifaIdHT,search,fifam);
		data->rating += (float)atof(third);
		data->count++;

		userIdInsertHT(userIdHT,data,atoi(first),userm);
    }
	fclose(users);
}
void insertPlayers(TrieNode *root, HT *fifaIdHT, int m, int p, char *fileName)
{
	Data *data;
	FILE *players = fopen(fileName,"r");
	char line[256];
	char *first,*second,*third,*fourth,*fifth;

	fgets(line, 256, players);
    while (fgets(line, 256, players))
    {
		data = malloc(sizeof(Data));

		data->player_pos[0][0] = 0;
		data->player_pos[1][0] = 0;
		data->player_pos[2][0] = 0;

		first = strtok(line,",");
        second = strtok(NULL,",");
		third = strtok(NULL,",");

		*--second = *--third = '\0';
		second++;
		third++;

		data->sofifa_id = atoi(first);
		strcpy(data->name,second);

		fourth = strtok(NULL,",");
		if(!fourth) strtok(third,"\n");
		else
		{
			third++;
			fifth = strtok(NULL,",");
			if(!fifth) strtok(fourth,"\"");
			else
			{
				strtok(fifth,"\"");
				strcpy(&(data->player_pos[2][0]),fifth);
			}
			strcpy(&(data->player_pos[1][0]),fourth);
		}
		strcpy(&(data->player_pos[0][0]),third);
		data->count = 0;
		data->rating = 0;

		insertTrie(root,data);
		fifaIdInsertHT(fifaIdHT,data,m);
    }
	fclose(players);
}
