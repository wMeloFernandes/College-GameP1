#ifndef SERIALIZABLE_HPP
#define SERIALIZABLE_HPP

#include <fstream>
#include <iostream>
#include <string>

#define MAX_MSG_STRING 1000000

struct DataContainer {
  int oldUserNumberUsersOnline;
  int numberUsersOnline;
  float position;
  int life;
  float velocidade;
  float posicaoHorizontal;
  float posicaoVertical;
  int forca;
  int type;
};


class RelevantData {
  private:
    DataContainer data;

  public:
    RelevantData();
    RelevantData(int oldUserNumberUsersOnline,int numberUsersOnline,float position, int life,float velocidade, float posicaoHorizontal, float posicaoVertical, int forca, int type);
    RelevantData(std::string buffer_in);
    void serialize(std::string &buffer_out);
    void unserialize(std::string buffer_in);
    void dump();
    int getOldUserNumberUsersOnline();
    int getNumberUsersOnline();
    float get_position();
    int getLife();
    float get_velocidade();
    float get_posicaoHorizontal();
    float get_posicaoVertical();
    int get_forca();
    int get_type();
};

#endif
