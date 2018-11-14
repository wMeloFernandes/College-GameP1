#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#include "oo_model.hpp"

using namespace std::chrono;
uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main () {
  Corpo *c1 = new Corpo(15);
  Corpo *c2 = new Corpo(45);
  Tiro *tt1 = new Tiro(15,15,15,1);
  Tiro *tt2 = new Tiro(15,45,15,1);
  unsigned int turn = 0;
  int mFloorHited = 0;

  ListaDeCorpos *lc = new ListaDeCorpos();
  ListaDeTiros *lt = new ListaDeTiros();
  lc->add_corpo(c1);
  lc->add_corpo(c2);
  lt->add_tiro(tt1);
  lt->add_tiro(tt2);

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
  uint64_t contTiro;
  
  int i = 0;

  Audio::Sample *asample;
  asample = new Audio::Sample();
  asample->load("assets/blip.dat");
  Audio::Sample *bsample;
  bsample = new Audio::Sample();
  bsample->load("assets/saida.dat");

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

  player->play(bsample);
  
  T = get_now_ms();
  t1 = T;

  while (1) {
    // Atualiza timers
    t0 = t1;
    t1 = get_now_ms();
    deltaT = t1-t0;

    // Atualiza tela
    tela->update(t1-T, tiro, turn);    
    // Lê o teclado
    char c = teclado->getchar();

    if (c == 'w' && !tiro) {
      f->movimento('w', turn);
      asample->set_position(0);
      player->play(asample);
    }
    if (c == 's' && !tiro) {
      f->movimento('s', turn);
      asample->set_position(0);
      player->play(asample);
    }

    if (c == '+' && !tiro) {
      f->alteraForca('+', turn);
    }

    if (c== '-' && !tiro) {
	  f->alteraForca('-', turn);
    }

    if (c=='q') {
      break;
    }

    if (c==' ' || tiro) {
      f->tiro(deltaT, turn, &mFloorHited);
      tiro = 1;

      if (mFloorHited) {
		tiro = 0;
		mFloorHited = 0;
	    turn = !turn;
	    T = get_now_ms();
	    t1 = T;
	  }

    }

    // Condicao de parada
    if (!tiro) {
      if ((t1-T) > 15000) {
      	std::this_thread::sleep_for (std::chrono::milliseconds(500));
        turn = !turn;
        T = get_now_ms();
        t1 = T;
      }
    }

    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    i++;
  }
  //player->stop();
  tela->stop();
  teclado->stop();
  return 0;
}