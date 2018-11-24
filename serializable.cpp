
#include "serializable.hpp"

#include <string>
#include <cstring>


RelevantData::RelevantData() {
};


RelevantData::RelevantData(int oldUserNumberUsersOnline,int numberUsersOnline,float position, int life,float velocidade, float posicaoHorizontal, float posicaoVertical, float forca, int type) {
  this->data.oldUserNumberUsersOnline = oldUserNumberUsersOnline;
  this->data.numberUsersOnline = numberUsersOnline;  
  this->data.position = position;
  this->data.life = life;
  this->data.velocidade = velocidade;
  this->data.posicaoHorizontal = posicaoHorizontal;
  this->data.posicaoVertical =posicaoVertical ;
  this->data.forca = forca;
  this->data.type =type	 ;
   
}

RelevantData::RelevantData(std::string buffer_in) {
  this->unserialize(buffer_in);
}


int RelevantData::getOldUserNumberUsersOnline(){
  return this->data.oldUserNumberUsersOnline;
}

int RelevantData::getNumberUsersOnline(){
  return this->data.numberUsersOnline;
}


float RelevantData::get_position() {
  return this->data.position;
}

int RelevantData::getLife(){
  return this->data.life;
}

float RelevantData::get_velocidade(){
	return this->data.velocidade;
}

float RelevantData::get_posicaoHorizontal(){
	return this->data.posicaoHorizontal;
}

float RelevantData::get_posicaoVertical(){
	return this->data.posicaoVertical;
}

float RelevantData::get_forca(){
	return this->data.forca;
}

int RelevantData::get_type(){
	return this->data.type;
}

void RelevantData::serialize(std::string &buffer_out) {
   sprintf((char*) buffer_out.c_str() , "%d %d %f %d %f %f %f %f %d", this->data.oldUserNumberUsersOnline,this->data.numberUsersOnline,this->data.position, this->data.life,this->data.velocidade, this->data.posicaoHorizontal,this->data.posicaoVertical,this->data.forca,this->data.type);
}

void RelevantData::unserialize(std::string buffer_in) {
  float a;
  int c,d,e;
  sscanf(buffer_in.c_str(), "%d %d %f %d %f %f %f %f %d", &this->data.oldUserNumberUsersOnline,&this->data.numberUsersOnline,&this->data.position, &this->data.life,&this->data.velocidade, &this->data.posicaoHorizontal,&this->data.posicaoVertical,&this->data.forca,&this->data.type);
}

void RelevantData::dump() {
  std::cout << "Position x:" << this->data.position << '\n';
  std::cout << "Life:" << this->data.life << '\n';
  std::cout << "Old Number:" << this->data.oldUserNumberUsersOnline << '\n';
  std::cout << "New Number:" << this->data.numberUsersOnline << '\n';
  std::cout << "Velocidade:" << this->data.velocidade << '\n';
  std::cout << "Pos HOR:" << this->data.posicaoHorizontal << '\n';
  std::cout << "Pos VER:" << this->data.posicaoVertical << '\n';
  std::cout << "FORCA:" << this->data.forca << '\n';
  std::cout << "Type:" << this->data.type << '\n';
  
   
}