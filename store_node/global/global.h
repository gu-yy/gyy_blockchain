#ifndef GLOBAL_H
#define GLOBAL_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>

#include "identity.pb.h"

#include "gini/gini.h"

namespace store_node{

std::string read_file_once(std::string filename);
int write_file_once(std::string filename,const char* message,int length);
int mk_multi_dir(std::string& path);


extern glib::ParseIni global_ini;
extern blockchain::ApplyPubKeyCertReply global_node_cert;

extern size_t global_blockfile_number;
extern size_t global_block_number;

}


#endif