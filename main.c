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
#include <time.h>
#include "fifaSearch.h"

#define NAME_SIZE 80

int main(void)
{
    TrieNode *root = newNode();
    int fifam = 7507;
    int userm = 100000;
    int p = 1728;
    HT *fifaIdHT = (HT*)malloc(fifam*sizeof(HT));
    userHT *userIdHT = (userHT*)malloc(userm*sizeof(userHT));
    Data data;
    fifaIdinitHT(fifaIdHT,fifam);
    userIdinitHT(userIdHT,userm);
    char *breakPoint,*cmd,*search,input[NAME_SIZE];
    clock_t time;

    printf("Inicializando dados...\n");
    time = clock();
    insertPlayers(root,fifaIdHT,fifam,p,"players_clean2.csv");
    insertUsers(root,fifaIdHT,userIdHT,fifam,userm,p,"rating.csv");
    time = clock() - time;
    printf("Inicialiacao completa em %f segundos.\n", ((double)(time)/CLOCKS_PER_SEC));

    do
    {
      fgets(input, sizeof(input), stdin);
      input[strlen(input)-1] = '\0'; //Retira o '/n' do final

      for(int i=0;i<strlen(input);i++) input[i] = tolower(input[i]);

      breakPoint = strchr(input,' ');
      if(breakPoint)
      {
          *breakPoint = '\0';
          search = ++breakPoint;
      }
      cmd = input;

      if(!strcmp("player",cmd)) smartSearch(root,search);
      else if(!strcmp("fifaid",cmd))
      {
        data.sofifa_id = atoi(search);
        Data *found = fifaIdsearchHT(fifaIdHT,data,fifam);
        printf("sofifa_id\tname\t\t\t\t\t\tplayer_positions\trating\t\tcount\n");
        if(found) printData(*found);
        else printf("fifa_id nao encontrado.\n");
      }
      else if(!strcmp("user",cmd))
      {
        userHT *found = userIdsearchHT(userIdHT,data,atoi(search),userm);
        if(!found) printf("user_id nao encontrado.\n");
        else
        {
          printf("sofifa_id\tname\t\t\t\t\t\tplayer_positions\trating\t\tcount\n");
          while(found && found->key == atoi(search))
          {
            printData(*(found->data));
            found = found->next;
          }
        }
      }
      else if(!strcmp("top10",cmd));
      else if(!strcmp("tags",cmd));
      else if(!strcmp("exit",cmd)) break;
      else printf("Comando Invalido.\n");

    }while(1);

    return 0;
}
