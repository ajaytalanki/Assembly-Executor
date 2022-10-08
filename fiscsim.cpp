#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <vector>
#include <iomanip>
#include <cmath>

int hexToDecimal(const std::string& hex) {
    double sum = 0;
    int digit = 0;
    for(int i = 1; i <= static_cast<int>(hex.size()); ++i) {
        if(isdigit(hex.at(i - 1)))
            digit = hex.at(i - 1) - 48;
        else
            digit = hex.at(i - 1) - 'A' + 10;
        sum += (digit * pow(16, (double)hex.size() - i));
    }
    return static_cast<int>(sum);
}

int main(int argc, char** argv) {
    std::ifstream file;
    std::string line;
    std::vector<u_int8_t> IM(64);
    std::vector<u_int8_t> registers(4, 0);
    std::vector<std::string> dissasembly;
    u_int8_t binary = 0, opCode = 0, Rd = 0, Rm = 0, Rn = 0, target = 0;
    file.open(argv[1]);
    if(!file.is_open()) {
        std::cout << "File did not open." << std::endl;
        exit(0);
    }
    std::getline(file, line);
    if(line != "v2.0 raw"){
        std::cout << "File must conform to Logism format. The first line must read v2.0 raw" << std::endl;
        exit(0);
    }
    int i = 0;
    while(std::getline(file, line)){
        binary = hexToDecimal(line);
        IM[i] = binary;
        i++;
    }
    for(const auto& instruction : IM){
        opCode = (instruction >> 6) & 0b00000011;
        if((unsigned)opCode == 3)
            target = instruction & 0b00111111;
        Rn = (instruction >> 4) & 0b00000011;
        Rm = (instruction >> 2) & 0b00000011;
        Rd = instruction & 0b00000011;
        if(opCode == 0)
            registers[Rd] = registers[Rn] + registers[Rm];
        else if(opCode == 1)
            registers[Rd] = registers[Rn] & registers[Rm];
        else if(opCode == 2)
            registers[Rd] = ~registers[Rn];
        else if(opCode == 3){
            // BNZ
        }


    }
//    std::cout << unsigned(opCode) << " " << unsigned(Rn) << " " << unsigned(Rm)
//              << " " << unsigned(Rd) << std::endl;
    return 0;
}

