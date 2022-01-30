#include <array>
#include <cstdlib>
#include <iostream>
#include <vector>

enum class Opcode : int32_t {
  CALL,  // push adress of next instruction to stack and jump to called address
  RET,   // pop stack and return to that adress
  CMP,   // compare registers
  JNE,   // if equal flag not set, jump to relative position
  PUSH,  // push register to stack
  POP,   // pop stack to register
  SET,   // set register to constant
  COPY,  // copy first register to second register
  ADD,   // add second register to first register
  DEC,   // decrement register
  PRINT, // print register
  EXIT,  // exit program
};

void exec(std::vector<int32_t> &code, uint32_t sp) {

  uint32_t ip = 0;
  bool equalFlag = false;

  std::array<int32_t, 3> registers;

  for (;;) {
    switch (static_cast<Opcode>(code[ip])) {
    case Opcode::CALL: {
      code[sp] = ip + 2;
      sp++;
      ip = code[ip + 1];
      break;
    }
    case Opcode::RET: {
      sp--;
      ip = code[sp];
      break;
    }
    case Opcode::JNE: {
      if (!equalFlag) {
        ip += code[ip + 1];
      } else {
        ip += 2;
      }
      break;
    }
    case Opcode::PRINT: {
      std::cout << registers[code[ip + 1]] << " \n";
      ip += 2;
      break;
    }
    case Opcode::CMP: {
      equalFlag = registers[code[ip + 1]] == code[ip + 2];
      ip += 3;
      break;
    }
    case Opcode::SET: {
      registers[code[ip + 1]] = code[ip + 2];
      ip += 3;
      break;
    }
    case Opcode::PUSH: {
      code[sp] = registers[code[ip + 1]];
      sp++;
      ip += 2;
      break;
    }
    case Opcode::POP: {
      sp--;
      registers[code[ip + 1]] = code[sp];
      ip += 2;
      break;
    }
    case Opcode::DEC: {
      registers[code[ip + 1]]--;
      ip += 2;
      break;
    }
    case Opcode::ADD: {
      registers[code[ip + 1]] += registers[code[ip + 2]];
      ip += 3;
      break;
    }
    case Opcode::COPY: {
      registers[code[ip + 2]] = registers[code[ip + 1]];
      ip += 3;
      break;
    }
    case Opcode::EXIT: {
      exit(EXIT_SUCCESS);
      break;
    }
    default:
      std::cerr << "unexpected opcode: " << code[ip] << " at " << ip << "\n";
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "ERROR: agrument missing.\n";
    std::cout << "USAGE e.g. \"" << argv[0] << " 20\" to print the 20th fibonacci number.\n";
    return EXIT_FAILURE;
  }

  const int32_t n = std::stoi(argv[1]);

  std::vector<int32_t> fibcode{
      static_cast<int32_t>(Opcode::SET), 0, n, // write the value n to register 0
      static_cast<int32_t>(Opcode::CALL), 8, // call the fibonacci method (starting at offset 9)
      static_cast<int32_t>(Opcode::PRINT), 1, // print the return value of the fibonacci method (register 1)
      static_cast<int32_t>(Opcode::EXIT), // terminate program
      // fibonacci method begins here
      static_cast<int32_t>(Opcode::CMP), 0, 0, // is argument zero?
      static_cast<int32_t>(Opcode::JNE), 6, // if not zero skip next two instructions
      static_cast<int32_t>(Opcode::SET), 1, 0, // set return value (register 1) to 0
      static_cast<int32_t>(Opcode::RET), // return
      static_cast<int32_t>(Opcode::CMP), 0, 1, // is argument one?
      static_cast<int32_t>(Opcode::JNE), 6, // if not one, skip next two instructions
      static_cast<int32_t>(Opcode::SET), 1, 1, // set return value (register 1) to 1
      static_cast<int32_t>(Opcode::RET), // return
      static_cast<int32_t>(Opcode::PUSH), 0, // argument is neither 0 nor 1, we will do recursive calls, save register 0
      static_cast<int32_t>(Opcode::PUSH), 2, // save register 2
      static_cast<int32_t>(Opcode::DEC), 0, // decrement argument (register 0)
      static_cast<int32_t>(Opcode::CALL), 8, // call fibonacci method for arg-1
      static_cast<int32_t>(Opcode::COPY), 1, 2, // save result to register 2
      static_cast<int32_t>(Opcode::DEC), 0, // decrement argument (register 0) again
      static_cast<int32_t>(Opcode::CALL), 8, // call fibonacci method for arg-2
      static_cast<int32_t>(Opcode::ADD), 1, 2, // add the results of the two calls (register 1 += register 2)
      static_cast<int32_t>(Opcode::POP), 2, // restore register 2
      static_cast<int32_t>(Opcode::POP), 0, // restore register 0
      static_cast<int32_t>(Opcode::RET)}; // return (result is in register 1)

  const uint32_t stackPointer = fibcode.size(); // stack begins at end of code
  fibcode.resize(fibcode.size() +
                 n * 3); // keep enough space for the stack (return adress + 2 registers per recursion level)

  exec(fibcode, stackPointer);

  return EXIT_SUCCESS;
}
