#ifndef DB_H
#define DB_H
#include "BPTree.h"
#include <fstream>
#include <string>
#include <vector>
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
    BPTree *insert(int64_t x, string y);
    void set_tree(BPTree *new_tree);
    void search(int64_t x);
    void dump(ostream &cso = cout);
    bool bulk_load();
    void cache_locate();
    long cache_beg;
    long cache_end_pos;
    void cache_read();
    void cache_search(int64_t x, ostream &cso = cout); // for GET
    void rename(int new_fileCode);

private:
};

class DB
{
public:
    const int64_t BLK_PER_FILE;
    const int ENTRY_PER_BLK;
    DB(int64_t blocks,
       int entry);
    ~DB();
    void DBinit();
    void put(int64_t key, string str);
    void get(int64_t key);
    void scan(int64_t k1, int64_t k2);
    void read();
    vector<DFile *> DFile_admin;

private:
    void create_new_file(int fileCode);
    vector<DFile *> files;
};
#endif // DB_H