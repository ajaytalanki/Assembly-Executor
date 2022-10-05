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

int main(int argc, char** argv) {
    // PASS 1
    std::ifstream file;
    std::string line;
    std::string word;
    int address = 0;
    std::unordered_map<std::string, int> symbol_table;
    std::unordered_set<std::string> instructions{"not", "and", "add", "bnz"};
    std::unordered_map<std::string, std::string> registers{{"r0", "00"}, {"r1", "01"}, {"r2", "10"}, {"r3", "11"}};
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
    std::string Rd, Rm, Rn;
    for(int i = 0; i < lines.size(); i++){
        std::stringstream parser(lines[i]);
        std::string binary_string;
        parser >> word;
        if(word == "add")
            binary_string += "00";
    }

    return 0;
}
