#include "db.h"
// fileCode, NODE_MAX_NUM, ENTRY_PER_BLK
DFile::DFile(int fileCode,
             int64_t node_max_num,
             int entry) : fileCode(fileCode),
                          NODE_MAX_NUM(node_max_num),
                          ENTRY_PER_BLK(entry)
{
    filename = "./storage/" + to_string(fileCode) + ".file";
    tree = new BPTree(ENTRY_PER_BLK,
                      NODE_MAX_NUM);
}

DFile::~DFile()
{
    fp.close();
}

void DFile::openfile(ios_base::openmode mode)
{
    fp.open(filename, mode | ios::binary);
    if (!fp)
    {
        cerr << "DFile::openfile(): file pointer error.\n";
        return;
    }
}

BPTree *DFile::split()
{
    BPTree *new_tree = new BPTree(ENTRY_PER_BLK,
                                  NODE_MAX_NUM);
    queue<Node *> qu, qu_new;
    Node *cursor = tree->getRoot();
    while (!cursor->IS_LEAF)
        cursor = cursor->ptr[0];
    while (cursor)
    {
        if (qu.size() < tree->leafNodeNum / 2)
            qu.push(cursor);
        else
            qu_new.push(cursor);
        cursor = cursor->ptr[cursor->size];
    }
    // split the link
    qu.back()->ptr[qu.back()->size] = NULL;
    // Deallocate parent nodes
    tree->del_tree(tree->getRoot(), false);
    tree->bulk_load_parents(qu);
    new_tree->bulk_load_parents(qu_new);
    return new_tree;
}

// Return new_tree if overflow
BPTree *DFile::insert(int64_t x, string y)
{
    BPTree *new_tree = NULL;
    if (!tree->getRoot())
    {
        bulk_load();
    }
    if (tree->insert(x, y, cerr))
    {
        // overflow
        new_tree = split();
    }
    if (is_sync)
        is_sync = false;
    return new_tree;
}

void DFile::search(int64_t x)
{
    // Tree in memory
    if (tree->getRoot())
    {
        tree->search(x);
    }
    else
    {
        cache_search(x);
    }
}

void DFile::dump(ostream &cso)
{
    openfile(ios::out);
    Node *cursor = tree->getRoot();
    while (!cursor->IS_LEAF)
        cursor = cursor->ptr[0];
    queue<int64_t *> max_key_in_block;
    int64_t i = 0, entry_num = 0;
    while (true)
    {
        // i += cursor->size;
        fp.write((char *)cursor->datum,
                 sizeof(Data) * cursor->size);
        entry_num += cursor->size;
        // if block full
        if (i + cursor->size >= tree->MAX)
        {
            max_key_in_block.push(
                &cursor->datum[tree->MAX - i - 1].key);
            i += cursor->size - tree->MAX;
        }
        else
        {
            i += cursor->size;
        }
        if (cursor->ptr[cursor->size])
            cursor = cursor->ptr[cursor->size];
        else
        {
            // last incomplete block
            if (i > 0)
                max_key_in_block.push(&cursor->datum[cursor->size - 1].key);
            break;
        }

        /*********** DFile Structure ***********/
        /* -------------- Content --------------*/
        /* ============ key,value ============ */
        /* ===== max. key for each block ===== */
        /* --------------- Cache ---------------*/
        /* ========== cache_beg_pos ========== */
        /* ========= min key for file ========= */
        /* ========= max key for file ========= */
    }

    // Cache
    while (!max_key_in_block.empty())
    {
        cerr << "max_key: " << *max_key_in_block.front() << endl;
        fp.write((char *)max_key_in_block.front(), sizeof(Data::key));
        max_key_in_block.pop();
    }
    cerr << "entry_num: " << entry_num << endl;
    long file_info_pos = entry_num * sizeof(Data);
    cerr << "file_info_pos = " << file_info_pos << endl;
    fp.write((char *)&file_info_pos, sizeof(file_info_pos));
    fp.write((char *)&tree->min_key, sizeof(tree->max_key));
    fp.write((char *)&tree->max_key, sizeof(tree->min_key));
    fp.close();
}

// Load data from file
bool DFile::bulk_load()
{
    // if (tree->getRoot())
    // {
    //     cerr << "DFile::bulk_load(): tree is in mem. Node_num = " << tree->NodeNum << endl;
    //     return false;
    // }
    is_sync = true;
    cache_read();
    openfile(ios::in);
    // Read blocks from file to queue
    // and construct leaf nodes
    queue<Node *> qu;
    int64_t i = 0;
    cerr << "cache_beg  : " << cache_beg << endl;
    int64_t entry_num = cache_beg / sizeof(Data);
    while (entry_num > 0)
    {
        cerr << "fp.tellg() : " << fp.tellg() << ", cache_beg: " << cache_beg;
        Node *leaf = new Node(ENTRY_PER_BLK, true);
        // Link leaf node
        if (!qu.empty())
            qu.back()->ptr[qu.back()->size] = leaf;
        // Fill leaf.key with HALF size
        if (entry_num >= tree->HALF)
        {
            fp.read((char *)leaf->datum, sizeof(Data) * tree->HALF);
            leaf->size = tree->HALF;
            entry_num -= tree->HALF;
        }
        else
        {
            fp.read((char *)leaf->datum, sizeof(Data) * entry_num);
            leaf->size = entry_num;
            entry_num = 0;
        }
        qu.push(leaf);
        // getc(stdin);
    }
    // Bulk load part 2: construct parent nodes
    tree->bulk_load_parents(qu);
    return true;
}

