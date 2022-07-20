#include "sha256.hpp"
#include <iostream>
#include <limits>
#include <fstream>
#include <string.h>

int main(){
  std::ifstream file("input.txt");
  std::string msg;
  if(file.is_open()){
    std::string line;
    while(std::getline(file, line)){
      msg += line + "\n";
    }
    msg.pop_back();
  }
  //std::cout << msg << std::endl;
  unsigned char* in = new unsigned char[msg.length()];
  memcpy(in, msg.data(), msg.length());
  std::cout << "Length: " << msg.length() << std::endl;
  unsigned char* hash_msg = hash::sha256(in, msg.length());
  std::cout << "SHA256(BYTE): ";
  for (unsigned int i = 0; i < 32; ++i)
    std::cout << hash_msg[i];
  std::cout << '\n';

  std::cout << "SHA256(HEX): " << hash::formatedHashValue(hash_msg, 32) << std::endl;
  std::cout << "========================================================" << std::endl;
  delete[] hash_msg;
  return 0;
}