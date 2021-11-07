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
//Função que completa o nome key com os possíveis nomes de jogadores.
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
//Libera a árvore Trie
void freeTrie(TrieNode *node)
{
	if(!isTrieLeaf(node))
		for(int i=0; i<ALPHABET_SIZE;i++)
			if (node->children[i])
				freeTrie(node->children[i]);
	free(node);
}

/*	HASH	*/

//Função geral de iniciação de HashTable
void initHT(HT* hashTable, const unsigned long m) {
	for(int i = 0; i<m; i++)
	{
		hashTable[i].next = NULL;
		hashTable[i].data = NULL;
	}
}

//Função geral de hashing
unsigned long hash(unsigned long hash, const enum keyType type, const unsigned long m, const int p) {
	switch (type)
	{
		case fifaId:
		break;
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
//Insere os dados no final da corrent correspondente ao hash, caso eles ainda não tenham sido inseridos.
void fifaIdInsertHT(HT *hashTable, Data *data, const unsigned long m) {
	HT *x = &hashTable[  hash(data->sofifa_id,fifaId,m,0) ];
	while (x->next && (x->data->sofifa_id != data->sofifa_id)) x = x->next;
	if (!x->data) x->data = data;
	else if (x->next != NULL) return;
	else {
		x->next = (HT*)malloc(sizeof(HT));
		x->next->data = data;
		x->next->next = NULL;
	}
}
//Retorna NULL caso não encontre, retorna o ponteiro para os dados caso contrário.
Data *fifaIdsearchHT(HT *hashTable, Data data, unsigned long m) {
	HT *x = &hashTable[ hash(data.sofifa_id,fifaId,m,0) ];
	for (unsigned long n = 0; x->next && (x->data->sofifa_id != data.sofifa_id); x = x->next, n++);
	if(!x->data || (x->data->sofifa_id != data.sofifa_id)) return NULL;
	return x->data;
}
/*insere os dados do usuário no começo da corrente referente a chave.	*/
void userIdInsertHT(HT *hashTable, Data *data, const unsigned long key) {
	HT *x = hashTable[key].next;
	hashTable[key].next = (HT*)malloc(sizeof(HT));
	hashTable[key].next->data = data;
	hashTable[key].next->next = x;
}
/*Retorna o primeiro nodo de dados do usuário. Para ler todos os nodos do usuário, basta usar 
o nodo retornado e seguir a corrente de ponteiros até o fim	*/
HT *userIdsearchHT(HT *hashTable, const unsigned long key) {
	return hashTable[key-1].next;
}
//Libera o espaço de uma corrente de celulas
void freeHTCell(HT *hashCell) {
  if(hashCell->next != NULL) freeHTCell(hashCell->next);
  free(hashCell);
}
//Chama freeHTCell para cada espaço da HT
void freeHT(HT *hashTable, const unsigned long m) {
  for(int j=0; j<m; j++)
    if(hashTable[j].next != NULL) freeHTCell(hashTable[j].next);
  free(hashTable);
}

/*	Uso Geral	*/

//Transforma char em inteiro
int chatoi(const char c)
{
	if (c >= 'a' && c <= 'z') return ((int)c - (int)'a');
	else return (ALPHABET_SIZE-1);
}
//Printa os dados de forma tabelada
void printData(const Data data)
{
	printf("%-12lu\t",data.sofifa_id);
	printf("%-40s\t",data.name);
	if((data.player_pos[1][0] != 0) && (data.player_pos[2][0] != 0)) printf("%s, %s, %-10s\t",data.player_pos[0],data.player_pos[1],data.player_pos[2]);
	else if(data.player_pos[1][0] != 0) printf("%s, %-12s\t",data.player_pos[0],data.player_pos[1]);
	else printf("%-18s\t",data.player_pos[0]);
	printf("%1.6f\t",(float)(data.rating/data.count));
	printf("%d\n",data.count);
}
//insere os dados sobre os usuários
void insertUsers(TrieNode *root, HT *fifaIdHT, HT *userIdHT, const unsigned long *m, const char *fileName)
{
	Data *data;
	Data search;
	FILE *users = fopen(fileName,"r");
	char line[256];
	char *first,*second,*third;

	fgets(line, 256, users);
    while (fgets(line, 256, users))
    {
		first = strtok(line,",");
        second = strtok(NULL,",");
		third = strtok(NULL,",");

		*--second = *--third = '\0';
		second++;
		third++;
		third[4] = '\0';

		search.sofifa_id = (unsigned long)atol(second);

		data = fifaIdsearchHT(fifaIdHT,search,m[fifaId]);
		data->rating += (float)atof(third);
		data->count++;

		//printf("data %p\n", data);

		userIdInsertHT(userIdHT,data,(const unsigned long)(atol(first)-1));
    }
	fclose(users);
}
//insere os dados sobre os jogadores
void insertPlayers(TrieNode *root, HT *fifaIdHT, const unsigned long *m, const char *fileName)
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

		data->sofifa_id = (unsigned long)atol(first);
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
		fifaIdInsertHT(fifaIdHT,data,m[fifaId]);
    }
	fclose(players);
}
//Acha os melhores valores para m
unsigned long *fineTune(char fileNames[][100])
{
	int i,j;
	const int n = 1;
	unsigned long recordM[2],*m = (unsigned long*)malloc(2*sizeof(unsigned long));
	const int tries=3;
	double timeTaken, recordTime;
	clock_t time;

	m[0] = 20000;
	m[1] = 138493;

	for(i=0;i<n;i++)
	{
		recordTime = INT_MAX;
		for(m[i]=1000000;m[i]<=500000; m[i]*=2)
		{
			printf("Testes para m[%d] = %lu...", i,m[i]);
			timeTaken = 0;
			for(j=0;j<tries;j++)
			{
				TrieNode *root = newNode();
				HT *fifaIdHT = (HT*)malloc(m[fifaId]*sizeof(HT));
				HT *userIdHT = (HT*)malloc(m[userId]*sizeof(HT));
				initHT(fifaIdHT,m[fifaId]);
				initHT(userIdHT,m[userId]);

				time = clock();
				insertPlayers(root,fifaIdHT,m,fileNames[fifaId]);
				insertUsers(root,fifaIdHT,userIdHT,m,fileNames[userId]);
				time = clock() - time;
				timeTaken += ((double)(time)/CLOCKS_PER_SEC);

				thanoSnap(root,fifaIdHT,userIdHT,m);
			}
			timeTaken /= tries;
			if(timeTaken<recordTime)
			{
				recordTime = timeTaken;
				recordM[i] = m[i];
			}
			printf(" OK\n");
		}
		m[i] = recordM[i];
	}
	printf("fineTune() achou os seguintes valores de m: %lu, %lu.\n",m[fifaId],m[userId]);

	return m;
}
//Delata todas as estruturas, liberando a memória ocupada por elas
void thanoSnap(TrieNode *root, HT *fifaIdHT, HT *userIdHT, const unsigned long *m)
{
	freeTrie(root);
	freeHT(fifaIdHT,m[fifaId]);
	freeHT(userIdHT,m[userId]);
}
//Lida com os argumentos passados
void argOpt(const int argc, char **argv, unsigned long *m, char fileNames[][100])
{
	int flagf,flagp[2];
	flagf = flagp[0] = 0;
	for (int i=1; i<argc; i++)
	{
		if(!strcmp("-f",argv[i]) || !strcmp("-fine-tune",argv[i]))
			flagf++;
		else if((!strcmp("-p",argv[i]) || !strcmp("-path",argv[i])) && (argc>(i+2)))
		{
			flagp[0]++;
			flagp[1]=i;
		}
	}
	if(flagp[0])
	{
	strcpy(fileNames[fifaId],argv[flagp[1]+1]);
	strcpy(fileNames[userId],argv[flagp[1]+2]);
	}
	if(flagf)
	{
	unsigned long *mFound = fineTune(fileNames);
	m[fifaId] = mFound[fifaId];
	m[userId] = mFound[userId];
	free(mFound);
	}
	if(!flagf && !flagp[0])
	{
	printf("Comando desconhecido.\n");
	printf("Comandos disponiveis:\n-f\n-fine-tune\n");
	abort();
	}
}