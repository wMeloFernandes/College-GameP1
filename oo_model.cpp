#include <vector>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <cstring>

#include "oo_model.hpp"

#include <ncurses.h>
using namespace std::chrono;
using namespace Audio;

Sample::Sample() {

}

Sample::~Sample() {

}

bool Sample::finished() {
  if ( (this->position) >= (this->data.size())) return true;
  else return false;
}

void Sample::load(const char *filename) {
  std::string buffer;
  float fdata;
  std::ifstream input_file;
  unsigned int count = 0;

  input_file.open(filename, std::ios_base::in);
  if (!input_file) {
    std::cerr << "Arquivo " << filename << " nao encontrado" << std::endl;
    return;
  }

  while (std::getline(input_file, buffer) ) {
    std::stringstream(buffer) >> fdata;
    (this->data).push_back(fdata);
    count ++;
  }
  this->position = 0;

  std::cerr << "Total: " << count << " samples" << std::endl;

}

unsigned int Sample::get_position() {
  return this->position;
}

void Sample::set_position(unsigned int pos) {
  this->position = pos;
}

std::vector<float> Sample::get_data() {
  return this->data;
}


Player::Player() {
  this->playing = false;
  this->audio_sample = NULL;
}

void Player::pause() {
  this->playing = false;
}

Player::~Player() {

}

Sample *Player::get_data() {
  return this->audio_sample;
}


int mix_and_play (const void *inputBuffer, void *outputBuffer,
                  unsigned long framesPerBuffer,
                  const PaStreamCallbackTimeInfo* timeInfo,
                  PaStreamCallbackFlags statusFlags,
                  void *userData )

{
  Player *player = (Player*) userData;
  float *buffer = (float *) outputBuffer;
  Sample *s;
  s = player->get_data();
  if (s != NULL) {
    std::vector<float> data = s->get_data();
    unsigned int pos = s->get_position();

    // Fill the buffer with samples!
    for (int i=0; (i<framesPerBuffer); i++) {
      if (pos < data.size())
        buffer[i] = data[pos];
      else
        buffer[i] = 0;
      pos++;
    }
    s->set_position(pos);
  }
  return 0;
}

void Player::play(Sample *audiosample) {
  this->audio_sample = audiosample;
}

void Player::init() {
  PaError err;

  err = Pa_Initialize();
  if( err != paNoError ) {
    std::cerr << "Error on Pa_Initialize()" << std::endl;
    return;
  }

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* Default output device. */
  if (outputParameters.device == paNoDevice) {
    std::cerr << "Error: No default output device on Pa_GetDefaultOutputDevice()" << std::endl;
    return;
  }

  outputParameters.channelCount = 1;                     /* Mono output. */
  outputParameters.sampleFormat = paFloat32;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;
  err = Pa_OpenStream( &stream,
                         NULL,      /* No input. */
                         &outputParameters,
                         44100,
                         1024,       /* Frames per buffer. */
                         paClipOff, /* We won't output out of range samples so don't bother clipping them. */
                         mix_and_play,
                         this );

  if( err != paNoError ) {
    std::cerr << "Error on Pa_OpenStream()" << std::endl;
    return;
  }

  err = Pa_StartStream( stream );
  if( err != paNoError ) {
    std::cerr << "Error on Pa_StartStream()" << std::endl;
    return;
  }


}

void Player::stop() {
  PaError err;
  err = Pa_StopStream( stream );
  if( err != paNoError ) {
    std::cerr << "Error on Pa_StopStream()" << std::endl;
    return;
  }

  err = Pa_CloseStream( stream );
  if( err != paNoError ) {
    std::cerr << "Error on Pa_StopStream()" << std::endl;
    return;
  }

  Pa_Terminate();

}

Tiro::Tiro (float velocidade, float posicaoHorizontal, float posicaoVertical, float forca) {
  this->velocidade = velocidade;
  this->posicaoHorizontal = posicaoHorizontal;
  this->posicaoVertical = posicaoVertical;
  this->forca = forca;
}

void Tiro::update(float nova_velocidade, float nova_posicao_horizontal, float nova_posicao_vertical, float nova_forca) {
  this->velocidade = nova_velocidade;
  this->posicaoHorizontal = nova_posicao_horizontal;
  this->posicaoVertical = nova_posicao_vertical;
  this->forca = forca;
}

float Tiro::get_velocidade() {
  return this->velocidade;
}

