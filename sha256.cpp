#include "sha256.hpp"
#include <iostream>
#include <string.h>
#include <sstream>

//Algorithm Link: https://qvault.io/cryptography/how-sha-2-works-step-by-step-sha-256/

const unsigned int ROUND_CONST[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                                      0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                                      0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                                      0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                                      0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 
                                      0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                                      0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                                      0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

const unsigned int INIT_HASH_VARS[8]   = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

unsigned int choose(const unsigned int& E, const unsigned int& F, const unsigned int& G){
  return (E & F) ^ (~E & G);
}

unsigned int majority(const unsigned int& A, const unsigned int& B, const unsigned int& C){
  return (A & B) ^ (A & C) ^ (B & C);
}

unsigned int leftRotate(unsigned int n, unsigned int d){
  return (n << d)|(n >> (32 - d));
}
 
unsigned int rightRotate(unsigned int n, unsigned int d){
  return (n >> d)|(n << (32 - d));
}

unsigned int sigma0(const unsigned int& A){
  return rightRotate(A, 2) ^ rightRotate(A, 13) ^ rightRotate(A, 22);
}

unsigned int sigma1(const unsigned int& E){
  return rightRotate(E, 6) ^ rightRotate(E, 11) ^ rightRotate(E, 25);
}

unsigned int concat_32bits(const unsigned char& b0, const unsigned char& b1, const unsigned char& b2, const unsigned char& b3){
  unsigned int word = b0;
  word <<= 8;
  word |= b1;
  word <<= 8;
  word |= b2;
  word <<= 8;
  word |= b3;
  return word;
}

unsigned int* makeMessageSchedule(unsigned char* chunk){
  unsigned int* w = new unsigned int[64];
  memset(w, 0, 64);
  for(unsigned int i = 0; i < 16; ++i)
    w[i] = concat_32bits(chunk[(i * 4)], chunk[(i * 4) + 1], chunk[(i * 4) + 2], chunk[(i * 4) + 3]);

  for(unsigned int i = 16; i < 64; ++i){
    unsigned int s0 = rightRotate(w[i - 15], 7) ^ rightRotate(w[i - 15], 18) ^ (w[i - 15] >> 3);
    unsigned int s1 = rightRotate(w[i - 2], 17) ^ rightRotate(w[i - 2], 19) ^ (w[i - 2] >> 10);
    w[i] = w[i - 16] + s0 + w[i - 7] + s1;
  }
  return w;
}

unsigned char* sha256Padding(unsigned char in[], const unsigned int& inLen, unsigned int& outLen){
  outLen = inLen + 65 - ((inLen + 1) % 64);
  unsigned char* fixed_input = new unsigned char[outLen];
  memset(fixed_input, 0, outLen);
  memcpy(fixed_input, in, inLen);
  fixed_input[inLen] = 0x80;
  unsigned long long big_endian = (unsigned long long)inLen * 8;
  for(unsigned int i = outLen; i > outLen - 8; --i){
    fixed_input[i - 1] = big_endian & 0xff;
    big_endian >>= 8;
  }
  return fixed_input;
}

unsigned char* hash::sha256(unsigned char in[], const unsigned int& inLen){
  unsigned int padding_size;
  unsigned char* padding = sha256Padding(in, inLen, padding_size);
  unsigned int h[8];
  for (unsigned int i = 0; i < 8; ++i)
    h[i] = INIT_HASH_VARS[i];
  for (unsigned long long chunk = 0; chunk < padding_size; chunk += 64){
    unsigned int* W = makeMessageSchedule(padding + chunk);
    unsigned int A = h[0], B = h[1], C = h[2], D = h[3];
    unsigned int E = h[4], F = h[5], G = h[6], H = h[7];
    for (unsigned char i = 0; i < 64; ++i){
      unsigned int temp1 = H + sigma1(E) + choose(E, F, G) + ROUND_CONST[i] + W[i];
      unsigned int temp2 = sigma0(A) + majority(A, B, C);
      H = G;
      G = F;
      F = E;
      E = D + temp1;
      D = C;
      C = B;
      B = A;
      A = temp1 + temp2;
    }
    h[0] += A;
    h[1] += B;
    h[2] += C;
    h[3] += D;
    h[4] += E;
    h[5] += F;
    h[6] += G;
    h[7] += H;
    delete[] W;
  }
  delete[] padding;
  unsigned char* hash_bytes = new unsigned char[32];
  for (unsigned int i = 0; i < 8; ++i){
    for(unsigned int j = 4; j > 0; --j){
      hash_bytes[(i * 4) + j - 1] = h[i] & 0xff;
      h[i] >>= 8;
    }
  }
  return hash_bytes;
}

template<typename Int_t>
std::string hexFormat(const Int_t& n){
  unsigned short int_hex_size = sizeof(Int_t) * 2;
  std::stringstream stream_buf;
  stream_buf << std::hex << (unsigned long long)n;
  return std::string(int_hex_size - stream_buf.str().length(), '0') + stream_buf.str();
}

std::string hash::formatedHashValue(unsigned char hash_[], const unsigned int& hLen){
  std::string hash_string = "";
  for (unsigned int i = 0; i < hLen; ++i)
    hash_string += hexFormat(hash_[i]);
  return hash_string;
}