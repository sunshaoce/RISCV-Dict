#include "Instruction.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

Instruction emitInst(string Line) {
  // Name
  decltype(Line.size()) TabIndexBegin = 0, TabIndexEnd = Line.find('\t');
  string Name = Line.substr(TabIndexBegin, TabIndexEnd);

  // Extentions
  TabIndexBegin = TabIndexEnd + 1;
  while (Line[TabIndexBegin] == '\t')
    ++TabIndexBegin;
  TabIndexEnd = Line.find('\t', TabIndexBegin);
  string Exts = Line.substr(TabIndexBegin, TabIndexEnd - TabIndexBegin);

  // Format
  TabIndexBegin = TabIndexEnd + 1;
  while (Line[TabIndexBegin] == '\t')
    ++TabIndexBegin;
  TabIndexEnd = Line.find('\t', TabIndexBegin);
  string Format = Line.substr(TabIndexBegin, TabIndexEnd - TabIndexBegin);

  // Actual
  TabIndexBegin = TabIndexEnd + 1;
  while (Line[TabIndexBegin] == '\t')
    ++TabIndexBegin;
  TabIndexEnd = Line.find('\t', TabIndexBegin);
  string Actual = Line.substr(TabIndexBegin, TabIndexEnd - TabIndexBegin);

  // Description
  TabIndexBegin = TabIndexEnd + 1;
  while (Line[TabIndexBegin] == '\t')
    ++TabIndexBegin;
  TabIndexEnd = Line.find('\t', TabIndexBegin);
  string Description = Line.substr(TabIndexBegin, TabIndexEnd - TabIndexBegin);

  // Columns
  TabIndexBegin = TabIndexEnd + 1;
  while (Line[TabIndexBegin] == '\t')
    ++TabIndexBegin;
  TabIndexEnd = Line.find('\t', TabIndexBegin);
  vector<int> Columns;
  unsigned I = TabIndexBegin + 1;
  string InstType;
  while (I != TabIndexEnd - 1) {
    if (isspace(Line[I])) {
      ++I;
      continue;
    }
    if (isdigit(Line[I])) {
      string Num{Line[I]};
      while (isdigit(Line[++I]))
        Num += Line[I];
      Columns.push_back(stoi(Num));
    }
    if (isalpha(Line[I])) {
      InstType = Line[I];
      ++I;
      while (isalpha(Line[I]) || Line[I] == '-')
        InstType += Line[I++];
      if (InstType == "R" || InstType == "S" || InstType == "Shift-I" ||
          InstType == "FCVT-R" || InstType == "F-ARITH-R")
        Columns = {7, 5, 5, 3, 5, 7};
      if (InstType == "I" || InstType == "Off-I" || InstType == "CSR-I" ||
          InstType == "CSRI-I")
        Columns = {12, 5, 3, 5, 7};
      if (InstType == "AMO-R")
        Columns = {5, 1, 1, 5, 5, 3, 5, 7};
      if (InstType == "B")
        Columns = {1, 6, 5, 5, 3, 4, 1, 7};
      if (InstType == "J")
        Columns = {1, 10, 1, 8, 5, 7};
      if (InstType == "U")
        Columns = {20, 5, 7};
      if (InstType == "C-Arith-R" || InstType == "C-Arith-I")
        Columns = {3, 1, 5, 5, 2};
      if (InstType == "C-Logic-R")
        Columns = {6, 3, 2, 3, 2};
      if (InstType == "C-Logic-I")
        Columns = {3, 1, 2, 3, 5, 2};
      if (InstType == "C-LoadStore")
        Columns = {3, 3, 3, 1, 1, 3, 2};
      if (InstType == "R4")
        Columns = {5, 2, 5, 5, 3, 5, 7};
    }
  }

  // Encodings
  TabIndexBegin = TabIndexEnd + 1;
  while (Line[TabIndexBegin] == '\t')
    ++TabIndexBegin;
  TabIndexEnd = Line.find('\t', TabIndexBegin);
  if (TabIndexEnd == string::npos)
    TabIndexEnd = Line.size() - 1;
  vector<string> Encodings;
  unsigned Index = TabIndexBegin + 1;
  while (Index <= TabIndexEnd - 1) {
    if (isspace(Line[Index])) {
      ++Index;
      continue;
    } else if (isalnum(Line[Index]) || Line[Index] == '-') {
      string Str{Line[Index]};
      ++Index;
      while (isalnum(Line[Index]) || Line[Index] == '[' || Line[Index] == ']' ||
             Line[Index] == '\'' || Line[Index] == ':' || Line[Index] == '|')
        Str += Line[Index++];
      Encodings.push_back(Str);
    } else {
      cerr << "ERROR: Unknown Char " << Line[Index] << endl;
    }
  }
  if (InstType == "R" && Encodings.size() == 3)
    Encodings = {Encodings[0], "rs2", "rs1", Encodings[1], "rd", Encodings[2]};
  if (InstType == "I" && Encodings.size() == 2)
    Encodings = {"imm[11:0]", "rs1", Encodings[0], "rd", Encodings[1]};
  if (InstType == "Off-I" && Encodings.size() == 2)
    Encodings = {"off[11:0]", "rs1", Encodings[0], "rd", Encodings[1]};
  if (InstType == "AMO-R" && Encodings.size() == 2)
    Encodings = {Encodings[0], "aq",         "rl", "rs2",
                 "rs1",        Encodings[1], "rd", "0101111"};
  if (InstType == "B" && Encodings.size() == 2)
    Encodings = {"off[12]",    "off[10:5]", "rs2",     "rs1",
                 Encodings[0], "off[4:1]",  "off[11]", Encodings[1]};
  if (InstType == "J" && Encodings.size() == 1)
    Encodings = {"off[20]",    "off[10:1]", "off[11]",
                 "off[19:12]", "rd",        Encodings[0]};
  if (InstType == "S" && Encodings.size() == 2)
    Encodings = {"off[11:5]",  "rs2",      "rs1",
                 Encodings[0], "off[4:0]", Encodings[1]};
  if (InstType == "U" && Encodings.size() == 1)
    Encodings = {"imm[31:12]", "rd", Encodings[0]};
  if (InstType == "C-Arith-R" && Encodings.size() == 3)
    Encodings = {Encodings[0], Encodings[1], "rd", "rs2", Encodings[2]};
  if (InstType == "C-Arith-I" && Encodings.size() == 2)
    Encodings = {Encodings[0], "imm[5]", "rd", "imm[4:0]", Encodings[1]};
  if (InstType == "C-Logic-R" && Encodings.size() == 3)
    Encodings = {Encodings[0], Encodings[1], "rd'", "rs2'", Encodings[2]};
  if (InstType == "C-Logic-I" && Encodings.size() == 3)
    Encodings = {Encodings[0], "imm[5]",   Encodings[1],
                 "rd'",        "imm[4:0]", Encodings[2]};
  if (InstType == "CSR-I" && Encodings.size() == 1)
    Encodings = {"csr", "rs1", Encodings[0], "rd", "1110011"};
  if (InstType == "CSRI-I" && Encodings.size() == 1)
    Encodings = {"csr", "zimm[4:0]", Encodings[0], "rd", "1110011"};
  if (InstType == "Shift-I" && Encodings.size() == 3)
    Encodings = {Encodings[0], "shamt", "rs1",
                 Encodings[1], "rd",    Encodings[2]};
  if (InstType == "C-LoadStore" && Encodings.size() == 2)
    Encodings = {Encodings[0], "uimm[5:3]",  "rs1'", "uimm[2]",
                 "uimm[6]",    Encodings[1], "00"};
  if (InstType == "R4" && Encodings.size() == 2)
    Encodings = {"rs3", Encodings[0], "rs2", "rs1", "rm", "rd", Encodings[1]};
  if (InstType == "FCVT-R" && Encodings.size() == 2)
    Encodings = {Encodings[0], Encodings[1], "rs1", "rm", "rd", "1010011"};
  if (InstType == "F-ARITH-R" && Encodings.size() == 1)
    Encodings = {Encodings[0], "rs2", "rs1", "rm", "rd", "1010011"};

  // Instruction
  return Instruction{Name,        Exts,    Format,   Actual,
                     Description, Columns, Encodings};
}

int main(int Argc, const char **Argv) {
  vector<string> Args(Argv, Argv + Argc);

reload:
  unordered_map<string, Instruction> Insts;

  if (Args.size() == 1)
    Args.push_back("Instructions.csv");
  ifstream InstFile(Args[1]);
  if (InstFile.fail()) {
    cerr << "ERROR: can't find file " << Args[1] << endl;
    return 1;
  }
  string Line;
  getline(InstFile, Line);
  while (getline(InstFile, Line)) {
    if (Line.size() <= 5)
      continue;
    auto Inst = emitInst(Line);
    Insts[Inst.getName()] = Inst;
  }

  string InstName;
  while (true) {
    cout << std::string(60, '=') << "\n"
         << "Enter Instruction Name:\t ";
    cin >> InstName;
    if (Insts.count(InstName))
      cout << "\n" << Insts[InstName].print() << endl;
    else
      goto reload;
  }

  return 0;
}