float Tiro::get_posicaoHorizontal() {
  return this->posicaoHorizontal;
}

float Tiro::get_posicaoVertical() {
  return this->posicaoVertical;
}

float Tiro::get_forca(){
  return this->forca;
}

void Tiro::setDownForca(){
  this->forca = this->forca - 1;
}

void Tiro::setUpForca(){
  this->forca = this->forca + 1;
}

ListaDeTiros::ListaDeTiros() {
  this->tiros = new std::vector<Tiro *>(0);
}

void ListaDeTiros::hard_copy(ListaDeTiros *ldt) {
  std::vector<Tiro *> *tiros = ldt->get_tiros();

  for (int k=0; k<tiros->size(); k++) {
    Tiro *t = new Tiro( (*tiros)[k]->get_velocidade(),\
                          (*tiros)[k]->get_posicaoHorizontal(),\
                          (*tiros)[k]->get_posicaoVertical(), \
                          (*tiros)[k]->get_forca()
                        );
    this->add_tiro(t);
  }

}

void ListaDeTiros::add_tiro(Tiro *t) {
  (this->tiros)->push_back(t);
}

std::vector<Tiro*> *ListaDeTiros::get_tiros() {
  return (this->tiros);
}

Corpo::Corpo(float posicao) {
  this->posicao = posicao;
  this->life = 3; 
}

void Corpo::update(float nova_posicao) {
  this->posicao = nova_posicao;
}

void Corpo::updateLife(int life){
  this->life = life;
}


float Corpo::get_posicao() {
  return this->posicao;
}

int Corpo::getLife(){
  return this->life;
}

void Corpo::setLife(){
  int x = getLife();
  x = x-1;
  this->life = x;
}

void Corpo::less_life() {
  this->life = this-> life -1;
}

ListaDeCorpos::ListaDeCorpos() {
  this->corpos = new std::vector<Corpo *>(0);
}

void ListaDeCorpos::hard_copy(ListaDeCorpos *ldc) {
  std::vector<Corpo *> *corpos = ldc->get_corpos();

  for (int k=0; k<corpos->size(); k++) {
    Corpo *c = new Corpo((*corpos)[k]->get_posicao());
    this->add_corpo(c);
  }

}

void ListaDeCorpos::add_corpo(Corpo *c) {
  (this->corpos)->push_back(c);
}

std::vector<Corpo*> *ListaDeCorpos::get_corpos() {
  return (this->corpos);
}

Fisica::Fisica(ListaDeCorpos *ldc, ListaDeTiros *ldt) {
  this->lista_corpos = ldc;
  this->lista_tiros = ldt;
}

void Fisica::alteraForca (char option, unsigned int turn,int id) {
  std::vector<Tiro *> *t = this->lista_tiros->get_tiros();
  if (option =='+') {
    if((*t)[id]->get_forca() < 10) (*t)[id]->setUpForca();
  }
  else if (option == '-') {
    if ((*t)[id]->get_forca() > 1) (*t)[id]->setDownForca();
  }
}


void Fisica::movimento(char option, unsigned int turn,int id) {
  // Atualiza parametros dos corpos!
  std::vector<Corpo *> *c = this->lista_corpos->get_corpos();
  std::vector<Tiro *> *t = this->lista_tiros->get_tiros(); 
  int new_pos;
  int new_life;
  new_pos = ((*c)[id]->get_posicao());
  new_life = ((*c)[id]->getLife());
  if (option=='w') {
    if (new_pos<59) {
      ++new_pos;   
    }
  } 
  else if (option=='s') {
    if (new_pos>0) {
      --new_pos;
    }
  }

  (*c)[turn]->updateLife(new_life);
  (*c)[turn]->update(new_pos);
  (*t)[turn]->update(15, new_pos, 15, ((*t)[turn]->get_forca()));    
}

