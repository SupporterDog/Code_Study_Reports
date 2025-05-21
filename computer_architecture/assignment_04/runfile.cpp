#include <sstream>
#include <bitset>
#include <random>
#include <bitset>
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

unsigned long totalOperations = 0;
unsigned long readOperations = 0;
unsigned long writeOperations = 0;

unsigned long long traceID = 0;

vector<string> tokenize(const string& str, char delimiter) {
    vector<string> tokens;
    istringstream iss(str);
    string token;
    while (getline(iss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

bool containsChar(const string& str, char c) {
    return str.find(c) != string::npos;
}

bool isNumber(const string& str, bool isHex = false) {
    for (char ch : str) {
        if (!isdigit(ch) && (!isHex || (ch != 'x' && !isxdigit(ch)))) {
            return false;
        }
    }
    return true;
}

class CacheBlock {
public:
    unsigned int dirtyFlag = 0;
    unsigned int validFlag = 0;
    string startAddr = "";
    unsigned long lastAccessTime = 0;
    bool isEmpty = false;
    CacheBlock(const string& startAddr, unsigned long timestep) {
        this->startAddr = startAddr;
        validFlag = 1;
        lastAccessTime = timestep;
    }
    CacheBlock() {
        startAddr = "000000000000000000000000000000000000000000000000";
        validFlag = 0;
        dirtyFlag = 2;
        isEmpty = true;
    }
};

class MemoryCache {
public:
    unsigned int readHits = 0;
    unsigned int readMisses = 0;
    unsigned int writeHits = 0;
    unsigned int writeMisses = 0;
    unsigned int cleanEvicts = 0;
    unsigned int dirtyEvicts = 0;
    unsigned int blockSize = 0;
    unsigned int associativity = 1;
    unsigned int cacheSize = 0;
    unsigned int numBlocks = 0;
    unsigned int numSets = 0;
    bool useLRU = true;
    vector<vector<CacheBlock*>> cacheSets;

    CacheBlock* emptyBlockPtr;

    unsigned int powerOf2(unsigned int num) {
        return num > 0 ? 31 - __builtin_clz(num) : 0;
    }
    vector<unsigned long> parseTagIndex(const string& memaddr) {
        unsigned long tag = stoul(memaddr.substr(0, memaddr.length() - powerOf2(numSets)), nullptr, 2);
        unsigned long index = stoul(memaddr.substr(memaddr.length() - powerOf2(numSets), powerOf2(numSets)), nullptr, 2);
        return { tag, index };
    }
    vector<unsigned long> parseRealMemTagIndex(const string& memaddr) {
        unsigned long tempMemAddr = stoul(memaddr, nullptr, containsChar(memaddr, 'x') ? 16 : 2);
        bitset<48> memAddr(tempMemAddr);
        string memAddrBitString = memAddr.to_string();
        unsigned long tag = stoul(memAddrBitString.substr(0, 48 - powerOf2(numSets) - powerOf2(blockSize)), nullptr, 2);
        unsigned long index = stoul(memAddrBitString.substr(48 - powerOf2(numSets) - powerOf2(blockSize), powerOf2(numSets)), nullptr, 2);
        return { tag, index };
    }
    unsigned int getStatistic(const string& var) {
        if (var == "readHits") return readHits;
        if (var == "readMisses") return readMisses;
        if (var == "writeHits") return writeHits;
        if (var == "writeMisses") return writeMisses;
        if (var == "cleanEvicts") return cleanEvicts;
        if (var == "dirtyEvicts") return dirtyEvicts;
        if (var == "blockSize") return blockSize;
        if (var == "associativity") return associativity;
        if (var == "cacheSize") return cacheSize;
        if (var == "numBlocks") return numBlocks;
        if (var == "numSets") return numSets;
        return 0;
    }
    virtual void initializePointer(MemoryCache* cache) = 0;
    CacheBlock* handleAccess(const vector<string>& meminfo);
    virtual CacheBlock* handleMiss(const vector<string>& meminfo) = 0;
    virtual void insertBlock(CacheBlock* block) = 0;
    virtual void evictBlock(const vector<unsigned long>& tagindex, const string& start_addr) = 0;
};

class L1MemoryCache : public MemoryCache {
private:
    MemoryCache* L2;
public:
    L1MemoryCache(unsigned int capacity, unsigned int asso, unsigned int blockSize, unsigned int isLRU, CacheBlock* emptyblock);
    ~L1MemoryCache();
    void initializePointer(MemoryCache* cache);
    CacheBlock* handleMiss(const vector<string>& meminfo);
    void insertBlock(CacheBlock* block);
    void evictBlock(const vector<unsigned long>& tagindex, const string& start_addr);
};

class L2MemoryCache : public MemoryCache {
private:
    MemoryCache* L1;
public:
    L2MemoryCache(unsigned int capacity, unsigned int asso, unsigned int blockSize, unsigned int isLRU, CacheBlock* emptyblock);
    ~L2MemoryCache();
    void initializePointer(MemoryCache* cache);
    CacheBlock* handleMiss(const vector<string>& meminfo);
    void insertBlock(CacheBlock* block);
    void evictBlock(const vector<unsigned long>& tagindex, const string& start_addr);
};

CacheBlock* MemoryCache::handleAccess(const vector<string>& meminfo) {
    vector<unsigned long> memTagIndex = parseRealMemTagIndex(meminfo[1]);
    bool isHit = false;
    CacheBlock* accessBlock = nullptr;

    for (auto& block : cacheSets[memTagIndex[1]]) {
        if (parseTagIndex(block->startAddr)[0] == memTagIndex[0]) {
            isHit = true;
            accessBlock = block;
            break;
        }
    }
    if (isHit) {
        accessBlock->lastAccessTime = totalOperations;
        if (meminfo[0] == "R") {
            readHits++;
        }
        else {
            accessBlock->dirtyFlag = 1;
            writeHits++;
        }
    }
    else {
        accessBlock = handleMiss(meminfo);
        insertBlock(accessBlock);
        accessBlock->lastAccessTime = totalOperations;
        if (meminfo[0] == "R") {
            readMisses++;
        }
        else if (meminfo[0] == "W") {
            accessBlock->dirtyFlag = 1;
            writeMisses++;
        }
    }
    return new CacheBlock(*accessBlock);
}

L1MemoryCache::L1MemoryCache(unsigned int capacity, unsigned int asso, unsigned int blockSize, unsigned int isLRU, CacheBlock* emptyblock) {
    cacheSize = capacity;
    associativity = asso;
    this->blockSize = blockSize;
    useLRU = (isLRU != 2);

    emptyBlockPtr = emptyblock;

    numBlocks = (cacheSize * 1024) / blockSize;
    numSets = numBlocks / associativity;
    cacheSets.resize(numSets, vector<CacheBlock*>(associativity, emptyBlockPtr));
}

L1MemoryCache::~L1MemoryCache() {
    for (auto& set : cacheSets) {
        for (auto& block : set) {
            if (!block->isEmpty) delete block;
        }
    }
}

void L1MemoryCache::initializePointer(MemoryCache* cache) {
    L2 = cache;
}

CacheBlock* L1MemoryCache::handleMiss(const vector<string>& meminfo) {
    return L2->handleAccess(meminfo);
}

void L1MemoryCache::insertBlock(CacheBlock* block) {
    vector<unsigned long> blockTagIndex = parseTagIndex(block->startAddr);
    bool isInserted = false;
    for (auto& slot : cacheSets[blockTagIndex[1]]) {
        if (slot->isEmpty) {
            slot = block;
            isInserted = true;
            break;
        }
    }
    if (!isInserted) {
        evictBlock(blockTagIndex, block->startAddr);
        insertBlock(block);
    }
}

void L1MemoryCache::evictBlock(const vector<unsigned long>& tagindex, const string& start_addr) {
    long newLoc = -1;
    bool isBlockExist = false;

    vector<unsigned long> tempTag = parseTagIndex(start_addr);
    for (unsigned int i = 0; i < associativity; i++) {
        if (parseTagIndex(cacheSets[tempTag[1]][i]->startAddr)[0] == tempTag[0]) {
            isBlockExist = true;
            newLoc = i;
            break;
        }
    }
    if (!isBlockExist) {
        if (useLRU) {
            map<unsigned long, unsigned long> tempLRU;
            for (unsigned long long i = 0; i < cacheSets[tagindex[1]].size(); i++) {
                tempLRU[cacheSets[tagindex[1]][i]->lastAccessTime] = i;
            }
            newLoc = tempLRU.begin()->second;
        }
        else {
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<int> dis(0, cacheSets[tagindex[1]].size() - 1);
            newLoc = dis(gen);
        }
    }

    if (newLoc != -1) {
        CacheBlock* evictBlock = cacheSets[tempTag[1]][newLoc];
        if (evictBlock->dirtyFlag == 0) {
            cleanEvicts++;
        }
        else if (evictBlock->dirtyFlag == 1) {
            dirtyEvicts++;
            unsigned long evictBlockRealaddr = stoul(evictBlock->startAddr, nullptr, 2) << powerOf2(blockSize);
            bitset<48> evictBlockRealaddr_bitset(evictBlockRealaddr);
            vector<string> tempMeminfo = { "W", evictBlockRealaddr_bitset.to_string() };
            CacheBlock* dummy = L2->handleAccess(tempMeminfo);
        }
        if (!evictBlock->isEmpty) {
            delete evictBlock;
        }
        cacheSets[tempTag[1]][newLoc] = emptyBlockPtr;
    }
}

L2MemoryCache::L2MemoryCache(unsigned int capacity, unsigned int asso, unsigned int blockSize, unsigned int isLRU, CacheBlock* emptyblock) {
    cacheSize = capacity;
    associativity = asso;
    this->blockSize = blockSize;
    useLRU = (isLRU != 2);

    emptyBlockPtr = emptyblock;

    numBlocks = (cacheSize * 1024) / blockSize;
    numSets = numBlocks / associativity;
    cacheSets.resize(numSets, vector<CacheBlock*>(associativity, emptyBlockPtr));
}

L2MemoryCache::~L2MemoryCache() {
    for (auto& set : cacheSets) {
        for (auto& block : set) {
            if (!block->isEmpty) delete block;
        }
    }
}

void L2MemoryCache::initializePointer(MemoryCache* cache) {
    L1 = cache;
}

CacheBlock* L2MemoryCache::handleMiss(const vector<string>& meminfo) {
    unsigned long tempMemaddr = stoul(meminfo[1], nullptr, 16);
    bitset<48> tempMemaddr_bitset(tempMemaddr);
    string tempMemaddr_string = tempMemaddr_bitset.to_string().substr(0, 48 - powerOf2(blockSize));
    CacheBlock* ret_block = new CacheBlock(tempMemaddr_string, totalOperations);
    return ret_block;
}

void L2MemoryCache::insertBlock(CacheBlock* block) {
    vector<unsigned long> blockTagIndex = parseTagIndex(block->startAddr);
    bool isInserted = false;
    for (auto& slot : cacheSets[blockTagIndex[1]]) {
        if (slot->isEmpty) {
            slot = block;
            isInserted = true;
            break;
        }
    }
    if (!isInserted) {
        evictBlock(blockTagIndex, block->startAddr);
        insertBlock(block);
    }
}

void L2MemoryCache::evictBlock(const vector<unsigned long>& tagindex, const string& start_addr) {
    unsigned long newLoc = 0;
    if (useLRU) {
        map<unsigned long, unsigned long> tempLRU;
        for (unsigned long i = 0; i < cacheSets[tagindex[1]].size(); i++) {
            tempLRU[cacheSets[tagindex[1]][i]->lastAccessTime] = i;
        }
        newLoc = tempLRU.begin()->second;
    }
    else {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dis(0, cacheSets[tagindex[1]].size() - 1);
        newLoc = dis(gen);
    }

    CacheBlock* evictBlock = cacheSets[tagindex[1]][newLoc];
    vector<unsigned long> temp_tagindex = tagindex;
    temp_tagindex.push_back(1);
    L1->evictBlock(temp_tagindex, evictBlock->startAddr);

    if (evictBlock->dirtyFlag == 0) {
        cleanEvicts++;
    }
    else if (evictBlock->dirtyFlag != 0) {
        dirtyEvicts++;
    }
    if (!evictBlock->isEmpty) {
        delete evictBlock;
    }
    cacheSets[tagindex[1]][newLoc] = emptyBlockPtr;
}

class MainException { };
class ArgCapacityException : public MainException { };
class ArgAssociativityException : public MainException { };
class ArgBlockSizeException : public MainException { };
class LruInvalidException : public MainException { };

int main(int argc, char** argv) {
    unsigned int CAPACITY = 0;
    unsigned int ASSOCIATIVITY = 0;
    unsigned int BLOCK_SIZE = 0;
    unsigned int IS_LRU = 0;
    string TRACEFILE = "";

    if (argc > 9) {
        cerr << "Error : too many argument" << endl;
        return 1;
    }

    try {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-c") == 0) {
                if (!isNumber(argv[++i]))
                    throw ArgCapacityException();
                CAPACITY = stoul(argv[i], nullptr, 0);
            }
            else if (strcmp(argv[i], "-a") == 0) {
                if (!isNumber(argv[++i]))
                    throw ArgAssociativityException();
                ASSOCIATIVITY = stoul(argv[i], nullptr, 0);
            }
            else if (strcmp(argv[i], "-b") == 0) {
                if (!isNumber(argv[++i]))
                    throw ArgBlockSizeException();
                BLOCK_SIZE = stoul(argv[i], nullptr, 0);
            }
            else if (strcmp(argv[i], "-lru") == 0) {
                IS_LRU = 1;
            }
            else if (strcmp(argv[i], "-random") == 0) {
                IS_LRU = 2;
            }
            else {
                TRACEFILE = argv[i];
            }
        }
        if (IS_LRU == 0) throw LruInvalidException();
    }
    catch (ArgCapacityException) {
        cerr << "Error : invalid capacity" << endl;
        return 1;
    }
    catch (ArgAssociativityException) {
        cerr << "Error : invalid associativity" << endl;
        return 1;
    }
    catch (ArgBlockSizeException) {
        cerr << "Error : invalid block size" << endl;
        return 1;
    }
    catch (LruInvalidException) {
        cerr << "Error : no -lru option" << endl;
        return 1;
    }

    ifstream inputFile(TRACEFILE, ios::binary);
    if (!inputFile) {
        cerr << "Error : can't open object file" << endl;
        return 1;
    }

    unsigned int l2_associativity = ASSOCIATIVITY;
    unsigned int l1_associativity = l2_associativity / 4;
    unsigned int l2_capacity = CAPACITY;
    unsigned int l1_capacity = l2_capacity / 4;
    if (ASSOCIATIVITY <= 2) l1_associativity = l2_associativity;

    CacheBlock emptyBlock;
    CacheBlock* emptyBlockPointer = &emptyBlock;

    MemoryCache* l1 = new L1MemoryCache(l1_capacity, l1_associativity, BLOCK_SIZE, IS_LRU, emptyBlockPointer);
    MemoryCache* l2 = new L2MemoryCache(l2_capacity, l2_associativity, BLOCK_SIZE, IS_LRU, emptyBlockPointer);

    l1->initializePointer(l2);
    l2->initializePointer(l1);

    string temp;
    while (getline(inputFile, temp)) {
        traceID++;

        vector<string> memInfo = tokenize(temp, ' ');

        totalOperations++;
        if (memInfo[0] == "R")
            readOperations++;
        else if (memInfo[0] == "W")
            writeOperations++;

        CacheBlock* dummy = l1->handleAccess(memInfo);
        delete dummy;
    }
    inputFile.close();

    fstream outputFile;
    string output_filename = TRACEFILE.substr(0, TRACEFILE.length() - 4) + "_" + to_string(CAPACITY) + "_" + to_string(ASSOCIATIVITY) + "_" + to_string(BLOCK_SIZE) + ".out";
    outputFile.open(output_filename, ios::out);

    if (outputFile.is_open()) {
        outputFile << "-- General Stats --" << endl;
        outputFile << "L1 Capacity: " << l1->cacheSize << endl;
        outputFile << "L1 way: " << l1->associativity << endl;
        outputFile << "L2 Capacity: " << CAPACITY << endl;
        outputFile << "L2 way: " << l2->associativity << endl;
        outputFile << "Block Size: " << BLOCK_SIZE << endl;
        outputFile << "Total accesses: " << totalOperations << endl;
        outputFile << "Read accesses: " << readOperations << endl;
        outputFile << "Write accesses: " << writeOperations << endl;
        outputFile << "L1 Read misses: " << l1->readMisses << endl;
        outputFile << "L1 Write misses: " << l1->writeMisses << endl;
        outputFile << "L2 Read misses: " << l2->readMisses << endl;
        outputFile << "L2 Write misses: " << l2->writeMisses << endl;
        outputFile << "L1 Read miss rate: " << (static_cast<double>(l1->readMisses) / readOperations) * 100 << "%" << endl;
        outputFile << "L2 Read miss rate: " << (static_cast<double>(l2->readMisses) / l1->readMisses) * 100 << "%" << endl;
        outputFile << "L1 Write miss rate: " << (static_cast<double>(l1->writeMisses) / writeOperations) * 100 << "%" << endl;
        outputFile << "L2 Write miss rate: " << (static_cast<double>(l2->writeMisses) / l1->writeMisses) * 100 << "%" << endl;
        outputFile << "L1 Clean eviction: " << l1->cleanEvicts << endl;
        outputFile << "L2 Clean eviction: " << l2->cleanEvicts << endl;
        outputFile << "L1 dirty eviction: " << l1->dirtyEvicts << endl;
        outputFile << "L2 dirty eviction: " << l2->dirtyEvicts << endl;

    }

    outputFile.close();

    delete l1;
    delete l2;

    return 0;
}
