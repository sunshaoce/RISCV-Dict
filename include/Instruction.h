#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#define DEBUG

#include <string>
#include <vector>

class Instruction {
  std::string Name;
  std::string Extentions;
  std::string Format;
  std::string Actual;
  std::string Description;
  std::vector<int> Columns;
  std::vector<std::string> Encodings;

  std::string printChart();

public:
  Instruction(std::string N = "", std::string Exts = "", std::string F = "",
              std::string A = "", std::string D = "", std::vector<int> C = {},
              std::vector<std::string> E = {})
      : Name(N), Extentions(Exts), Format(F), Actual(A), Description(D),
        Columns(C), Encodings(E) {}
  std::string print() {
    return "NAME:\n\t" + Name + "\nEXTENTIONS:\n\t" + Extentions +
           "\nFORMAT:\n\t" + Format + "\nACTUAL:\n\t" + Actual +
           "\nDESCRIPTION:\n\t" + Description + "\nENCODING:\n" + printChart() +
           "\n\n";
  }
  std::string getName() { return Name; }
};

#endif // !INSTRUCTION_H
