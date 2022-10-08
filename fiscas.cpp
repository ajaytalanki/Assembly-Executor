#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <vector>
#include <iomanip>

std::string lower(std::string str){
    for(char& c : str)
        c = (char)tolower(c);
    return str;
}

std::string upper(std::string str){
    for(char& c : str)
        c = (char)toupper(c);
    return str;
}

void changeBits(int& n, const int& pos, const int& change){
    int mask = 0b11 << pos;
    int temp = ((n & ~mask) | (change << pos));
    n &= temp;
}

void parse(std::string& str, const std::unordered_set<std::string>& instructions){
    std::stringstream parser(str);
    std::string word;
    while(parser >> word){ // find location of instruction word
        if(instructions.find(word) != instructions.end())
            break;
    }
    auto index = str.find(word);
    str = str.substr(index, str.size() - index); // trimming string until instruction word
    index = str.find(';');
    if(index != std::string::npos) // deleting comments
        str = str.substr(0, index);
}

void registerCheck(const std::string& reg, const std::unordered_map<std::string, int>& registers){
    if(reg.empty())
        return;
    if(registers.find(reg) == registers.end())
        std::cout << reg << " is not a valid register." << std::endl;
}

int main(int argc, char** argv) {
    //PASS 1
    std::ifstream file;
    file.open(argv[1]);
    if(!file.is_open() or argc < 3) {
        std::cout << "Usage: fiscas <asm file> <hex file> [-l]" << std::endl;
        exit(0);
    }
    std::string line, word;
    int address = 0;
    std::map<std::string, int> symbolTable;
    std::unordered_set<std::string> instructions{"not", "and", "add", "bnz"};
    std::unordered_map<std::string, int> registers{{"r0", 0}, {"r1", 1}, {"r2", 2}, {"r3", 3}};
    std::vector<std::string> assembly, hex;
    while(std::getline(file, line)){
        line = lower(line); // makes assembler case sensitive
        std::stringstream parser(line);
        parser >> word;
        if(word.back() == ':'){ // label definition
            if(symbolTable.find(word) != symbolTable.end()) // label is already defined
                std::cout <<  "Label " << word << "is already defined." << std::endl;
            else {
                symbolTable.insert(std::make_pair(word.substr(0, word.size() - 1), address));
                if((parser >> word) and instructions.find(word) != instructions.end()) {  // there is an instruction
                    assembly.emplace_back(line);
                    address++;
                }
            }
        } else if(instructions.find(word) != instructions.end()){ // line starts with instruction
            assembly.emplace_back(line);
            address++;
        }
        if(address >= 64) {
            std::cout << "Assembler is constrained to addresses 0-63, any code after won't be processed." << std::endl;
            break;
        }
    }

    //PASS 2
    std::ofstream out;
    out.open("fiscas.h");
    out << "v2.0 raw" << std::endl;
    int binary = 255;
    std::string Rd, Rm, Rn, target;
    for(int i = 0; i < assembly.size(); i++){
        parse(assembly[i], instructions); // gets rid of label definitions and comments
        std::stringstream parser(assembly[i]);
        binary = 255;
        parser >> word;
        if(instructions.find(word) == instructions.end())
            std::cout << word << " is an invalid instruction" << std::endl;
        if (word == "bnz") {
            parser >> target;
            if(symbolTable.find(target) != symbolTable.end()) { // label is defined
                binary = symbolTable[target];
                binary |= (3 << 6);
            } else
                std::cout << "Label " << target << " is undefined." << std::endl;
            goto end;
        }
        parser >> Rd;
        parser >> Rn;
        parser >> Rm;
        registerCheck(Rd, registers); // checks for valid registers
        registerCheck(Rn, registers);
        registerCheck(Rm, registers);
        if(word == "add"){
            changeBits(binary, 6, 0);
            changeBits(binary, 4, registers[Rn]);
            changeBits(binary, 2, registers[Rm]);
            changeBits(binary, 0, registers[Rd]);
        } else if (word == "and"){
            changeBits(binary, 6, 1);
            changeBits(binary, 4, registers[Rn]);
            changeBits(binary, 2, registers[Rm]);
            changeBits(binary, 0, registers[Rd]);
        } else if (word == "not"){
            changeBits(binary, 6, 2);
            changeBits(binary, 4, registers[Rn]);
            changeBits(binary, 2, 00);
            changeBits(binary, 0, registers[Rd]);
        }
        end:
        std::ostringstream oss;
        oss << std::hex << std::setw(2) << std::setfill('0') << binary;
        std::string hexCode = upper(oss.str());
        out << hexCode << std::endl;
        hex.emplace_back(hexCode);
    }
    std::string argv3 = argv[3];
    if(argc > 3 and argv3 == "-l"){ // listing output when user compiles with -l flag
        std::cout << "*** LABEL LIST ***" << std::endl;
        for(const auto& pair : symbolTable)
            std::cout << pair.first << "   " << std::setw(2) << std::setfill('0') << pair.second << std::endl;
        std::cout << "*** MACHINE PROGRAM ***" << std::endl;
        for(int i = 0; i < hex.size(); i++){
            std::cout << std::setw(2) << std::setfill('0') << i;
            std::cout << ":" << hex[i] << "   " << assembly[i] << std::endl;
        }
    }
    return 0;
}
