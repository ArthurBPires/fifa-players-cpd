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
	printTableTop();
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
void textInitHT(textHT* hashTable, const unsigned long m) {
	for(int i = 0; i<m; i++)
	{
		hashTable[i].next = NULL;
		hashTable[i].data = NULL;
		strcpy(hashTable[i].text,"");
	}
}

//Função geral de hashing
unsigned long hash(char *text, const enum keyType type, const unsigned long m, const int p) {
	unsigned long long hash=0;	//Atualmente não está sendo usada por fifaId ou userId por buscarmos máxima velocidade de processamento.
	switch (type)
	{
		case fifaId:
		break;
		case userId:
		break;
		case tag:
		case pos:
			for (int i = 0; i < strlen(text); i++)
      			hash = (hash * p + (text[i] - 'a' + 1));
		break;
		default:
			printf("Erro ao iniciar HashTable\n");
			abort();
	}
	return hash % m;
}
//Insere os dados no final da corrente correspondente ao hash, caso eles ainda não tenham sido inseridos.
void fifaIdInsertHT(HT *hashTable, Data *data, const unsigned long m) {
	HT *x = &hashTable[data->sofifa_id % m];
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
	HT *x = &hashTable[data.sofifa_id % m];
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
o nodo retornado e seguir a corrente de ponteiros até o fim. Retorna NULL caso não encontre	*/
HT *userIdsearchHT(HT *hashTable, const unsigned long key) {
	return hashTable[key-1].next;
}
//Insere os dados no final da corrente correspondente a key, caso eles ainda não tenham sido inseridos.
void tagInsertHT(textHT *hashTable, char *key, Data *data, const unsigned long m) {
	textHT *x = &hashTable[ hash(key,tag,m,31) ];
	while (x->next && ((x->data->sofifa_id != data->sofifa_id) || strcmp(x->text,key))) x = x->next;
	if (!x->data)
	{
		x->data = data;
		strcpy(x->text,key);
	}
	else if (x->next || (x->data->sofifa_id == data->sofifa_id)) return;
	else {
		x->next = (textHT*)malloc(sizeof(textHT));
		x->next->data = data;
		strcpy(x->next->text,key);
		x->next->next = NULL;
	}
}
/*Retorna o primeiro nodo de dados correspondente a tag. Para ler todos os nodos, basta usar 
o nodo retornado e seguir a corrente de ponteiros até o fim ou até chegar em outra tag*/
textHT *tagsearchHT(textHT *hashTable, char *key, const unsigned long m) {
	textHT *x = &hashTable[ hash(key,tag,m,31) ];
	while(x && strcmp(x->text,key)) x = x->next;
	return x;
}
void multTagsearchHT(textHT *hashTable, char **tagList, const unsigned long m, const int n) {
	textHT *found,*alsoFound;
	int flagFound;

	if(!n) return;

	found = tagsearchHT(hashTable,tagList[0],m);
	if(found) printTableTop();
	while(found && !strcmp(found->text,tagList[0]))
	{
		flagFound = 1;
		for(int i=1;i<n;i++)
		{
			alsoFound = tagsearchHT(hashTable,tagList[i],m);
			
			while(alsoFound && !strcmp(alsoFound->text,tagList[i]) && (alsoFound->data->sofifa_id != found->data->sofifa_id))
				alsoFound = alsoFound->next;
			if(!alsoFound || (alsoFound->data->sofifa_id != found->data->sofifa_id))
			{
				flagFound = 0;
				break;
			}
		}
		if(flagFound || n==1) printData(*(found->data));
		found = found->next;
	}
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
//Printa o cabeçalho da tabela de dados
void printTableTop()
{
	printf("sofifa_id\tname\t\t\t\t\t\t\tplayer_positions\trating\t\tcount\n");
}
//Printa os dados de forma tabelada
void printData(const Data data)
{
	printf("%-12lu\t",data.sofifa_id);
	printf("%-50s\t",data.name);
	if((data.player_pos[1][0] != 0) && (data.player_pos[2][0] != 0)) printf("%s, %s, %-10s\t",data.player_pos[0],data.player_pos[1],data.player_pos[2]);
	else if(data.player_pos[1][0] != 0) printf("%s, %-12s\t",data.player_pos[0],data.player_pos[1]);
	else printf("%-18s\t",data.player_pos[0]);
	if(data.count)
		printf("%1.6f\t",(float)(data.rating/data.count));
	else printf("%1.6f\t",(float)0);
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


		userIdInsertHT(userIdHT,data,(const unsigned long)(atol(first)-1));
    }
	fclose(users);
}

void clearStringPos( char str[] ){
    int i,j;
    i = 0;
    while(i<strlen(str))
    {
        if (str[i]==' ' || str[i]==',') 
        { 
            for (j=i; j<strlen(str); j++)
                str[j]=str[j+1];   
        } else i++;
    }
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
				// if(fifth)printf("before: )%s(",fifth);
				clearStringPos(fifth);
				// if(fifth)printf("after: )%s(\n",fifth);
				strcpy(&(data->player_pos[2][0]),fifth);
			}
			// if(fourth)printf("before: )%s(",fourth);
			clearStringPos(fourth);
			// if(fourth)printf("after: )%s(\n",fourth);
			strcpy(&(data->player_pos[1][0]),fourth);
		}
		// if(third)printf("before: )%s(",third);
		clearStringPos(third);
		// if(third)printf("after: )%s(\n",third);
		strcpy(&(data->player_pos[0][0]),third);
		data->count = 0;
		data->rating = 0;

		// INSERT IN NEW LSE

		insertTrie(root,data);
		fifaIdInsertHT(fifaIdHT,data,m[fifaId]);
    }
	fclose(players);
}
//insere os dados sobre as tags
void insertTags(TrieNode *root, HT *fifaIdHT, textHT *tagHT, const unsigned long *m, const char *fileName)
{
	Data *data;
	Data search;
	FILE *tags = fopen(fileName,"r");
	char line[256];
	char *second,*third;
	int i;

	fgets(line, 256, tags);
    while (fgets(line, 256, tags))
    {
		strtok(line,",");
        second = strtok(NULL,",");
		third = strtok(NULL,",");

		*--third = '\0';
		third++;
		strtok(third,"\n");
		for (i = 0; i < strlen(third); i++) third[i] = tolower(third[i]);

		search.sofifa_id = (unsigned long)atol(second);

		data = fifaIdsearchHT(fifaIdHT,search,m[fifaId]);

		tagInsertHT(tagHT,third,data,m[tag]);
    }
	fclose(tags);
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
		else if((!strcmp("-p",argv[i]) || !strcmp("-path",argv[i])) && (argc>(i+3)))
		{
			flagp[0]++;
			flagp[1]=i;
		}
	}
	if(flagp[0])
	{
	strcpy(fileNames[fifaId],argv[flagp[1]+1]);
	strcpy(fileNames[userId],argv[flagp[1]+2]);
	strcpy(fileNames[tag],argv[flagp[1]+3]);
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

// void swapData(Data *d1, Data *d2) {
// 	Data temp;
// 	temp.count = d1->count;
// 	temp.sofifa_id = d1->sofifa_id;
// 	strcpy(temp.name, d1->name);
// 	temp.rating = d1->rating;
// 	strcpy(temp.player_pos[0], d1->player_pos[0]);
// 	strcpy(temp.player_pos[1], d1->player_pos[1]);
// 	strcpy(temp.player_pos[2], d1->player_pos[2]);

// 	d1->count = d2->count;
// 	d1->sofifa_id = d2->sofifa_id;
// 	strcpy(d1->name, d2->name);
// 	d1->rating = d2->rating;
// 	strcpy(d1->player_pos[0], d2->player_pos[0]);
// 	strcpy(d1->player_pos[1], d2->player_pos[1]);
// 	strcpy(d1->player_pos[2], d2->player_pos[2]);

// 	d2->count = temp.count;
// 	d2->sofifa_id = temp.sofifa_id;
// 	strcpy(d2->name, temp.name);
// 	d2->rating = temp.rating;
// 	strcpy(d2->player_pos[0], temp.player_pos[0]);
// 	strcpy(d2->player_pos[1], temp.player_pos[1]);
// 	strcpy(d2->player_pos[2], temp.player_pos[2]);

// }

// Para a funcao quicksort
void swapData(Data** vet, int p1, int p2){
	Data* aux = vet[p1];
	vet[p1] = vet[p2];
	vet[p2] = aux;
}
// Para a funcao quicksort
int partition(Data **data, int low, int high){
    Data *pivot = data[high];
    int i = low - 1;
	 
    for (int j = low; j <= high-1; j++)
    {
        if (data[j]->rating/data[j]->count >= pivot->rating/pivot->count)
        {
            i++;
            swapData(data, i, j);
        }
    }
    swapData(data, i + 1,high);
    return (i + 1);
}

// quicksort que ordena um vetor de ponteiros para Data
void quicksort(Data **data, int low, int high){
    if (low < high)
    {
        int pi = partition(data, low, high);
        quicksort(data, low, pi - 1);
        quicksort(data, pi + 1, high);
    }
}

// Olha para um Data e verifica a presenca de um TAG
int hasTag(Data* data, char* tag){
	if( !strcmp( data->player_pos[0], tag) || !strcmp( data->player_pos[1], tag) || !strcmp( data->player_pos[2], tag)) return 1;
	return 0;
}

void stringUpperCase(char* str){
	for (int i = 0; str[i]!='\0'; i++) {
   	if(str[i] >= 'a' && str[i] <= 'z') {
      	str[i] = str[i] -32;
   	}
	}
}