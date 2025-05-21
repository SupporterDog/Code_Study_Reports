#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <bitset>
#include <fstream>
#include <cstring>
#include <stdexcept>
using namespace std;

const unsigned int TEXT_START_ADDR = 0x400000;
const unsigned int DATA_START_ADDR = 0x10000000;

unsigned int PC;
map<unsigned int, int> Register;
map<unsigned int, string> Memory;


int MemoryRead(unsigned int address, bool isWord) {
    if (isWord) {
        if (Memory.find(address) != Memory.end() &&
            Memory.find(address + 1) != Memory.end() &&
            Memory.find(address + 2) != Memory.end() &&
            Memory.find(address + 3) != Memory.end()) {
            string word;
            for (int i = 0; i < 4; ++i) {
                word += Memory[address + i];
            }
            return stoul(word, nullptr, 2);
        }
        else {
            return 0;
        }
    }
    else {
        if (Memory.find(address) != Memory.end()) {
            return stoul(Memory[address], nullptr, 2);
        }
        else {
            return 0;
        }
    }
}

void MemoryWrite(unsigned int address, int value, bool isWord) {
    if (isWord) {
        bitset<32> word(value);
        string wordStr = word.to_string();
        for (int i = 0; i < 4; ++i) {
            Memory[address + i] = wordStr.substr(i * 8, 8);
        }
    }
    else {
        bitset<8> byte(value);
        Memory[address] = byte.to_string();
    }
}

void ExecuteRTypeInstruction(const string& rs, const string& rt, const string& rd, const string& shamt, const string& funct) {
    unsigned int rs_temp = stoul(rs, nullptr, 2);
    unsigned int rt_temp = stoul(rt, nullptr, 2);
    unsigned int rd_temp = stoul(rd, nullptr, 2);
    unsigned int shamt_temp = stoul(shamt, nullptr, 2);
    unsigned int funct_temp = stoul(funct, nullptr, 2);
    switch (funct_temp) {
    case 0x21: // addu
        Register[rd_temp] = Register[rs_temp] + Register[rt_temp];
        PC += 4;
        break;
    case 0x24: // and
        Register[rd_temp] = Register[rs_temp] & Register[rt_temp];
        PC += 4;
        break;
    case 8: // jr
        PC = Register[rs_temp];
        break;
    case 0x27: // nor
        Register[rd_temp] = ~(Register[rs_temp] | Register[rt_temp]);
        PC += 4;
        break;
    case 0x25: // or
        Register[rd_temp] = Register[rs_temp] | Register[rt_temp];
        PC += 4;
        break;
    case 0x2b: // sltu
        Register[rd_temp] = (Register[rs_temp] < Register[rt_temp]) ? 1 : 0;
        PC += 4;
        break;
    case 0: // sll
        Register[rd_temp] = Register[rt_temp] << shamt_temp;
        PC += 4;
        break;
    case 2: // srl
        Register[rd_temp] = Register[rt_temp] >> shamt_temp;
        PC += 4;
        break;
    case 0x23: // subu
        Register[rd_temp] = Register[rs_temp] - Register[rt_temp];
        PC += 4;
        break;
    default:
        throw invalid_argument("Unknown R-type instruction.");
    }
}

string convertToBits(const string& num, unsigned int size) {
    int numValue = stoi(num, nullptr, 0);
    bitset<32> bits(numValue);
    return bits.to_string().substr(32 - size);
}


string BitExtend(const string& num, bool isSignExt) {
    char extendChar = isSignExt ? num[0] : '0';
    return string(32 - num.length(), extendChar) + num;
}



