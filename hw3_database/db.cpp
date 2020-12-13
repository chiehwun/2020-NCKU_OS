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
    if (fp.is_open())
        fp.close();
}

void DFile::openfile(ios_base::openmode mode)
{
    if (fp.is_open())
        fp.close();
    fp.open(filename, mode | ios::binary);
    if (!fp.is_open())
    {
        cerr << "DFile::openfile(): file pointer error.\n";
        exit(EXIT_FAILURE);
    }
}

void DFile::set_tree(BPTree *new_tree)
{
    if (!new_tree)
        return;
    delete tree;
    tree = new_tree;
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
    // Deallocate old parent nodes
    tree->del_tree(tree->getRoot(), false);
    tree->bulk_load_parents(qu);
    new_tree->bulk_load_parents(qu_new);
    return new_tree;
}

// Return new_tree if overflow
BPTree *DFile::insert(int64_t x, string y, bool first)
{
    BPTree *new_tree = NULL;
    // Check tree in memory
    if (!tree->getRoot() && !first)
        bulk_load();
    if (tree->insert(x, y, cerr))
        // overflow
        new_tree = split();
    is_sync = false;
    return new_tree;
}

// tree search or cache search
void DFile::search(int64_t x)
{
    // Tree in memory
    if (tree->getRoot())
        tree->search(x, false);
    else
        cache_search(x);
}

