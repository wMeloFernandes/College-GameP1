#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <chrono>

#include "serializable.hpp"
#include "oo_model.hpp"

using namespace std::chrono;
uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int socket_fd;
ListaDeCorpos *lc;
ListaDeTiros *lt;
int oldUserNumberUsersOnline=0;
int numberUsersOnline=0;

void *receber_respostas(void *parametros) {
  /* Recebendo resposta */
  char reply[MAX_MSG_STRING];
  int msg_len;
  int msg_num;
  msg_num = 0;
  RelevantData DadosCorpo(0,0,0,0,0,0,0,0,1);

  while(1) {
    msg_len = recv(socket_fd, reply, MAX_MSG_STRING, MSG_DONTWAIT);

    if (msg_len > 0) {

      char *ptr = strtok (reply,"#");
      std::vector<Corpo *> *c_ptr = lc->get_corpos();
      std::vector<Tiro *> *t_ptr = lt->get_tiros();
      int h=0;
      int p=0;
      while(ptr != NULL) {
        DadosCorpo.unserialize(ptr);
        //DadosCorpo.dump();
        ptr = strtok (NULL,"#\n\0");
        oldUserNumberUsersOnline = DadosCorpo.getOldUserNumberUsersOnline();
        numberUsersOnline = DadosCorpo.getNumberUsersOnline();
        if(DadosCorpo.get_type()==1){
        	if( h < (*c_ptr).size() ) { //Atualiza corpo
    
          		(*c_ptr)[h]->update(  DadosCorpo.get_position());
          		(*c_ptr)[h]->updateLife(  DadosCorpo.getLife());
          		//oldUserNumberUsersOnline = DadosCorpo.getOldUserNumberUsersOnline();
          		//numberUsersOnline = DadosCorpo.getNumberUsersOnline();
        	}
        	else { //Novo corpo
          		Corpo *c1 = new Corpo(DadosCorpo.get_position());
          		lc->add_corpo(c1);
          		//oldUserNumberUsersOnline = DadosCorpo.getOldUserNumberUsersOnline();
          		//numberUsersOnline = DadosCorpo.getNumberUsersOnline();
        	}
        	h++;
        }
        else if(DadosCorpo.get_type()==2){
        	if( p < (*t_ptr).size() ) { //Atualiza tiro
    
          		(*t_ptr)[p]->update(DadosCorpo.get_velocidade(),DadosCorpo.get_posicaoHorizontal(),DadosCorpo.get_posicaoVertical(),DadosCorpo.get_forca());
          		//oldUserNumberUsersOnline = DadosCorpo.getOldUserNumberUsersOnline();
          		//numberUsersOnline = DadosCorpo.getNumberUsersOnline();
        	}
        	else { //Novo Tiro
          		Tiro *t1 = new Tiro(DadosCorpo.get_velocidade(),DadosCorpo.get_posicaoHorizontal(),DadosCorpo.get_posicaoVertical(),DadosCorpo.get_forca());
          		lt->add_tiro(t1);
          		//oldUserNumberUsersOnline = DadosCorpo.getOldUserNumberUsersOnline();
          		//numberUsersOnline = DadosCorpo.getNumberUsersOnline();
        	}
        	p++;

        }
      }
    }
  }
}


int main() {
  struct sockaddr_in target;
  pthread_t receiver;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  int mFloorHited = 0;

  lc = new ListaDeCorpos();
  lt = new ListaDeTiros();

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
  
  int i = 0;

  Audio::Sample *asample;
  asample = new Audio::Sample();
  asample->load("assets/blip.dat");

  Audio::Player *player;
  player = new Audio::Player();
  freopen("/dev/null", "w", stderr);
  player->init();
  
  // Espera
  while (1) {
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    t1 = get_now_ms();
    if (t1-t0 > 500) break;
  }

  player->play(asample);
  
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

    if(numberUsersOnline>0){

      if(oldUserNumberUsersOnline < numberUsersOnline){
        tela = new Tela(lc, lt, 20, 20, 20, 20);
        tela->init();
      }
      else{
      // Atualiza tela
        tela->update(t1-T, tiro);
      }

      char c = teclado->getchar();
      char answer[5];
      answer[0]=c;
      
      if (c == 'w') {
      	//std::cout << "TAMANHO LISTA CORPOS:" << lc->get_corpos()->size() << '\n';
      	//std::cout << "TAMANHO LISTA TIROS:" << lt->get_tiros()->size() << '\n';
        send(socket_fd,answer, 5, 0);
        asample->set_position(0);
        player->play(asample);
      }
      if (c == 's') {
        send(socket_fd,answer, 5, 0);
        asample->set_position(0);
        player->play(asample);
      }

      if (c == '+') {
        send(socket_fd,answer, 5, 0);
        asample->set_position(0);
        player->play(asample);
      }

      if (c== '-') {
        send(socket_fd,answer, 5, 0);
        asample->set_position(0);
        player->play(asample);
      }

      if(c=='m'){
      	//std::cout << "TAMANHO LISTA TIROS:" << lt->get_tiros()->size() << '\n';
        send(socket_fd,answer, 5, 0);
        asample->set_position(0);
        player->play(asample);
      }

      if (c=='q') {
        send(socket_fd,answer, 5, 0);
      }
    }
    
  }
  player->stop();
  tela->stop();
  teclado->stop();
  return 0;
}

