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

#ifdef _WIN32
    double timeCPU(){
      FILETIME a,b,c,d;
      if (GetProcessTimes(GetCurrentProcess(),&a,&b,&c,&d) != 0){
          //  Retorna o tempo total passado
          return
              (double)(d.dwLowDateTime |
              ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
      }else
          return 0;
    }
#endif


int main(int argc, char **argv)
{
    TrieNode *root = newNode();
    unsigned long m[] = {20000,USERS,20000}; //fifaid min = 41, fifaid max = 199987. Total = 3014 fifaids.
    char fileNames[][100] = {"players_clean2.csv","rating.csv","tags.csv"};
    
    if(argc>1) argOpt(argc,argv,m,fileNames);

    HT *fifaIdHT = (HT*)malloc(m[fifaId]*sizeof(HT));
    HT *userIdHT = (HT*)malloc(m[userId]*sizeof(HT));
    textHT *tagHT = (textHT*)malloc(m[tag]*sizeof(textHT));
    Data data;
    initHT(fifaIdHT,m[fifaId]);
    initHT(userIdHT,m[userId]);
    textInitHT(tagHT,m[tag]);
    char *breakPoint,*cmd,*search,input[NAME_SIZE];

    printf("Inicializando dados...\n"); //Recorde: 7,28125 em Clang -Ofast
    double timeElapsed;
    #ifdef _WIN32
    double begin,end;
    begin = timeCPU();
    #else
    clock_t begin = clock();
    #endif

    insertPlayers(root,fifaIdHT,m,fileNames[fifaId]);
    insertUsers(root,fifaIdHT,userIdHT,m,fileNames[userId]);
    insertTags(root,fifaIdHT,tagHT,m,fileNames[tag]);

    #ifdef _WIN32
    end = timeCPU();
    timeElapsed = end - begin;
    #else
    clock_t end = clock();
    timeElapsed = (double)(end - begin)/CLOCKS_PER_SEC;
    #endif


    printf("Inicialiacao completa em %f segundos.\n", timeElapsed);

    do
    {
      fgets(input, sizeof(input), stdin);
      input[strlen(input)-1] = '\0'; //Retira o '/n' do final

      for(int i=0;i<strlen(input);i++) 
        input[i] = tolower(input[i]);

      cmd = input;

      breakPoint = strchr(input,' ');
      if(breakPoint)
      {
          *breakPoint = '\0';
          search = ++breakPoint;
      }
      if(!strcmp("exit",cmd)) break;
      else if(!strcmp("clear",cmd) || !strcmp("cls",cmd) || !strcmp("reset",cmd))
      {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
      }
      //Apartir daqui não se aceita input sem espaço
      else if(!breakPoint) printf("Comando Invalido\n");
      else if(!strcmp("player",cmd)) smartSearch(root,search);
      //Apartir daqui não se aceita input sem argumento depois do espaço
      else if(!strlen(search)) printf("Comando Invalido\n");
      else if(!strcmp("fifaid",cmd))
      {
        data.sofifa_id = atol(search);
        Data *found = fifaIdsearchHT(fifaIdHT,data,m[fifaId]);
        if(found)
        {
          printTableTop();
          printData(*found);
        }
        else printf("fifa_id nao encontrado.\n");
      }
      else if(!strcmp("user",cmd))
      {
        HT *list = NULL;
        const unsigned long user = atol(search);
        if(user<1 || user>USERS) printf("user_id nao encontrado.\n");
        else
        {
          list = userIdsearchHT(userIdHT,user);
          if(!list) printf("user_id nao encontrado.\n");
          else
          {
            printTableTop();
            while(list)
            {
              printData(*(list->data));
              list = list->next;
            }
          }
        }
      }
      else if(!strcmp("top10",cmd));
      else if(!strcmp("tags",cmd))
      {
        char *tagSearch;
        char **tagList = NULL;
        int i=0;
        search++;
        tagSearch = search;
        if(strtok(search,"\'"))
        {
          while (tagSearch)
          {
            i++;
            tagList = realloc(tagList,i*sizeof(char*));
            char *newString = malloc(TAG_SIZE*sizeof(char));
            strcpy(newString,tagSearch);
            tagList[i-1] = newString;
            strtok(NULL,"\'");
            tagSearch = strtok(NULL,"\'");
          }
          multTagsearchHT(tagHT,tagList,m[tag],i);
          while(i)
          {
            i--;
            free(tagList[i]);
            if(!i) free(tagList);
          }
        }
      }
      else printf("Comando Invalido.\n");

    }while(1);

    thanoSnap(root,fifaIdHT,userIdHT,m);

    return 0;
}
