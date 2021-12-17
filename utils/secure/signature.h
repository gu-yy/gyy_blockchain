#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <string>

#include "md5.h"
#include "rsa.h"

namespace glib{

std::string sign(std::string text,std::string key);
bool valid(std::string signature,std::string text,std::string key);

}

#endif