#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>

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

std::string upper(std::string str){
  for(char& c : str)
    c = (char)toupper(c);
  return str;
}

std::string format(const u_int8_t& num){
  std::ostringstream oss;
  oss << std::setw(2) << std::setfill('0') << std::hex << num;
  return upper(oss.str());
}

void disassembly(const u_int8_t& instruction){
  u_int8_t opCode = (instruction >> 6) & 0b00000011;
  u_int8_t Rn = (instruction >> 4) & 0b00000011;
  u_int8_t Rm = (instruction >> 2) & 0b00000011;
  u_int8_t Rd = instruction & 0b00000011;
  std::string registers[4] = {"r0", "r1", "r2", "r3"};
  if(opCode == 0){
    std::cout << "Disassembly: add " << registers[Rd] << " " <<
    registers[Rn] << " " << registers[Rm];
  } else if(opCode == 1) {
    std::cout << "Disassembly: and " << registers[Rd] << " " <<
                registers[Rn] << " " << registers[Rm];
  } else if(opCode == 2)
    std::cout << "Disassembly: not " << registers[Rd] << " " << registers[Rn];
  else if(opCode == 3)
    std::cout << "Disassembly: bnz " << (unsigned)(instruction & 0b00111111);
  std::cout << std::endl;
}

int main(int argc, char** argv) {
  std::ifstream file;
  std::string line;
  std::vector<u_int8_t> IM(64);
  std::vector<u_int8_t> registers(4, 0);
  u_int8_t binary = 0, opCode = 0, Rd = 0, Rm = 0, Rn = 0, target = 0;
  bool Z = false;
  file.open(argv[1]);
  if(!file.is_open()) {
    std::cout << "File did not open." << std::endl;
    exit(0);
  }
  std::getline(file, line);
  if(line != "v2.0 raw"){
    std::cout << "File must conform to Logism format. "
                   "The first line must read v2.0 raw" << std::endl;
    exit(0);
  }
  int i = 0, cycle = 1;
  while(std::getline(file, line)){
    binary = hexToDecimal(line);
    IM[i] = binary;
    i++;
  }
  i = 0;
  while(i < IM.size()){
    opCode = (IM[i] >> 6) & 0b00000011;
    Rn = (IM[i] >> 4) & 0b00000011;
    Rm = (IM[i] >> 2) & 0b00000011;
    Rd = IM[i] & 0b00000011;
    if(opCode == 0) {
      registers[Rd] = registers[Rn] + registers[Rm];
      Z = (registers[Rn] + registers[Rm]) == 0;
    } else if(opCode == 1) {
      registers[Rd] = registers[Rn] & registers[Rm];
      Z = (registers[Rn] & registers[Rm]) == 0;
    } else if(opCode == 2) {
      registers[Rd] = ~registers[Rn];
      Z = (~registers[Rn]) == 0;
    } else if(opCode == 3){ //BNZ
      if(Z) {
        target = IM[i] & 0b00111111;
        i = target;
        cycle++;
        continue;
      }
    }
    std::cout << "Cycle: " << cycle << " State:PC:" << std::hex <<
    format(i) << " Z:" << std::boolalpha << Z
    << " R0: " << format(registers[0]) << " R1: "<<
    format(registers[1]) << " R2: " << format(registers[2]) << " R3: "
    << format(registers[3]) << std::endl;
    disassembly(IM[i]);
    cycle++;
    i++;
  }
  return 0;
}
