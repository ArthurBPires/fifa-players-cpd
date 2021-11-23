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
    unsigned long m[] = {20000,USERS,20000};
    char fileNames[][100] = {"players.csv","rating.csv","tags.csv"};
    
    if(argc>1) argOpt(argc,argv,m,fileNames);

    HT *fifaIdHT = (HT*)malloc(m[fifaId]*sizeof(HT));
    floatHT *userIdHT = (floatHT*)malloc(m[userId]*sizeof(floatHT));
    textHT *tagHT = (textHT*)malloc(m[tag]*sizeof(textHT));
    Data data;
    initHT(fifaIdHT,m[fifaId]);
    floatInitHT(userIdHT,m[userId]);
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
          printTableTop(1);
          printData(*found,1);
        }
        else printf("fifa_id nao encontrado.\n");
      }
      else if (!strcmp("user", cmd))
      {
        const unsigned long user = atol(search);
        if (user < 1 || user > USERS)
          printf("user_id nao encontrado.\n");
        else
        {
          int i;
          floatHT *x = userIdsearchHT(userIdHT,user);
          floatHT list[20];
          for(i=0;i<20;i++)
            list[i].userRating = -1;
          
          while(x)
          {
            for(i=0;i<20;i++)
            {
              if(x->userRating > list[i].userRating)
              {
                for(int j=18;j>=i;j--)
                {
                  list[j+1].data = list[j].data;
                  list[j+1].userRating = list[j].userRating;
                }
                list[i].data = x->data;
                list[i].userRating = x->userRating;
                break;
              }
            }
            x = x->next;
          }
          printTableTop(0);
				  printf("\tuser_rating\n");
          for(i=0;i<20;i++)
          {
            if(list[i].userRating >= 0)
            {
              printData(*(list[i].data), 0);
							printf("\t%.1f\n", list[i].userRating);
            }
          }
        }
      }
      else if(cmd[0]=='t' && cmd[1]=='o' && cmd[2]=='p' && (strlen(cmd) > 3)){
        //Verifica se a entrada é válida
        search++;
        if((strtok(search,"\'") == NULL) || (strlen(search) < 2))
        {
          printf("Informe uma posicao valida.\n");
        }
        else
        {
          // coloca em many a quantidade de jogadores
          char playersAmount[10];
          int i;
          for ( i=3 ; i<strlen(cmd); i++){
            playersAmount[i-3] = cmd[i];
          }
          playersAmount[i-1] = '\0';
          int many = atoi(playersAmount);

          //aloca um vetor de ponteiros para Data inicializados em NULL
          Data** bestNPlayers = (Data**)malloc(sizeof(Data*) * many);
          for (int i=0 ; i<many ; i++){
            bestNPlayers[i] = NULL;
          }

          //Percorre a HT com as informações realizando filtragens
          char pos[4];
          strcpy(pos, search);

          stringUpperCase(pos);
          // percorre o vetor HT
          for(int i=0 ; i<m[fifaId] ; i++){
            if(fifaIdHT[i].data){
              HT *player = &fifaIdHT[i];
              // Percorre a LSE que eh cada elemento da HT
              while(player){
                // player eh o jogador atual, se for da posicao certa e tiver mais de 1000 avaliacoes continua
                if( hasTag(player->data, pos) && player->data->count >= 1000){
                  // Agora o player tentara ser inserido no vetor com os 10 maiores ratings
                  int insertPos = -1;// esse int guarda a pos de insercao, se manter-se -1 nao insere
                  int l;
                  // se algum elemento do vetor dos 10 maiores raitings for NULL, ele eh inserido
                  for ( l=0 ; l<many ; l++){
                    if(bestNPlayers[l]==NULL){
                      insertPos= l;
                      l=many+1;
                    }
                  }
                  // se nenhum for null, eh consultado se algum elemento do vetor dos 10 maiores eh menor que ele
                  if(insertPos == -1){
                    int menor = 0;
                    // busca o menor
                    for(int k=0 ;k<many ;k++){
                      if(bestNPlayers[menor]->rating/bestNPlayers[menor]->count > bestNPlayers[k]->rating/bestNPlayers[k]->count)
                        menor = k; 
                    }
                    // se o player tiver o rating maior que o menor eh inserido na posicao
                    if(player->data->rating/player->data->count > bestNPlayers[menor]->rating/bestNPlayers[menor]->count){
                      insertPos = menor;
                    }
                  }
                  // realiza a isercao se possivel
                  if(insertPos != -1){
                    bestNPlayers[insertPos] = player->data;
                  }
                }
                player = player->next;
              }
            }
          }
          if(bestNPlayers[0] == NULL)
          {
            printf("Nenhum jogador encontrado com essa posicao.\n");
          }
          else
          {
            // imprime os dados na tela de forma ordenada
            printTableTop(1);
            quicksort(bestNPlayers, 0, many-1);
            for(int i=0 ; i<many ; i++){
              if(bestNPlayers[i])
                printData(*bestNPlayers[i],1);
            }
          }
          free(bestNPlayers);
        }
      }
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

    thanoSnap(root,fifaIdHT,userIdHT,tagHT,m);

    return 0;
}
