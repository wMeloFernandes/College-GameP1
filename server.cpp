#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "serializable.hpp"
#include "oo_model.hpp"
#define MAX_CONEXOES 4

struct sockaddr_in myself, client;
socklen_t client_size;
int socket_fd;
int connection_fd[MAX_CONEXOES];
int conexao_usada[MAX_CONEXOES];
int running;
ListaDeCorpos *lc;
ListaDeTiros *lt;
int oldUserNumberUsersOnline=0;
int numberUsersOnline=0;
int positionDefaultTeam1 = 10;
int positionDefaultTeam2 = 45;

using namespace std::chrono;
uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int adicionar_conexao(int new_connection_fd) {
  int i;
  for (i=0; i<MAX_CONEXOES; i++) {
    if (conexao_usada[i] == 0) {
      conexao_usada[i] = 1;
      connection_fd[i] = new_connection_fd;
      numberUsersOnline++;
      return i;
    }
  }
  return -1;
}

int remover_conexao(int user) {
  if (conexao_usada[user]==1) {
  conexao_usada[user] = 0;
  numberUsersOnline--;
  close(connection_fd[user]);
  }
  return 1;
}

void *wait_connections(void *parameters) {
  int conn_fd;
  int user_id;
  while(running) {
    conn_fd = accept(socket_fd, (struct sockaddr*)&client, &client_size);
    user_id = adicionar_conexao(conn_fd);
  }
  return NULL;
}

int main () {
  std::string send_buffer;
  std::string send_buffer2;
  pthread_t esperar_conexoes;
  int msglen;
  int user_iterator;
  char output_buffer[60];
  char input_buffer[50];

  lc = new ListaDeCorpos();
  lt = new ListaDeTiros();

  /* Inicializando variaveis */
  client_size = (socklen_t)sizeof(client);
  for (int i=0; i<MAX_CONEXOES; i++) {
    conexao_usada[i] = 0;
  }
  running = 1;

  /* socket, bind, listen */
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  //printf("Socket criado\n");
  myself.sin_family = AF_INET;
  myself.sin_port = htons(3001);
  inet_aton("127.0.0.1", &(myself.sin_addr));
  //printf("Tentando abrir porta 3001\n");
  if (bind(socket_fd, (struct sockaddr*)&myself, sizeof(myself)) != 0) {
    printf("Problemas ao abrir porta\n");
    return 0;
  }
  listen(socket_fd, 2);

  /* Dispara thread para ouvir conexoes */
  pthread_create(&esperar_conexoes, NULL, wait_connections, NULL);

  unsigned int turn = 0;
  int mFloorHited = 0;

  Fisica *f = new Fisica(lc, lt);

  Tela *tela = new Tela(lc, lt, 20, 20, 20, 20);
  tela->init();

  Teclado *teclado = new Teclado();
  teclado->init();

  uint64_t t0;
  uint64_t t1;
  uint64_t deltaT;
  uint64_t T;
  uint64_t tiro = 0;
  int v1=0;
  int v2=0;
  
  int i = 0;
  int vetId[MAX_CONEXOES];

  for (i = 0; i < MAX_CONEXOES; i++) {
    vetId[i] = 0;
  }


  // Espera
  while (1) {
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    t1 = get_now_ms();
    if (t1-t0 > 500) break;
  }

  
  T = get_now_ms();
  t1 = T;

  while (running) {

    // Atualiza timers
    t0 = t1;
    t1 = get_now_ms();
    deltaT = t1-t0;

    if(numberUsersOnline>0){
    	v1=oldUserNumberUsersOnline;
    	v2=numberUsersOnline;
      if(oldUserNumberUsersOnline<numberUsersOnline){
        if(numberUsersOnline%2==0){
          positionDefaultTeam2=positionDefaultTeam2+5;
          lc->add_corpo(new Corpo(positionDefaultTeam2));
          lt->add_tiro(new Tiro(15,positionDefaultTeam2,15,1));
        }
        else{
          positionDefaultTeam1=positionDefaultTeam1+5;
          lc->add_corpo(new Corpo(positionDefaultTeam1));
          lt->add_tiro(new Tiro(15,positionDefaultTeam1,15,1));  
        }

        oldUserNumberUsersOnline++;
        
        tela = new Tela(lc, lt, 20, 20, 20, 20);
        tela->init();
      }
      else {
        // Atualiza tela
        tela->update(t1-T, tiro);
      }

      if (tiro) {
       for (i = 0; i < numberUsersOnline; i++) {
         if (vetId[i] == 1) {
           f->tiro(deltaT, &mFloorHited, i, numberUsersOnline);

           if (mFloorHited) {
              vetId[i] = 0;
              mFloorHited = 0;
           }
          }
          if (vetId[0] == 0 and vetId[1] == 0 and vetId[2] == 0 and vetId[3] == 0) {
            tiro = 0;
          }
          T = get_now_ms();
          t1 = T;
        }
      }

      for (user_iterator=0; user_iterator<MAX_CONEXOES; user_iterator++) {
        if (conexao_usada[user_iterator] == 1) {
          msglen = recv(connection_fd[user_iterator], input_buffer, 50, MSG_DONTWAIT);
          if (msglen > 0) {
            //printf("Recebi mensagem de %d\n", user_iterator);
            if (strcmp(input_buffer, "END") == 0) running=0;

            if (input_buffer[0] == 'w') {
              f->movimento('w', user_iterator);
            }
            if (input_buffer[0] == 's' ) {
              f->movimento('s', user_iterator);
             }

            if (input_buffer[0] == '+') {
             f->alteraForca('+', user_iterator);
            }

            if (input_buffer[0] == '-') {
             f->alteraForca('-', user_iterator);
            }

            if (input_buffer[0] =='q') {
              for (user_iterator=0; user_iterator<MAX_CONEXOES; user_iterator++)
                remover_conexao(user_iterator);
              pthread_join(esperar_conexoes, NULL);
              break;
            }

            if (input_buffer[0] == 'm') {
               tiro = 1;
               vetId[user_iterator] = 1;
            }
          }
        }
      }


    }
    send_buffer = '#';
    std::vector<Corpo *> *c = lc->get_corpos();
    std::vector<Tiro *> *t = lt->get_tiros();
    for(int i=0; i < c->size(); i++){
      RelevantData	DadosCorpo( v1 ,\
                                  v2,\
                                (*c)[i]->get_posicao(),\
                                (*c)[i]->getLife(),\
                                (*t)[i]->get_velocidade(),\
                                (*t)[i]->get_posicaoHorizontal(),\
                                (*t)[i]->get_posicaoVertical(),\
                                (*t)[i]->get_forca(),\
                                1
                                          );
      std::string buffer(100, '#');
      DadosCorpo.serialize(buffer);
      send_buffer = send_buffer + buffer;
      std::replace( send_buffer.begin(), send_buffer.end(), '\0', '#'); // replace all '\0' to '#'
      send_buffer+='\0';
    }

      for (int ret=0; ret<MAX_CONEXOES; ret++) {
        if (conexao_usada[ret] == 1) {
          if (send(connection_fd[ret], send_buffer.c_str() , MAX_MSG_STRING, MSG_NOSIGNAL) == -1) { //editei para enviar buffer serializado
            remover_conexao(ret);
          }
        }
      }
    
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    i++;
  }
  tela->stop();
  teclado->stop();
  return 0;
}