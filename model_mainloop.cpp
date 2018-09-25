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
  Corpo *c1 = new Corpo(0);
  Corpo *c2 = new Corpo(59);
  Tiro *tt1 = new Tiro(0,0,14);
  Tiro *tt2 = new Tiro(0,59,14);
  unsigned int turn = 0;
  int mTargetCallback = 1;

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

  //Audio::Sample *asample;
  //asample = new Audio::Sample();
  //asample->load("assets/blip.dat");

  //Audio::Player *player;
  //player = new Audio::Player();
  //player->init();

  // Espera
  while (1) {
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    t1 = get_now_ms();
    if (t1-t0 > 500) break;
  }

  //player->play(asample);
  
  T = get_now_ms();
  t1 = T;
  //player->play(asample);

  while (1) {
    // Atualiza timers
    t0 = t1;
    t1 = get_now_ms();
    deltaT = t1-t0;
    // Atualiza modelo
    f->update(deltaT);

    // Atualiza tela
    tela->update(t1-T, tiro);    
    // Lê o teclado
    char c = teclado->getchar();
    if (c == 'w' && !tiro) {
      f->movimento('w', turn, TARGET_NOT_HITED);
      //asample->set_position(0);
    }
    if (c == 's' && !tiro) {
      f->movimento('s', turn, TARGET_NOT_HITED);
      //asample->set_position(0);
    }

    if(c=='m' && !tiro){
      f->movimento('m', turn,TARGET_NOT_HITED);         //Deixei funcionando a barra de vida, a efeito de demosntração, para você ver o resultado.
    }

    // if(c=='m' && mTargetCallback == TARGET_HITED){       LUANA = Essa função será utilizada para diminui a barra de vida 
    //   f->movimento('m', turn, TARGET_HITED);             do jogador. Será necessário que aperte a tecla m (QUE DEVERÁ SER A TECLA
    // }                                                     DE ESPAÇO) e além disso, você deve dizer par a função se o jogador acertou o alvo. Essa variável que eu chamei de mTargetCallback, passa pra ela se foi atingido o alvo o não!
    if (c=='q') {
      break;
    }

    if (c=='t' || tiro) {
      f->tiro(deltaT, turn);
      tiro = 1;
      contTiro++;
      if (contTiro >= 30) {
        tiro = 0;
        contTiro = 0;
        turn = !turn;
      T = get_now_ms();
      t1 = T;
    }
    }

    // Condicao de parada
    if (!tiro) {
      if ((t1-T) >10000) {
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