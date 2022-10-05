#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cctype>

//MUST CHECK FOR INVALID INSTRUCTIONS IN PASS 1

std::string lower(std::string str){
    for(int i = 0; i < str.length(); i++)
        str[i] = tolower(str[i]);
    return str;
}

void modifyBits(int& n, const int& pos, const int& change){
    int mask = 0b11 << pos;
    int temp = ((n & ~mask) | (change << pos));
    n &= temp;
}

int main(int argc, char** argv) {
    //PASS 1
    std::ifstream file;
    std::string line;
    std::string word;
    int address = 0;
    std::unordered_map<std::string, int> symbol_table;
    std::unordered_set<std::string> instructions{"not", "and", "add", "bnz"};
    std::unordered_map<std::string, int> registers{{"r0", 0}, {"r1", 1}, {"r2", 2}, {"r3", 3}};
    std::vector<std::string> lines;
    file.open(argv[1]);
    if(!file.is_open())
        std::cout << "File did not open." << std::endl;
    while(std::getline(file, line)){
        line = lower(line);
        std::stringstream parser(line);
        parser >> word;
        if(word.back() == ':'){ // label definition
            if(symbol_table.find(word) != symbol_table.end()) // label is already defined
                std::cout <<  "Label " << word << "on line " << address << "is already defined." << std::endl;
            else {
                symbol_table.insert(std::make_pair(word.substr(0, word.size() - 1), address));
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
    // ADD Rd Rn Rm : Rd = Rn + Rm
    // Op Rn Rm Rd
    int binary = 255;
    std::string Rd, Rm, Rn;
    for(int i = 0; i < lines.size(); i++){
        std::stringstream parser(lines[i]);
        binary = 255;
        parser >> word;
        if(word == "add") {
            modifyBits(binary, 6, 0);
            parser >> Rd;
            parser >> Rn;
            parser >> Rm;
            modifyBits(binary, 4, registers[Rn]);
            modifyBits(binary, 2, registers[Rm]);
            modifyBits(binary, 0, registers[Rd]);
        } else if (word == "and") {
            modifyBits(binary, 6, 1);
            parser >> Rd;
            parser >> Rn;
            parser >> Rm;
            modifyBits(binary, 4, registers[Rn]);
            modifyBits(binary, 2, registers[Rm]);
            modifyBits(binary, 0, registers[Rd]);
        }
    }
    std::ostringstream oss;
    oss << std::hex << binary;
    std::cout << oss.str();
    return 0;
}

