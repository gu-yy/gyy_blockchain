#ifndef STATUSDB_H
#define STATUSDB_H

#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <cstring>

#include "leveldb/db.h"
#include "leveldb/write_batch.h"
#include "grena/grena.h"
#include "gini/gini.h"
#include "block.pb.h"
#include "encode/encode.h"
#include "../global/global.h"


namespace store_node{

struct status_data{
    enum code{
        Write = 0,
        Delete = 1,
    };
    status_data():_key(""),_value(""),_op(Write){}
    status_data(std::string& key,std::string& value,code op):_key(key),_value(value),_op(op){}
    status_data(std::string& key,code op):_key(key),_value(""),_op(op){}
    code _op;
    std::string _key;
    std::string _value;
};

class statusdb{
public:
    static statusdb* get_statusdb(){
        if(_statusdb==nullptr){
            _statusdb=new statusdb;
            recover_status(_statusdb,global_ini);
        }
        return _statusdb;
    }
    leveldb::Status Put(std::string& key,std::string& value){
        return _leveldb->Put(leveldb::WriteOptions(),key,value);
    }
    leveldb::Status Get(std::string& key,std::string* value){
        return _leveldb->Get(leveldb::ReadOptions(),key,value);
    }
    leveldb::Status Delete(std::string& key){
        return _leveldb->Delete(leveldb::WriteOptions(),key);
    }
    leveldb::Status batch_process(std::vector<status_data>& batch_datas);

private:
    leveldb::DB* _leveldb;
    leveldb::Options _options;
    leveldb::Status _status;
    static statusdb* _statusdb;
    statusdb(){
        _options.create_if_missing=true;
        leveldb::Status status=leveldb::DB::Open(_options,global_ini["statusdb"]["statusdb_path"].to_string(),&_leveldb);
        // recover_status("/home/hust/final_gyy_tmp/chain_store");
    }
    static void recover_status(statusdb* db,glib::ParseIni& config);
};

}

#endif