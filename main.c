/**
 * Aplicação dividida em 2 fases:
 * - Fase 1: Construção e inicialização das estruturas necessárias para suportar as consultas. Max 3 min de duração, tentar < 1min.
 * - Fase 2: Permite a busca e exibição de informações. Nesta fase os arquivos não devem ser abertos.
 * - Pesquisa por Nome (player <name or prefix>) : $ player Fer
 * - Pesquisa por avaliações de usuário (user <userID>) : $ user 4
 * - Pesquisa por melhores jogadores um posição x (top<N> ‘<position>’): $ top10 ‘ST’
 * - Pesquisa por tags (tags <list of tags>): $ tags ‘Brazil’ ‘Dribbler’
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "fifaSearch.h"

#define NAME_SIZE 80

int main(void)
{
	TrieNode *root = newNode();
	Data *data;
	char input[NAME_SIZE];
	int i;

	insertData(root,"players_clean2.csv");

	do
	{
		fgets(input, sizeof(input), stdin);
		input[strlen(input)-1] = '\0'; //Retira o '/n' do final

		for(i=0;i<strlen(input);i++) input[i] = tolower(input[i]);

		smartSearch(root,input);

	}while(strcmp("exit",input));

	return 0;
}