void DFile::dump(ostream &cso)
{
    /*********** DFile Structure ***********/
    /* -------------- Content --------------*/
    /* ============ key,value ============ */
    /* ===== max. key for each block ===== */
    /* --------------- Cache ---------------*/
    /* ========== cache_beg_pos ========== */
    /* ========= min key for file ========= */
    /* ========= max key for file ========= */
    cerr << "\n********** dump(): " << filename << " **********\n";
    openfile(ios::out);
    Node *cursor = tree->getRoot();
    while (!cursor->IS_LEAF)
        cursor = cursor->ptr[0];
    // queue<int64_t *> max_key_in_block;
    int64_t entry_num = 0;
    int i = 0;
    cache.clear();
    // Write key-value content and
    // collect max_key in discrete entries per MAX entries
    while (true)
    {
        // i += cursor->size;
        fp.write((char *)cursor->datum,
                 sizeof(Data) * cursor->size);
        entry_num += cursor->size;
        // if block full
        if (i + cursor->size >= tree->MAX)
        {
            //@@
            block_info b = {.max_key = cursor->datum[tree->MAX - i - 1].key,
                            .fpos = (int)cache.size() * ENTRY_PER_BLK * (int64_t)sizeof(Data)};
            cache.push_back(b);

            // max_key_in_block.push(
            //     &cursor->datum[tree->MAX - i - 1].key);
            i += cursor->size - tree->MAX;
        }
        else
            i += cursor->size;
        if (cursor->ptr[cursor->size])
            cursor = cursor->ptr[cursor->size];
        else
        {
            // last incomplete block
            if (i > 0)
            {
                block_info b = {.max_key = cursor->datum[cursor->size - 1].key,
                                .fpos = i * (int64_t)sizeof(Data)};
                cache.push_back(b);

                // max_key_in_block.push(&cursor->datum[cursor->size - 1].key);
            }
            break;
        }
    }
    // Write Cache
    // while (!max_key_in_block.empty())
    for (auto it = cache.begin(); it < cache.end(); ++it)
    {
        fp.write((char *)&(*it).max_key, sizeof(Data::key));
        // cerr << "max_key: " << *max_key_in_block.front() << endl;
        // max_key_in_block.pop();
    }
    cerr << "entry_num: " << entry_num << endl;
    long file_info_pos = entry_num * sizeof(Data);
    // cerr << "file_info_pos = " << file_info_pos << endl;
    fp.write((char *)&file_info_pos, sizeof(file_info_pos));
    fp.write((char *)&tree->min_key, sizeof(tree->max_key));
    fp.write((char *)&tree->max_key, sizeof(tree->min_key));
    openfile(ios::in);
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
    cache_locate();
    // Read blocks with HALF size from file to queue
    // and construct leaf nodes
    queue<Node *> qu;
    int64_t entry_num = cache_beg / sizeof(Data);
    fp.seekg(0, ios::beg);
    while (entry_num > 0)
    {
        // cerr << "fp.tellg() : " << fp.tellg() << ", cache_beg: " << cache_beg << endl;
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

// Read min_key, max_key, cache_beg of file
// cache_locate() => cache_read()
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

// Read max key for each block
void DFile::cache_read()
{
    cerr << "\n********** cache_read: " << filename << " **********\n";
    cache.clear();
    cache_locate();
    // Read max key for each block
    fp.seekg(cache_beg, ios::beg);
    for (int64_t i = 0; fp.tellg() < cache_end_pos; ++i)
    {
        block_info b = {.max_key = 0,
                        .fpos = i * ENTRY_PER_BLK * (int64_t)sizeof(Data)};
        fp.read((char *)&b.max_key, sizeof(Data::key));
        cache.push_back(b);
        // cerr << "max_key: " << cache.back().max_key << ", "
        //      << cache.back().fpos << endl;
    }
    cerr << cache.size() << " block"
         << (cache.size() > 1 ? "s has" : " have")
         << " been read.\n";
}

// for GET search in file with cache
void DFile::cache_search(int64_t x, ostream &cso)
{
    if (cache.empty())
    {
        cerr << "DFile::cache_search: cache is empty.\n";
        exit(EXIT_FAILURE);
    }
    // Boundary safe: checked by file_locate
    auto it_cache = cache.begin();
    for (; x > it_cache->max_key && it_cache < cache.end(); ++it_cache)
        ;
    // cerr << "tree->min_key: " << tree->min_key << endl;
    if (it_cache == cache.end() || x < tree->min_key)
    {
        cso << "DFile::cache_search(" << x << "): Not Found.\n";
        return;
    }
    cso << "DFile::cache_search(" << x << "): "
        << it_cache->max_key << ", " << it_cache->fpos << endl;
    if (tree->root)
        tree->reset_tree();
    // load the block
    tree->root = new Node(ENTRY_PER_BLK, true);
    ++tree->NodeNum;
    tree->root->set_sizeMAX(); // remind!!
    openfile(ios::in);
    fp.seekg(it_cache->fpos, ios::beg);
    fp.read((char *)tree->root->datum, sizeof(Data) * ENTRY_PER_BLK);
    // tree->visualize(6, cerr, true);
    tree->search(x);
    // Reset root to NULL
    tree->reset_tree();
}

void DFile::cache_show()
{
    cerr << "\n********** cache_show: " << filename << " **********\n";
    for (auto cache_it = cache.begin();
         cache_it < cache.end(); ++cache_it)
    {
        cerr << "max_key: " << (*cache_it).max_key << ", "
             << (*cache_it).fpos << endl;
    }
    cerr << cache.size() << " block"
         << (cache.size() > 1 ? "s" : "") << ".\n";
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

void DFile::file_show()
{
    openfile(ios::in);
    fp.seekg(0, ios::beg);
    Node node(ENTRY_PER_BLK, true);
    node.set_sizeMAX();
    fp.read((char *)node.datum, sizeof(Data) * tree->MAX);
    node.display();
}

// BLK_PER_FILE, ENTRY_PER_BLK
DB::DB(int64_t blocks, int entry) : BLK_PER_FILE(blocks),
                                    ENTRY_PER_BLK(entry)
{
    lru_mgmt = new LRUCache(4);
    lru_mgmt_cache = new LRUCache(4);
}

DB::~DB()
{
    for (auto it = DFile_admin.begin(); it < DFile_admin.end(); it++)
        delete *it;
    delete lru_mgmt;
    delete lru_mgmt_cache;
}

void DB::DBinit()
{
    cerr << "\n********** DBinit() **********\n";
    if (!DFile_admin.empty())
    {
        cerr << "DB::DBinit(): Invalid operation.\n";
        return;
    }
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
        infs.close();
    }
    cerr << filecode << " file" << (filecode > 1 ? "s" : "") << " detected.\n";
    // Read all cache for all files
    for (auto it = DFile_admin.begin(); it < DFile_admin.end(); ++it)
        (*it)->cache_read();
    cerr << "\n********** end of DBinit() **********\n";
}

DFile *DB::create_new_file(int fileCode)
{
    DFile *new_file = NULL;
    if (fileCode < 0)
    {
        cerr << "DB::create_new_file: fileCode = " << fileCode << " < 0 !!\n";
        exit(EXIT_FAILURE);
    }
    else if (fileCode >= (int)DFile_admin.size() - 1)
    {
        cerr << "push back " << DFile_admin.size() << ".file\n";
        new_file = new DFile(DFile_admin.size(),
                             BLK_PER_FILE,
                             ENTRY_PER_BLK);
        DFile_admin.push_back(new_file);
    }
    else if (fileCode < DFile_admin.size())
    {
        cerr << "insert " << fileCode << ".file\n";
        for (int i = DFile_admin.size() - 1; i > fileCode; --i)
            DFile_admin.at(i)->rename(i + 1);
        new_file = new DFile(fileCode,
                             BLK_PER_FILE,
                             ENTRY_PER_BLK);
        DFile_admin.insert(DFile_admin.begin() + fileCode + 1,
                           new_file);
    }
    return new_file;
}

vector<DFile *>::iterator DB::find_file(int64_t key)
{
    // loacte file, 3 cases
    // front(), back(), middle
    auto it = DFile_admin.begin();
    if (key < DFile_admin.front()->tree->min_key)
        it = DFile_admin.begin();
    else if (key > DFile_admin.back()->tree->max_key)
        it = prev(DFile_admin.end());
    else
        for (; it < DFile_admin.end(); ++it)
            if (key >= (*it)->tree->min_key &&
                key <= (*it)->tree->max_key)
                break;
    return it;
}

void DB::put(int64_t key, string str)
{
    bool first_put = DFile_admin.empty();
    // Brand new database
    if (first_put)
        create_new_file(0);
    auto it = find_file(key);
    BPTree *new_tree = NULL;
    new_tree = (*it)->insert(key, str, first_put);
    int IT = (*it)->fileCode;
    if (new_tree)
    {
        // insertion overflow
        DFile *new_df = create_new_file(it - DFile_admin.begin());
        new_df->set_tree(new_tree);
        lru_mgmt->refer(new_df);
        cerr << "insertion overflow\n";
        lru_mgmt->display();
    }
    lru_mgmt->refer(DFile_admin.at(IT));
    // if (new_tree)
    // {
    // it has change
    //     cerr << "it: " << it - DFile_admin.begin() << endl;
    //     lru_mgmt->display();
    // }
}

void DB::get(int64_t key)
{
    if (DFile_admin.empty())
    {
#ifdef DB_DB
        cerr << "DB::get(" << key << "): File no found." << endl;
#endif
        return;
    }

    auto it = find_file(key);
#ifdef DB_DB
    cerr << "DB::get(" << key << "): " << (*it)->filename << endl;
#endif
    (*it)->search(key);
}

void DB::scan(int64_t k1, int64_t k2)
{
    // if (k1 > k2)
    // {
    //     cerr << "DB::scan(): k1 > k2\n";
    //     swap(k1, k2);
    // }
    for (int64_t i = k1; i <= k2; ++i)
    {
        get(i);
    }
}

void DB::DBsave()
{
    for (auto it = DFile_admin.begin();
         it < DFile_admin.end(); ++it)
    {
        if (!(*it)->is_sync && (*it)->tree->getRoot())
            (*it)->dump();
    }
}

void DB::DBstatus()
{
    for (auto it = DFile_admin.begin();
         it < DFile_admin.end(); ++it)
    {
        cerr << (*it)->fileCode << ", "
             << ((*it)->tree->NodeNum * (sizeof(Data) * (*it)->ENTRY_PER_BLK) >> 20)
             << " MB.\n";
    }
}

// Declare the size
LRUCache::LRUCache(int n)
{
    csize = n;
}

// Refers key x with in the LRU cache
void LRUCache::refer(DFile *x)
{
    // not present in cache
    if (ma.find(x) == ma.end())
    {
        // cache is full
        if (dq.size() == csize)
        {
            // delete least recently used element
            DFile *last = dq.back();

            /***** Save memory strategy *****/
            cerr << "Swap to " << last->filename << endl;
            // Dump tree to disk
            last->dump();
            // Deallocate memory
            last->tree->reset_tree();
            last->is_sync = false;
            last->fp.close();
            /***** Save memory strategy *****/

            // Pops the last elment
            dq.pop_back();

            // Erase the last
            ma.erase(last);
        }
    }

    // present in cache
    else
        dq.erase(ma[x]);

    // update reference
    dq.push_front(x);
    ma[x] = dq.begin();
}

void LRUCache::refer_cache(DFile *x)
{
    // not present in cache
    if (ma.find(x) == ma.end())
    {
        // cache is full
        if (dq.size() == csize)
        {
            // delete least recently used element
            DFile *last = dq.back();

            /***** Save memory strategy *****/
            cerr << "Delete cache for " << last->filename << endl;
            // Delete cache
            last->cache.clear();
            // Deallocate memory
            last->is_sync = false;
            last->fp.close();
            /***** Save memory strategy *****/

            // Pops the last elment
            dq.pop_back();

            // Erase the last
            ma.erase(last);
        }
    }

    // present in cache
    else
        dq.erase(ma[x]);

    // update reference
    dq.push_front(x);
    ma[x] = dq.begin();
}

// Function to display contents of cache
void LRUCache::display()
{
    // Iterate in the deque and print
    // all the elements in it
    for (auto it = dq.begin();
         it != dq.end(); it++)
        cerr << (*it)->fileCode << " ";
    cerr << endl;
}