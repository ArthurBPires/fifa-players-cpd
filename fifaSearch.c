#include "fifaSearch.h"

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
//Transforma char em inteiro
int chatoi(char c)
{
	if (c >= 'a' && c <= 'z') return ((int)c - (int)'a');
	else return (ALPHABET_SIZE-1);
}

//Realiza a inserção na Trie
void insert(TrieNode *root, Data *data)
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
    if (root->data)
	{
		printf("%-12d\t",root->data->sofifa_id);
		printf("%-40s\t",root->data->name);
		if((root->data->player_pos[1][0] != 0) && (root->data->player_pos[2][0] != 0)) printf("%s, %s, %-10s\t",root->data->player_pos[0],root->data->player_pos[1],root->data->player_pos[2]);
		else if(root->data->player_pos[1][0] != 0) printf("%s, %-12s\t",root->data->player_pos[0],root->data->player_pos[1]);
		else printf("%-18s\t",root->data->player_pos[0]);
		printf("%1.6f\t",root->data->rating);
		printf("%d\n",root->data->count);
	}
 
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
void insertData(TrieNode *root, char *fileName)
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

		insert(root,data);
    }
	fclose(players);
}
