#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <vector>
#include <iomanip>

//MUST CHECK FOR INVALID INSTRUCTIONS IN PASS 1

std::string lower(std::string str){
    for(char& c : str)
        c = tolower(c);
    return str;
}

std::string upper(std::string str){
    for(char& c : str)
        c = toupper(c);
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
    while(parser >> word){
        if(instructions.find(word) != instructions.end())
            break;
    }
    auto index = str.find(word);
    str = str.substr(index, str.size() - index);
    index = str.find(';');
    if(index != std::string::npos)
        str = str.substr(0, index);
}

void registerCheck(const std::string& reg, const std::unordered_map<std::string, int>& registers){
    if(registers.find(reg) == registers.end())
        std::cout << reg << " is not a valid register." << std::endl;
}

int main(int argc, char** argv) {
    //PASS 1
    std::ifstream file;
    file.open(argv[1]);
    if(!file.is_open()) {
        std::cout << "File did not open." << std::endl;
        exit(0);
    }
    if(argc < 3)
        std::cout << "Usage: fiscas <asm file> <hex file> [-l]" << std::endl;
    std::string line, word;
    int address = 0;
    std::map<std::string, int> symbolTable;
    std::unordered_set<std::string> instructions{"not", "and", "add", "bnz"};
    std::unordered_map<std::string, int> registers{{"r0", 0}, {"r1", 1}, {"r2", 2}, {"r3", 3}};
    std::vector<std::string> lines;
    while(std::getline(file, line)){
        line = lower(line);
        std::stringstream parser(line);
        parser >> word;
        if(word.back() == ':'){ // label definition
            if(symbolTable.find(word) != symbolTable.end()) // label is already defined
                std::cout <<  "Label " << word << "is already defined." << std::endl;
            else {
                symbolTable.insert(std::make_pair(word.substr(0, word.size() - 1), address));
                if((parser >> word) and instructions.find(word) != instructions.end()) {  // there is an instruction
                    lines.emplace_back(line);
                    address++;
                }
            }
        } else if(instructions.find(word) != instructions.end()){
            lines.emplace_back(line);
            address++;
        }
    }

    //PASS 2
    std::ofstream out;
    out.open("fiscas.h");
    out << "v2.0 raw" << std::endl;
    int binary = 255;
    std::string Rd, Rm, Rn;
    for(int i = 0; i < lines.size(); i++){
        parse(lines[i], instructions);
        std::stringstream parser(lines[i]);
        binary = 255;
        parser >> word;
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
        } else if (word == "bnz") {
            if(symbolTable.find(Rd) != symbolTable.end()) {
                binary = symbolTable[Rd];
                binary |= (3 << 6);
            } else
                std::cout <<  "Label " << Rd << " is not in the symbol table." << std::endl;
        }
        std::ostringstream oss;
        oss << std::hex << std::setw(2) << std::setfill('0') << binary;
        out << upper(oss.str()) << std::endl;
    }
    out.close();
    file.close();
    return 0;
}
