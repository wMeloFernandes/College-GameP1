#ifndef OO_MODEL_HPP
#define OO_MODEL_HPP

#define TARGET_HITED 1
#define TARGET_NOT_HITED 0

#include "portaudio.h"
#include <vector>
#include <thread>

namespace Audio {

class Sample {
  private:
    std::vector<float> data;
    unsigned int position;

  public:
    Sample();
    ~Sample();
    void load(const char *filename);
    std::vector<float> get_data();
    unsigned int get_position();
    void set_position(unsigned int pos);
    bool finished();

};


class Player {
  private:
    Sample *audio_sample;
    bool playing;

    PaStreamParameters  outputParameters;
    PaStream*           stream;
    PaError             err;
    PaTime              streamOpened;


  public:
    Player();
    ~Player();

    void init();
    void pause();
    void stop();
    void play(Sample *audiosample);
    Sample *get_data();
};

}

class Corpo {
  private:
  float posicao;
  int life;

  public:
  Corpo(float posicao);
  void update(float nova_posicao);
  void updateLife(int life);
  float get_posicao();
  int getLife();
  void setLife();
  void less_life();
};

class ListaDeCorpos {
 private:
    std::vector<Corpo*> *corpos;

  public:
    ListaDeCorpos();
    void hard_copy(ListaDeCorpos *ldc);
    void add_corpo(Corpo *c);
    std::vector<Corpo*> *get_corpos();
};

class Tiro {
  private:
  float velocidade;
  float posicaoHorizontal;
  float posicaoVertical;
  float forca;

  public:
  Tiro(float velocidade, float posicaoHorizontal, float posicaoVertical, float forca);
  void update(float nova_velocidade, float nova_posicao_horizontal, float nova_posicao_vertical, float nova_forca);
  float get_velocidade();
  float get_posicaoHorizontal();
  float get_posicaoVertical();
  float get_forca();
  void setDownForca();
  void setUpForca();
};

class ListaDeTiros {
 private:
    std::vector<Tiro*> *tiros;

  public:
    ListaDeTiros();
    void hard_copy(ListaDeTiros *ldt);
    void add_tiro(Tiro *t);
    std::vector<Tiro*> *get_tiros();
};

class Fisica {
  private:
    ListaDeCorpos *lista_corpos;
    ListaDeTiros *lista_tiros;  

  public:
    Fisica(ListaDeCorpos *ldc, ListaDeTiros *ldt);
    void movimento(char option, int id);
    void tiro(float deltaT, int *mFloorHited,int id);
    void alteraForca (char option, int id);
};

class Tela {
  private:
    ListaDeCorpos *lista_corpos, *lista_corpos_anterior;
    ListaDeTiros *lista_tiros, *lista_tiros_anterior;
    int maxI, maxJ;
    float maxX, maxY;

  public:
    Tela(ListaDeCorpos *ldc, ListaDeTiros *ldt, int maxI, int maxJ, float maxX, float maxY);
    ~Tela();
    void stop();
    void init();
    void update(unsigned int t, unsigned int tiro);
};

void threadfun (char *keybuffer, int *control);

class Teclado {
  private:
    char ultima_captura;
    int rodando;

    std::thread kb_thread;

  public:
    Teclado();
    ~Teclado();
    void stop();
    void init();
    char getchar();
};

#endif