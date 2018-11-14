
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <chrono>

#include "oo_model.hpp"

using namespace std::chrono;
uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int socket_fd;

void *receber_respostas(void *parametros) {
  /* Recebendo resposta */
  char reply[60];
  int msg_len;
  int msg_num;
  msg_num = 0;
  while(1) {
    msg_len = recv(socket_fd, reply, 5, MSG_DONTWAIT);
    if (msg_len > 0) {
      msg_num++;
    }
  }
}

int main() {
  struct sockaddr_in target;
  pthread_t receiver;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  int mFloorHited = 0;

  ListaDeCorpos *lc = new ListaDeCorpos();
  ListaDeTiros *lt = new ListaDeTiros();

  Fisica *f = new Fisica(lc, lt);

  Tela *tela = new Tela(lc, lt, 20, 20, 20, 20);
  tela->init();

  Teclado *teclado = new Teclado();
  teclado->init();

  uint64_t t0;
  uint64_t t1;
  uint64_t deltaT;
  uint64_t T;
  
  int i = 0;
  // Espera
  while (1) {
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    t1 = get_now_ms();
    if (t1-t0 > 500) break;
  }

  T = get_now_ms();
  t1 = T;
  target.sin_family = AF_INET;
  target.sin_port = htons(3001);
  inet_aton("127.0.0.1", &(target.sin_addr));
  if (connect(socket_fd, (struct sockaddr*)&target, sizeof(target)) != 0) {
    printf("Problemas na conexao\n");
    return 0;
  }

  pthread_create(&receiver, NULL, receber_respostas, NULL);

  while(1) {
    char c = teclado->getchar();
    char answer[5];
    answer[0]=c;
    
    if (c == 'w') {
      send(socket_fd,answer, 5, 0);
    }
    if (c == 's') {
      send(socket_fd,answer, 5, 0);
    }

    if (c == '+') {
      send(socket_fd,answer, 5, 0);
    }

    if (c== '-') {
      send(socket_fd,answer, 5, 0);
    }

    if(c=='m'){
      send(socket_fd,answer, 5, 0);
    }

    if (c=='q') {
      send(socket_fd,answer, 5, 0);
    }
  }

  tela->stop();
  teclado->stop();
  return 0;
}

