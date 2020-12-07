#include <iostream>
#include <fstream>
#include <string>
using namespace std;
typedef struct data
{
    int64_t key; // 0-INT64_MAX
    char str[129];
} data;

typedef struct block
{
    data ind[1000];
} block;

class DB
{
public:
    void put(int64_t key, string str);
    void get(int64_t key);
    void scan(int64_t k1, int64_t k2);

private:
    void init();
};