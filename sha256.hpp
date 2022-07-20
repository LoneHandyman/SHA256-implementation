#ifndef SHA_256_HPP_
#define SHA_256_HPP_

#include <string>

namespace hash{

//std::string sha256(const std::string& inputMsg);

unsigned char* sha256(unsigned char in[], const unsigned int& inLen);

std::string formatedHashValue(unsigned char hash_[], const unsigned int& hLen);

}
#endif//SHA_256_HPP_