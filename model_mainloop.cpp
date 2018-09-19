#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#include "oo_model.hpp"

using namespace std::chrono;
uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main ()
{
//Corpo::Corpo(float massa, float velocidade, float posicao, float coeficienteMola, float viscosidade) {
  Corpo *c1 = new Corpo(10, 0, 0, 0, 0);
  Corpo *c2 = new Corpo(10, 0, 59, 0, 0);
  unsigned int turn = 1;

  ListaDeCorpos *l = new ListaDeCorpos();
  l->add_corpo(c1);
  l->add_corpo(c2);

  Fisica *f = new Fisica(l);

  Tela *tela = new Tela(l, 20, 20, 20, 20);
  tela->init();

  Teclado *teclado = new Teclado();
  teclado->init();

  uint64_t t0;
  uint64_t t1;
  uint64_t deltaT;
  uint64_t T;

  int i = 0;

  Audio::Sample *asample;
  asample = new Audio::Sample();
  asample->load("assets/blip.dat");

  Audio::Player *player;
  player = new Audio::Player();
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
        player->play(asample);

  while (1) {
    // Atualiza timers
    t0 = t1;
    t1 = get_now_ms();
    deltaT = t1-t0;
    // Atualiza modelo
    f->update(deltaT);
    std::cout<< deltaT;

    // Atualiza tela
    tela->update();

    // Lê o teclado
    char c = teclado->getchar();
    if (c=='w') {
      f->choque('w',deltaT, turn);
      asample->set_position(0);
    }
    if(c=='s'){
      f->choque('s',deltaT, turn);
      asample->set_position(0);
    }    
    if (c=='q') {
      break;
    }

    // Condicao de parada
    if ( (t1-T) > 15000){
      if(turn==1){
        turn=2;
      }else if(turn==2){
        turn=1;
      }
    }

    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    i++;
  }
  player->stop();
  tela->stop();
  teclado->stop();
  return 0;
}