void ExecuteITypeInstruction(const string& op, const string& rs, const string& rt, const string& immOffset) {
    unsigned int op_temp = stoul(op, nullptr, 2);
    unsigned int rs_temp = stoul(rs, nullptr, 2);
    unsigned int rt_temp = stoul(rt, nullptr, 2);
    switch (op_temp) {
    case 9: // addiu
        Register[rt_temp] = Register[rs_temp] + stoul(BitExtend(immOffset, true), nullptr, 2);
        PC += 4;
        break;
    case 0xc: // andi
        Register[rt_temp] = Register[rs_temp] & stoul(BitExtend(immOffset, false), nullptr, 2);
        PC += 4;
        break;
    case 4: // beq
        PC += (Register[rs_temp] == Register[rt_temp]) ? 4 * stoul(immOffset, nullptr, 2) : 4;
        break;
    case 5: // bne
        PC += (Register[rs_temp] != Register[rt_temp]) ? 4 * stoul(immOffset, nullptr, 2) : 4;
        break;
    case 0xf: // lui
        Register[rt_temp] = stoul(immOffset + string(16, '0'), nullptr, 2);
        PC += 4;
        break;
    case 0x23: // lw
        Register[rt_temp] = MemoryRead(Register[rs_temp] + stoul(immOffset, nullptr, 2), true);
        PC += 4;
        break;
    case 0x20: // lb
        Register[rt_temp] = stoul(BitExtend(Memory[Register[rs_temp] + stoul(immOffset, nullptr, 2)], true), nullptr, 2);
        PC += 4;
        break;
    case 0xd: // ori
        Register[rt_temp] = Register[rs_temp] | stoul(BitExtend(immOffset, false), nullptr, 2);
        PC += 4;
        break;
    case 0xb: // sltiu
        Register[rt_temp] = (Register[rs_temp] < stoul(BitExtend(immOffset, true), nullptr, 2)) ? 1 : 0;
        PC += 4;
        break;
    case 0x2b: // sw
        MemoryWrite(Register[rs_temp] + stoul(immOffset, nullptr, 2), Register[rt_temp], true);
        PC += 4;
        break;
    case 0x28: // sb
        MemoryWrite(Register[rs_temp] + stoul(immOffset, nullptr, 2), Register[rt_temp], false);
        PC += 4;
        break;
    default:
        throw invalid_argument("Unknown I-type instruction.");
    }
}


void ExecuteJTypeInstruction(const string& op, const string& target) {
    unsigned int op_temp = stoul(op, nullptr, 2);
    unsigned int target_temp = stoul(target, nullptr, 2);
    unsigned int address = (PC & 0xff000000) | (target_temp * 4);
    switch (op_temp) {
    case 2: // j
        PC = address;
        break;
    case 3: // jal
        Register[31] = PC + 4;
        PC = address;
        break;
    default:
        throw invalid_argument("Unknown J-type instruction.");
    }
}

void DecodeAndExecuteInstruction() {
    bitset<32> instructionBits(MemoryRead(PC, true));
    string instructionStr = instructionBits.to_string();
    string op = instructionStr.substr(0, 6);
    if (op == "000000") { // R-type
        ExecuteRTypeInstruction(instructionStr.substr(6, 5), instructionStr.substr(11, 5),
            instructionStr.substr(16, 5), instructionStr.substr(21, 5),
            instructionStr.substr(26));
    }
    else if (op == "000010" || op == "000011") { // J-type
        ExecuteJTypeInstruction(op, instructionStr.substr(6));
    }
    else { // I-type
        ExecuteITypeInstruction(op, instructionStr.substr(6, 5), instructionStr.substr(11, 5),
            instructionStr.substr(16));
    }
}

string ChangeDecToHex(unsigned int decimal) {
    stringstream hexStream;
    hexStream << hex << decimal;
    return "0x" + hexStream.str();
}

void PrintRegistersAndMemory(bool printMemory, const vector<string>& memoryRange) {

    cout << "Current register values:" << endl;
    cout << "-----------------------------------" << endl;
    cout << "PC: " << ChangeDecToHex(PC) << endl;
    cout << "Registers: " << endl;
    for (const auto& reg : Register) {
        cout << "R" << reg.first << ": " << ChangeDecToHex(reg.second) << endl;
    }

    if (printMemory) {
        cout << endl;
        cout << "Memory content [" << memoryRange.at(0) << ".." << memoryRange.at(1) << "]:" << endl;
        cout << "------------------------------------" << endl;
        unsigned int printMemStart = stoul(memoryRange.at(0), nullptr, 0);
        unsigned int printMemLast = stoul(memoryRange.at(1), nullptr, 0);
        for (unsigned int addr = printMemStart; addr <= printMemLast; addr += 4) {
            int tempValue = MemoryRead(addr, true);
            cout << ChangeDecToHex(addr) << ": " << ChangeDecToHex(tempValue) << endl;
        }
    }
}