void Fisica::tiro(float deltaT, unsigned int turn, int *mFloorHited, int id) {
  // Atualiza parametros dos corpos!
  std::vector<Tiro *> *t = this->lista_tiros->get_tiros(); 
  std::vector<Corpo *> *c = this->lista_corpos->get_corpos(); 

<<<<<<< HEAD
  float new_vel = (*t)[id]->get_velocidade() + (float)deltaT * (-10.0)/1000;
  float new_pos_hor;
  if (!turn) new_pos_hor = (*t)[turn]->get_posicaoHorizontal() + 0.2*(*t)[turn]->get_forca();
  else new_pos_hor = (*t)[turn]->get_posicaoHorizontal() - 0.2*(*t)[turn]->get_forca();
  float new_pos_ver = (*t)[turn]->get_posicaoVertical() - (float)deltaT * new_vel/1000;

  if (new_pos_ver >= 15) {

  	if(new_pos_hor == (*c)[!turn]->get_posicao() || new_pos_hor == (*c)[!turn]->get_posicao()+1 || new_pos_hor == (*c)[!turn]->get_posicao()+2 || new_pos_hor == (*c)[!turn]->get_posicao()+3 || new_pos_hor == (*c)[!turn]->get_posicao()-1 || new_pos_hor == (*c)[!turn]->get_posicao()-2 || new_pos_hor == (*c)[!turn]->get_posicao()-3) {
	    int new_life;
	    new_life = ((*c)[!turn]->getLife());

	    if (new_life>0) {
	      --new_life;
	    }

	    (*c)[!turn]->updateLife(new_life);
  	}

    new_pos_ver = 15;
    new_pos_hor = (*c)[turn]->get_posicao();
    new_vel = 15;
    *mFloorHited = 1;
  }

  (*t)[turn]->update(new_vel, new_pos_hor, new_pos_ver, (*t)[turn]->get_forca());   
}

Tela::Tela(ListaDeCorpos *ldc, ListaDeTiros *ldt, int maxI, int maxJ, float maxX, float maxY) {
  this->lista_corpos = ldc;
  this->lista_corpos_anterior = new ListaDeCorpos();
  this->lista_corpos_anterior->hard_copy(this->lista_corpos);
  this->lista_tiros = ldt;
  this->lista_tiros_anterior = new ListaDeTiros();
  this->lista_tiros_anterior->hard_copy(this->lista_tiros);
  this->maxI = maxI;
  this->maxJ = maxJ;
  this->maxX = maxX;
  this->maxY = maxY;
}

void Tela::init() {
  initscr();             /* Start curses mode     */
  raw();                 /* Line buffering disabled */
  curs_set(0);           /* Do not display cursor */
}

