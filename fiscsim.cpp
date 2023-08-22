#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <cstring>

/* converts hex string to decimal value */
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

std::string format(const u_int8_t& num){
  std::ostringstream oss;
  oss << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (unsigned)num;
  return oss.str();
}

bool isNumber(const std::string& str){
  return str.find_first_not_of("0123456789") == std::string::npos;
}

/* checks if -d flag is set and how many cycles are specified  */
void checkCommandLineArguments(int argc, char** argv, bool& d, bool& cycles,
                               int& cycleArg){
  if(argc == 3){
    if(strcmp(argv[2], "-d") == 0)
      d = true;
    if(isNumber(argv[2])){
      cycles = true;
      cycleArg = 2;
    }
  } else if(argc == 4) {
    if(strcmp(argv[2], "-d") == 0)
      d = true;
    if(isNumber(argv[2])){
      cycles = true;
      cycleArg = 2;
    }
    if(strcmp(argv[3], "-d") == 0)
      d = true;
    if(isNumber(argv[3])){
      cycles = true;
      cycleArg = 3;
    }
  }
}

/* prints the disassembly of machine code instruction */
void disassembly(const u_int8_t& instruction){
  u_int8_t opCode = (instruction >> 6) & 0b00000011;
  u_int8_t Rn = (instruction >> 4) & 0b00000011;
  u_int8_t Rm = (instruction >> 2) & 0b00000011;
  u_int8_t Rd = instruction & 0b00000011;
  std::string registers[4] = {"r0", "r1", "r2", "r3"};
  if(opCode == 0)
    std::cout << "Disassembly: add " << registers[Rd] << " " << registers[Rn] << " " << registers[Rm];
  else if(opCode == 1)
    std::cout << "Disassembly: and " << registers[Rd] << " " << registers[Rn] << " " << registers[Rm];
  else if(opCode == 2)
    std::cout << "Disassembly: not " << registers[Rd] << " " << registers[Rn];
  else if(opCode == 3)
    std::cout << "Disassembly: bnz " << (unsigned)(instruction & 0b00111111);
  std::cout << std::endl;
}

int main(int argc, char** argv) {

  std::ifstream file;
  std::string line;
  bool Z, d, cycles;
  int cycleArg = 0, i = 0, cycle = 0, iBeforeBranch;
  std::vector<u_int8_t> IM;
  std::vector<u_int8_t> registers(4);
  u_int8_t binary = 0, opCode = 0, Rd = 0, Rm = 0, Rn = 0, target = 0;
  checkCommandLineArguments(argc, argv, d, cycles, cycleArg);

  file.open(argv[1]);
  if(!file.is_open())
    throw std::runtime_error("File did not open");
  std::getline(file, line);
  if(line != "v2.0 raw")
    throw std::runtime_error("File must conform to Logism format. The first line must read v2.0 raw");

  /* populates IM vector */
  while(std::getline(file, line))
    IM.emplace_back(hexToDecimal(line));

  while(i < IM.size()){

    /* if cycles is specified stop at specified cycle, else stop at 20 */
    if(cycles){
      if(std::to_string(cycle) == argv[cycleArg])
        break;
    } else if(cycle == 20)
      break;

    /* parses opCode and register numbers */
    opCode = (IM[i] >> 6) & 0b00000011;
    Rn = (IM[i] >> 4) & 0b00000011;
    Rm = (IM[i] >> 2) & 0b00000011;
    Rd = IM[i] & 0b00000011;

    /* modifies register values based on instruction */
    if(opCode == 0) {
      registers[Rd] = registers[Rn] + registers[Rm];
      Z = (registers[Rn] + registers[Rm]) == 0;
    } else if(opCode == 1) {
      registers[Rd] = registers[Rn] & registers[Rm];
      Z = (registers[Rn] & registers[Rm]) == 0;
    } else if(opCode == 2) {
      registers[Rd] = ~registers[Rn];
      if(registers[Rd] == 0)
        Z = true;
    } else if(opCode == 3){ //BNZ
      if(!Z) {
        target = IM[i] & 0b00111111;
        iBeforeBranch = i;
        i = target;
      }
    }
    cycle++;

    /* not a branch, so increment instruction */
    if(opCode != 3 or Z)
      i++;

    /* prints the status of the PC and each register */
    std::cout << "Cycle:" << cycle << " State:PC:" << format(i) <<" Z:" << Z << " R0: "
    << format(registers[0]) << " R1: "<< format(registers[1]) << " R2: "
    << format(registers[2]) << " R3: " << format(registers[3]) << std::endl;

    /* prints disassembly of each instruction */
    if(d){
      if(opCode == 3 and !Z)
        disassembly(IM[iBeforeBranch]);
      else
        disassembly(IM[i - 1]);
      std::cout << std::endl;
    }
  }
  return 0;
}
