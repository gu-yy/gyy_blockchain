
#include "statusdb.h"

namespace store_node{
    statusdb* statusdb::_statusdb=nullptr;
    static void handle_block(const char* block_content,uint32_t size,statusdb* db){
        blockchain::block block;
        block.ParseFromArray(block_content,size);
        global_block_number=block.block_header().number();
        // auto it=block.block_body().set().rd_set().begin();
        // std::string key=it->key();
        bool flag=true;
        for(auto it=block.block_body().txs().begin();
            it!=block.block_body().txs().end();++it){
                for(auto tx_rd_set_it=it->tx_content().rd_set().begin();
                    tx_rd_set_it!=it->tx_content().rd_set().end();++tx_rd_set_it){
                        std::string key=tx_rd_set_it->key();
                        std::string value=tx_rd_set_it->value().value();
                        uint32_t block_number=tx_rd_set_it->value().blcok_number();
                        uint32_t version=tx_rd_set_it->value().version();
                        std::string statusdb_value;
                        // todo:记录返回状态
                        leveldb::Status status=db->Get(key,&statusdb_value);
                        blockchain::integrated_value parsed_value;
                        parsed_value.ParseFromString(statusdb_value);
                        if(block_number!=parsed_value.blcok_number() ||
                            version!=parsed_value.version() ||
                            value!=parsed_value.value()){
                                flag=false;
                                break;
                            }
                    }
                if(!flag){
                    break;
                }
        }
        if(flag){
            std::vector<status_data> write_datas;
            for(auto it=block.block_body().txs().begin();
                it!=block.block_body().txs().end();++it){
                    for(auto tx_wt_set_it=it->tx_content().wt_set().begin();
                        tx_wt_set_it!=it->tx_content().wt_set().end();++tx_wt_set_it){
                            std::string key=tx_wt_set_it->key();
                            blockchain::integrated_value value_struct=tx_wt_set_it->value();
                            value_struct.set_blcok_number(block.block_header().number());
                            std::string value=value_struct.SerializeAsString();
                            write_datas.push_back(status_data{key,value,status_data::Write});
                        }
                }
            // todo:记录返回状态
            leveldb::Status status=db->batch_process(write_datas);
        }
    }
    static void handle_blockfile(const char* blockfile_content,uint32_t size,statusdb* db){
        const char* cur_ptr=blockfile_content;
        uint32_t idx=0;
        while(idx<size){
            uint32_t length=glib::DecodeCharArray2Int(cur_ptr);
            cur_ptr+=4;
            char block_content[length];
            memcpy(block_content,cur_ptr,length);
            cur_ptr+=length;
            idx+=(4+length);
            handle_block(blockfile_content,length,db);
        }
    }

    void statusdb::recover_status(statusdb* db,glib::ParseIni& config){
        glib::Grena* blockfile_cache=glib::Grena::get_grena(config["blockchain"]["blockfile_size"].to_int());
        std::string statusdb_dir_name=config["blockchain"]["blockchain_store_path"].to_string();
        if(0!=access(statusdb_dir_name.c_str(),F_OK)){
            mk_multi_dir(statusdb_dir_name);
        }

        size_t file_number=0;
        std::string last_file_content;
        while(true){
            std::string blockfile_name=statusdb_dir_name;
            if(blockfile_name.back()!='/'){
                blockfile_name.push_back('/');
            }
            blockfile_name+="blockfile_"+std::to_string(file_number);
            file_number+=1;
            if(0!=access(blockfile_name.c_str(),F_OK)){
                break;
            }
            last_file_content=read_file_once(blockfile_name);
            handle_blockfile(last_file_content.c_str(),last_file_content.size(),db);
        }
        char* file_content_start_ptr=blockfile_cache->allocate(last_file_content.size());
        memcpy(file_content_start_ptr,last_file_content.c_str(),last_file_content.size());
        global_blockfile_number=file_number;
    }


    leveldb::Status statusdb::batch_process(std::vector<status_data>& batch_datas){
        leveldb::WriteBatch write_batch;
        for(size_t idx=0;idx<batch_datas.size();++idx){
            if(batch_datas[idx]._op==status_data::Delete){
                write_batch.Delete(batch_datas[idx]._key);
            }else{
                write_batch.Put(batch_datas[idx]._key,batch_datas[idx]._value);
            }
        }
        return _leveldb->Write(leveldb::WriteOptions(),&write_batch);
    }
}