void Tela::update(unsigned int t, unsigned int tiro, unsigned turn) {
  int i, j;

  std::vector<Corpo*> *corpos_old = this->lista_corpos_anterior->get_corpos();
  std::vector<Tiro*> *tiros_old = this->lista_tiros_anterior->get_tiros();

  // Apaga corpos na tela
  for (int k=0; k<corpos_old->size(); k++) {
    j = (int) ((*corpos_old)[k]->get_posicao()) * \
        (this->maxJ / this->maxY);
    if(j>-20 && j<60){
      move(15, j);   /* Move cursor to position */
      echochar(' ');  /* Prints character, advances a position */
    }
  }

  for (int k=0; k<tiros_old->size(); k++) {
    i = (int) ((*tiros_old)[k]->get_posicaoVertical()) * \
        (this->maxI / this->maxX);
    j = (int) ((*tiros_old)[k]->get_posicaoHorizontal()) * \
        (this->maxJ / this->maxY);
    if(i>-20 && i<60 && j>-20 && j<60) {
      move(i, j);   /* Move cursor to position */
      echochar(' ');  /* Prints character, advances a position */
    }
  }
  // Desenha corpos na tela
	std::vector<Corpo *> *corpos = this->lista_corpos->get_corpos();
	std::vector<Tiro *> *tiros = this->lista_tiros->get_tiros();

  for (int k=0; k<corpos->size(); k++) {
  	if ((*corpos)[k]->getLife() > 0) {
	    j = (int) ((*corpos)[k]->get_posicao()) * \
	        (this->maxJ / this->maxY);
	     if(j>-20 && j<60){
	        move(15, j);   /* Move cursor to position */
	        echochar('*');  /* Prints character, advances a position */
	     }
	    // Atualiza corpos antigos
	    (*corpos_old)[k]->update((*corpos)[k]->get_posicao());
	}

  }

  for (int k=0; k<tiros->size(); k++) {

  	if ((*corpos)[k]->getLife() > 0) {
	    i = (int) ((*tiros)[k]->get_posicaoVertical()) * \
	        (this->maxI / this->maxX);
	    j = (int) ((*tiros)[k]->get_posicaoHorizontal()) * \
	        (this->maxJ / this->maxY);
	     if(i>-20 && i<60 && j>-20 && j<60){
	      move(i, j);   /* Move cursor to position */
	      echochar('*');  /* Prints character, advances a position */
	     }
	    // Atualiza corpos antigos
	    (*tiros_old)[k]->update((*tiros)[k]->get_velocidade(), (*tiros)[k]->get_posicaoHorizontal(), (*tiros)[k]->get_posicaoVertical(), (*tiros)[k]->get_forca());
  	}
  }


  for (int k=0; k<corpos_old->size(); k++) {
    if(corpos_old->size()>0){
      if(k==0){
        //Position Player 1
        char player1[] = "Player 1";
        move(2,10);
        for(int i=0;i<strlen(player1);i++){
          echochar(player1[i]);
        }
         move(3,13);
        for(int i=0;i<=(*corpos)[0]->getLife();i++){
          echochar(' ');  
        }
        move(3,13);
        for(int i=0;i<(*corpos)[0]->getLife();i++){
        echochar('-');  
        }
        for(int i=1; i<=15;i++){
          move(14 - i, 5);
          echochar(' ');  
        }
        for(int i=1; i<=(*tiros)[0]->get_forca();i++){
          move(14 - i, 5);
          echochar('|');  
        }

      }
      if(k==1){
       //Position Player 2
        char player2[] = "Player 2";
        move(2,38);
        for(int i=0; i<strlen(player2);i++){
          echochar(player2[i]);    
        }
         move(3,43);
          for(int i=0;i<=(*corpos)[1]->getLife();i++){
            echochar(' ');  
          }
        move(3,43);
          for(int i=0;i<(*corpos)[1]->getLife();i++){
            echochar('-');  
          }
        for(int i=1; i<=15;i++){
          move(14 - i, 55);
          echochar(' ');  
        }

        for(int i=1; i<=(*tiros)[1]->get_forca();i++){
          move(14 - i, 55);
          echochar('|');  
        }
      }
      if(k==2){
        //Position Player 3
        char player1[] = "Player 3";
        move(2,24);
        for(int i=0;i<strlen(player1);i++){
          echochar(player1[i]);
        }
        move(3,29);
        for(int i=0;i<=(*corpos)[2]->getLife();i++){
          echochar(' ');  
        }
        move(3,29);
        for(int i=0;i<(*corpos)[2]->getLife();i++){
        echochar('-');  
        }
          for(int i=1; i<=15;i++){
            move(14 - i, 8);
            echochar(' ');  
          }
          for(int i=1; i<=(*tiros)[2]->get_forca();i++){
            move(14 - i, 8);
            echochar('|');  
          }

      }
      if(k==3){
        //Position Player 4
        char player2[] = "Player 4";
        move(2,50);
        for(int i=0; i<strlen(player2);i++){
          echochar(player2[i]);    
        }
        move(3,55);
          for(int i=0;i<=(*corpos)[3]->getLife();i++){
            echochar(' ');  
          }
        move(3,55);
          for(int i=0;i<(*corpos)[3]->getLife();i++){
            echochar('-');  
          }

        for(int i=1; i<=15;i++){
          move(14 - i, 58);
          echochar(' ');  
        }

        for(int i=1; i<=(*tiros)[3]->get_forca();i++){
          move(14 - i, 58);
          echochar('|');  
        }
      }
    }
  }
  
  // Atualiza tela
  refresh();
}

void Tela::stop() {
  endwin();
}

Tela::~Tela() {
  this->stop();;
}

void threadfun (char *keybuffer, int *control)
{
  char c;
  while ((*control) == 1) {
    c = getch();
    if (c!=ERR) (*keybuffer) = c;
    else (*keybuffer) = 0;
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
  }
  return;
}

Teclado::Teclado() {
}

Teclado::~Teclado() {
}

void Teclado::init() {
  // Inicializa ncurses
  raw();                 /* Line buffering disabled */
  keypad(stdscr, TRUE);  /* We get F1, F2 etc..   */
  noecho();              /* Don't echo() while we do getch */
  curs_set(0);           /* Do not display cursor */

  this->rodando = 1;
  std::thread newthread(threadfun, &(this->ultima_captura), &(this->rodando));
  (this->kb_thread).swap(newthread);
}

void Teclado::stop() {
  this->rodando = 0;
  (this->kb_thread).join();
}

char Teclado::getchar() {
  char c = this->ultima_captura;
  this->ultima_captura = 0;
  return c;
}