void DFile::cache_locate()
{
    openfile(ios::in);
    fp.seekg(-3 * sizeof(Data::key), ios::end);
    // record fp
    cache_end_pos = fp.tellg();

    fp.read((char *)&cache_beg, sizeof(cache_beg));
    fp.read((char *)&tree->min_key, sizeof(Data::key));
    fp.read((char *)&tree->max_key, sizeof(Data::key));
    // cerr << "cache_beg: " << cache_beg << endl;
    // cerr << "min_key  : " << tree->min_key << endl;
    // cerr << "max_key  : " << tree->max_key << endl;
}

void DFile::cache_read()
{
    cache.clear();
    cerr << "********** cache_read: " << filename << " **********\n";
    cache_locate();
    // Read max key for each block
    fp.seekg(cache_beg, ios::beg);
    for (int64_t i = 0; fp.tellg() < cache_end_pos; ++i)
    {
        block_info b = {.max_key = 0,
                        .fpos = i * (int64_t)sizeof(Data)};
        fp.read((char *)&b.max_key, sizeof(Data::key));
        cache.push_back(b);
        // cerr << "max_key: " << cache.back().max_key << ", "
        //      << cache.back().fpos << endl;
    }
    cerr << cache.size() << " cache has been read.\n";
    fp.close();
}

// for GET
void DFile::cache_search(int64_t x, ostream &cso)
{
    if (cache.empty())
    {
        cerr << "DFile::cache_search: cache is empty.\n";
        exit(EXIT_FAILURE);
    }
    auto ptr = cache.begin();
    for (; x > ptr->max_key && ptr != cache.end(); ++ptr)
    {
    }
    cso << "DFile::cache_search(" << x << "): "
        << ptr->max_key << ", " << ptr->fpos << endl;
    if (tree->root)
    {
        tree->reset_tree();
    }
    // load the block
    tree->root = new Node(ENTRY_PER_BLK, true);
    ++tree->NodeNum;
    tree->root->set_sizeMAX(); // remind!!
    openfile(ios::in);
    fp.seekg(ptr->fpos, ios::beg);
    fp.read((char *)tree->root->datum, sizeof(Data) * ENTRY_PER_BLK);
    tree->search(x);
    tree->reset_tree();
    fp.close();
}

void DFile::rename(int new_fileCode)
{
    string new_filename = "./storage/" + to_string(new_fileCode) + ".file";
    if (std::rename(filename.c_str(), new_filename.c_str()))
    {
        cerr << "DFile::rename(): Error renaming"
             << filename << " -> " << new_filename;
        return;
    }
    fileCode = new_fileCode;
    filename = new_filename;
}

DB::DB(int64_t blocks, int entry) : BLK_PER_FILE(blocks),
                                    ENTRY_PER_BLK(entry)
{
}

DB::~DB()
{
    for (auto it = DFile_admin.begin(); it < DFile_admin.end(); it++)
        delete *it;
}

void DB::DBinit()
{
    DFile_admin.clear();
    string const filepath("./storage/");
    string const file_extension(".file");
    ifstream infs;
    int filecode = 0;
    while (true)
    {
        infs.open(filepath + to_string(filecode) + file_extension,
                  ios::in | ios::binary);
        if (!infs)
            break;
        create_new_file(filecode);
        ++filecode;
    }
    cout << filecode << " file" << (filecode > 1 ? "s" : "") << " detected.\n";
    for (auto it = DFile_admin.begin(); it < DFile_admin.end(); ++it)
        (*it)->cache_read();
}

void DB::create_new_file(int fileCode)
{
    if (fileCode < 0)
    {
        cerr << "DB::create_new_file: fileCode = " << fileCode << " < 0 !!\n";
        exit(EXIT_FAILURE);
    }
    else if (fileCode >= (int)DFile_admin.size() - 1)
    {
        cout << "push back " << DFile_admin.size() << ".file\n";
        DFile_admin.push_back(new DFile(DFile_admin.size(),
                                        BLK_PER_FILE,
                                        ENTRY_PER_BLK));
    }
    else if (fileCode < DFile_admin.size())
    {
        cout << "insert " << fileCode << ".file\n";
        for (int i = DFile_admin.size() - 1; i > fileCode; --i)
        {
            DFile_admin.at(i)->rename(i + 1);
        }
        DFile_admin.insert(DFile_admin.begin() + fileCode + 1,
                           new DFile(fileCode,
                                     BLK_PER_FILE,
                                     ENTRY_PER_BLK));
    }
}

void DB::put(int64_t key, string str)
{
    // Brand new database
    if (DFile_admin.empty())
        DFile_admin.push_back(new DFile(0,
                                        BLK_PER_FILE,
                                        ENTRY_PER_BLK));
    if (DFile_admin.size() == 1)
    {
        BPTree *new_tree = DFile_admin.front()->insert(key, str);
        if (new_tree)
        {
            // insertion overflow
            create_new_file(1);
            delete DFile_admin.back()->tree;
            DFile_admin.back()->tree = new_tree;
        }
    }
    else
    {

        for (auto it = DFile_admin.begin();
             it < DFile_admin.end(); ++it)
        {
            // brand new file
            if (DFile_admin.size() && !(*it)->tree)
            {
            }
        }
    }
}

void DB::get(int64_t key)
{
}

void DB::scan(int64_t k1, int64_t k2)
{
}