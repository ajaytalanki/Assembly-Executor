#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <vector>
#include <iomanip>
#include <cstring>

/* converts instruction to lowercase */
std::string lower(std::string str){
  for(char& c : str)
    c = (char)tolower(c);
  return str;
}

/* modifies specified bits of a binary string */
void changeBits(int& n, const int& pos, const int& change){
  int mask = 0b11 << pos;
  int temp = ((n & ~mask) | (change << pos));
  n &= temp;
}

// trims whitespace of instruction and deletes comments */
void parse(std::string& s,const std::unordered_set<std::string>& instructions){
  std::stringstream parser(s);
  std::string word;
  while(parser >> word){ // find location of instruction word
    if(instructions.find(word) != instructions.end())
      break;
  }
  auto index = s.find(word);
  s = s.substr(index, s.size() - index); // trimming until instruction
  index = s.find(';');
  if(index != std::string::npos) // deleting comments
    s = s.substr(0, index);
}

/* checks to see if register is one of the four valid registers */
void registerCheck(const std::string& reg, const std::unordered_map<std::string, int>& registers){
  if(reg.empty())
    return;
  if(registers.find(reg) == registers.end())
    throw std::runtime_error(reg + " is not a valid register");
}

/* if -l flag is specified, print a list of labels and machine code of each instruction */
void displayListing(int argc, char** argv, const std::map<std::string, int>&
    symbolTable, const std::vector<std::string>& hex, const std::vector<std::string>& assembly){
  if((argc == 3 and strcmp(argv[2], "-l") == 0)
      or (argc == 4 and strcmp(argv[3], "-l") == 0)){
    std::cout << "*** LABEL LIST ***" << std::endl;
    for(const auto& pair : symbolTable)
      std::cout << pair.first << "   " << std::setw(2) << std::setfill('0')
                << pair.second << std::endl;
    std::cout << std::endl;
    std::cout << "*** MACHINE PROGRAM ***" << std::endl;
    for(int i = 0; i < hex.size(); i++){
      std::cout << std::setw(2) << std::setfill('0') << std::hex <<
                std::uppercase << i;
      std::cout << ":" << hex[i] << "   " << assembly[i] << std::endl;
    }
  }
}

int main(int argc, char** argv) {

  /* verifies .s file and determines if output file is specified */
  std::ifstream file;
  file.open(argv[1]);
  bool noOutputFile;
  if(!file.is_open())
    throw std::runtime_error("File did not open");
  if(argc == 2 or (argc == 3 and strcmp(argv[2], "-l") == 0)){
    std::cout << "Usage: fiscas <asm file> <hex file> [-l]" << std::endl;
    noOutputFile = true;
  }

  /* map of labels and their addresses */
  std::map<std::string, int> symbolTable;
  /* set of instructions */
  std::unordered_set<std::string> instructions{"not", "and", "add", "bnz"};
  /* maps registers to decimal value */
  std::unordered_map<std::string, int> registers{{"r0", 0}, {"r1", 1}, {"r2", 2}, {"r3", 3}};
  /* defines list of assembly instructions and hex instructions */
  std::vector<std::string> assembly, hex;

  /* PASS 1 */
  std::string line, word;
  int address = 0;
  while(std::getline(file, line)){

    /* lowercases string grabs each word from string */
    line = lower(line);
    std::stringstream parser(line);
    parser >> word;

    /* word is a label */
    if(word.back() == ':'){
      if(symbolTable.find(word) != symbolTable.end()) {
        std::cout <<  "Label " << word << "is already defined." << std::endl;
        exit(0);
      } else {
        symbolTable.insert(std::make_pair(word.substr(0,word.size() - 1), address));
        if((parser >> word) && instructions.find(word) != instructions.end()){
          assembly.emplace_back(line);
          address++;
        }
      }
    }
    /* checks to see if first word in line is instruction */
    else if(instructions.find(word) != instructions.end()){
      assembly.emplace_back(line);
      address++;
    }
    else if(instructions.find(word) == instructions.end())
      throw std::runtime_error(word + " is an invalid instruction.");
  }

  std::ofstream out;
  if(noOutputFile)
    out.open("fiscas.hex");
  else
    out.open(argv[2]);
  out << "v2.0 raw" << std::endl;
  int binary = 255;
  std::string Rd, Rm, Rn, target;

  /* PASS 2 */
  for(int i = 0; i < assembly.size(); i++){
    parse(assembly[i], instructions); // gets rid of label defs + comments
    std::stringstream parser(assembly[i]);
    binary = 255;
    parser >> word;

    /* if BNZ, disregard registers and shift target address into machine code */
    if (word == "bnz") {
      target = "";
      parser >> target;
      if(target.empty())
        throw std::runtime_error("bnz instruction requires target address");
      if(symbolTable.find(target) != symbolTable.end()) { // label is defined
        binary = symbolTable[target];
        binary |= (3 << 6);
      } else
        throw std::runtime_error("Label " + target + " is undefined.");
      goto end;
    }

    /* checks for valid register names */
    Rd = "";
    Rn = "";
    Rm = "";
    parser >> Rd;
    parser >> Rn;
    parser >> Rm;
    registerCheck(Rd, registers);
    registerCheck(Rn, registers);
    registerCheck(Rm, registers);

    /* uses bit shifting to create machine code */
    if(word == "add"){
      if(Rd.empty() or Rn.empty() or Rm.empty())
        throw std::runtime_error("add requires three registers");
      changeBits(binary, 6, 0);
      changeBits(binary, 4, registers[Rn]);
      changeBits(binary, 2, registers[Rm]);
      changeBits(binary, 0, registers[Rd]);
    } else if (word == "and"){
      if(Rd.empty() or Rn.empty() or Rm.empty())
        throw std::runtime_error("and requires three registers");
      changeBits(binary, 6, 1);
      changeBits(binary, 4, registers[Rn]);
      changeBits(binary, 2, registers[Rm]);
      changeBits(binary, 0, registers[Rd]);
    } else if (word == "not"){
      if(!Rm.empty())
        throw std::runtime_error("not only takes two registers");
      changeBits(binary, 6, 2);
      changeBits(binary, 4, registers[Rn]);
      changeBits(binary, 2, 00);
      changeBits(binary, 0, registers[Rd]);
    }

    /* formats, prints, and outputs hex string to output file */
    end:
    std::ostringstream oss;
    oss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << binary;
    std::string hexCode = oss.str();
    out << hexCode << std::endl;
    hex.emplace_back(hexCode);
  }

  /* Prints out list of labels as well as machine code of each instruction */
  displayListing(argc, argv, symbolTable, hex, assembly);
  return 0;
}
