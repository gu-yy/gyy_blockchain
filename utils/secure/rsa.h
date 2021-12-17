#ifndef RSA_H
#define RSA_H
#include <iostream>
#include <string>
#include <gmpxx.h>
#include <gmp.h>
#include <unistd.h>
#include <vector>
#include <algorithm>

namespace glib{
void generate_key(std::string* pub_key,std::string* pri_key);
std::string encrypt(std::string& message,std::string& pub_key);
std::string decrypt(std::string& message,std::string& pri_key);
}



#endif