vector<string> Split(const string& str, char separator) {
    vector<string> tokens;
    istringstream iss(str);
    string token;

    while (getline(iss, token, separator)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}

bool hasChar(const string& str, char c) {
    for (char ch : str) {
        if (ch == c) {
            return true;
        }
    }
    return false;
}

bool isNumeric(const std::string& str, bool isHex) {
    for (char ch : str) {
        if (!(std::isdigit(ch) || (isHex && ((ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f') || ch == 'x')))) {
            return false;
        }
    }
    return true;
}

void MemoryAndInstuctionExe(const string& inputFileName, bool Memout, const vector<string>& pMemAdd, bool instlmt, unsigned int instructionLimit, bool printA) {
    ifstream inputBinaryFile(inputFileName, ios::binary);
    if (!inputBinaryFile.is_open()) {
        cerr << "Error: Open file" << endl;
        PrintRegistersAndMemory(Memout, pMemAdd);
        exit(1);
    }

    string temp = "";
    vector<string> binaryfileLine;
    while (getline(inputBinaryFile, temp)) {
        binaryfileLine.push_back(temp);
    }
    inputBinaryFile.close();

    unsigned int fileSize = binaryfileLine.size();
    unsigned int textSize = stoul(binaryfileLine.at(0), nullptr, 0) / 4;
    unsigned int dataSize = stoul(binaryfileLine.at(1), nullptr, 0) / 4;
    for (unsigned int i = 2; i < textSize + 2; i++) {
        MemoryWrite(TEXT_START_ADDR + 4 * (i - 2), stoul(binaryfileLine.at(i), nullptr, 0), true);
    }
    for (unsigned int i = fileSize - dataSize; i < fileSize; i++) {
        MemoryWrite(DATA_START_ADDR + 4 * (i - fileSize + dataSize), stoul(binaryfileLine.at(i), nullptr, 0), true);
    }

    unsigned int count = 0;
    unsigned int lastaddress = TEXT_START_ADDR + 4 * textSize;
    while (PC < lastaddress) {
        if (instlmt && instructionLimit == count) {
            break;
        }
        count++;
        DecodeAndExecuteInstruction();
        if (printA) {
            PrintRegistersAndMemory(Memout, pMemAdd);
        }
    }
}


int main(int argc, char** argv) {
    vector<string> pMemAdd;
    unsigned int instructionLimit = 0;
    string inputFileName = "";
    PC = TEXT_START_ADDR;
    bool Memout = false;
    bool printA = false;
    bool instlmt = false;
    for (int i = 0; i < 32; i++) {
        Register[i] = 0;
    }
   
    try {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-m") == 0) {
                Memout = true;
                i++;
                if (!hasChar(argv[i], ':')) {
                    throw invalid_argument("invalid memory address format");
                }
                pMemAdd = Split(argv[i], ':');
                if (!isNumeric(pMemAdd.at(0), true) || !isNumeric(pMemAdd.at(1), true)) {
                    throw invalid_argument("invalid memory address");
                }
                if (stoul(pMemAdd.at(0), nullptr, 0) > stoul(pMemAdd.at(1), nullptr, 0)) {
                    throw invalid_argument("invalid memory address range");
                }
            }
            else if (strcmp(argv[i], "-d") == 0) {
                printA = true;
            }
            else if (strcmp(argv[i], "-n") == 0) {
                instlmt = true;
                i++;
                if (!isNumeric(argv[i], true)) {
                    throw invalid_argument("invalid instruction number");
                }
                instructionLimit = stoul(argv[i], nullptr, 0);
            }
            else {
                inputFileName = argv[i];
            }
        }
    }
    catch (const std::exception& e) {
        cerr << "Error" << e.what() << endl;
        PrintRegistersAndMemory(false, pMemAdd);
        exit(1);
    }
    MemoryAndInstuctionExe(inputFileName, Memout, pMemAdd, instlmt, instructionLimit, printA);
    PrintRegistersAndMemory(Memout, pMemAdd);
    
        

    

    return 0;
}

