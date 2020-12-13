#ifndef DB_H
#define DB_H
#include "BPTree.h"
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
// #define DB_DB
// #define BPTree_DBUG
using namespace std;
typedef struct block_info
{
    int64_t max_key;
    long fpos;
} block_info;

class DFile
{
public:
    BPTree *tree;
    fstream fp;
    bool is_sync; // for PUT
    int fileCode;
    string filename;
    const int64_t NODE_MAX_NUM; // Mem assume 4GB
    const int ENTRY_PER_BLK;    // 4KB
    // cache
    vector<block_info> cache;
    void openfile(ios_base::openmode mode);
    DFile(int fileCode,
          int64_t node_max_num,
          int entry);
    ~DFile();
    BPTree *split();
    void set_tree(BPTree *new_tree);
    BPTree *insert(int64_t x, string y,
                   bool first = false);
    void search(int64_t x);
    void dump(ostream &cso = cerr);
    bool bulk_load();
    void cache_locate();
    long cache_beg;
    long cache_end_pos;
    void cache_read();
    void cache_search(int64_t x, ostream &cso = cerr); // for GET
    void cache_show();
    void rename(int new_fileCode);
    void file_show();

private:
};

class LRUCache;

class DB
{
public:
    const int64_t BLK_PER_FILE;
    const int ENTRY_PER_BLK;
    vector<DFile *> DFile_admin;
    LRUCache *lru_mgmt;
    LRUCache *lru_mgmt_cache;
    DB(int64_t blocks,
       int entry);
    ~DB();
    void DBinit();
    vector<DFile *>::iterator find_file(int64_t key);
    void put(int64_t key, string str);
    void get(int64_t key);
    void scan(int64_t k1, int64_t k2);
    void DBsave();
    void DBstatus();
    DFile *create_new_file(int fileCode);

private:
    vector<DFile *> files;
};

class LRUCache
{
    // store keys of cache
    list<DFile *> dq;

    // store references of key in cache
    unordered_map<DFile *, list<DFile *>::iterator> ma;
    int csize; // maximum capacity of cache

public:
    LRUCache(int n);
    void refer(DFile *);
    void refer_cache(DFile *);
    void display();
};

#endif // DB_H