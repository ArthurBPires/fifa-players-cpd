/**
 * Aplicação dividida em 2 fases:
 * - Fase 1: Construção e inicialização das estruturas necessárias para suportar as consultas. Max 3 min de duração, tentar < 1min.
 * - Fase 2: Permite a busca e exibição de informações. Nesta fase os arquivos não devem ser abertos.
 * - Pesquisa por Nome (player <name or prefix>) : $ player Fer
 * - Pesquisa por avaliações de usuário (user <userID>) : $ user 4
 * - Pesquisa por melhores jogadores um posição x (top<N> ‘<position>’): $ top10 ‘ST’
 * - Pesquisa por tags (tags <list of tags>): $ tags ‘Brazil’ ‘Dribbler’
 */
#include "fifaSearch.h"
#define USERS 138493  //138493 users no total

int main(int argc, char **argv)
{
    TrieNode *root = newNode();
    unsigned long m[] = {20000,USERS}; //fifaid min = 41, fifaid max = 199987. Total = 3014 fifaids.
    char fileNames[][100] = {"players_clean2.csv","rating.csv"};
    int p = 1728;
    
    if(argc>1) argOpt(argc,argv,m,fileNames);

    HT *fifaIdHT = (HT*)malloc(m[fifaId]*sizeof(HT));
    HT *userIdHT = (HT*)malloc(m[userId]*sizeof(HT));
    Data data;
    initHT(fifaIdHT,m[fifaId]);
    initHT(userIdHT,m[userId]);
    char *breakPoint,*cmd,*search,input[NAME_SIZE];
    clock_t time;

    printf("Inicializando dados...\n");
    time = clock();
    insertPlayers(root,fifaIdHT,m,fileNames[fifaId]);
    insertUsers(root,fifaIdHT,userIdHT,m,fileNames[userId]);
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
        Data *found = fifaIdsearchHT(fifaIdHT,data,m[fifaId]);
        printf("sofifa_id\tname\t\t\t\t\t\tplayer_positions\trating\t\tcount\n");
        if(found) printData(*found);
        else printf("fifa_id nao encontrado.\n");
      }
      else if(!strcmp("user",cmd))
      {
        const unsigned long user = atol(search);
        if(user<1 || user>USERS) printf("user_id nao encontrado.\n");
        else
        {
          HT *list = userIdsearchHT(userIdHT,user);
          printf("sofifa_id\tname\t\t\t\t\t\tplayer_positions\trating\t\tcount\n");
          while(list)
          {
            printData(*(list->data));
            list = list->next;
          }
        }
      }
      else if(!strcmp("top10",cmd));
      else if(!strcmp("tags",cmd));
      else if(!strcmp("exit",cmd)) break;
      else printf("Comando Invalido.\n");

    }while(1);

    thanoSnap(root,fifaIdHT,userIdHT,m);

    return 0;
}
