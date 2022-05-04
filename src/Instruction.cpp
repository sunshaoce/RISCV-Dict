#include "Instruction.h"

// #define DEBUG

using namespace std;

string Instruction::printChart() {

  int Bits = 0;
  for (auto B : Columns)
    Bits += B;
  unsigned N = 2; // Single Column Width

adjust: // auto adjust Single Column Width
  string HorizontalLine((Columns.size() + Bits) * N + 1, '-');

  string Tens, Ones;
  string NewTens;
  string NewOnes;

  if (Bits == 16) {
    Tens = "111111          ";
    Ones = "5432109876543210";
  }
  else if (Bits == 32) {
    Tens = "3322222222221111111111          ";
    Ones = "10987654321098765432109876543210";
  } else {
    return "\t...";
  }
  for (int I = 0, Sum = 0; I != Columns.size(); ++I) {
    Tens.insert(Sum, "|");
    Ones.insert(Sum, "|");
    Sum += Columns[I] + 1;
  }
  for (char C : Tens)
    NewTens += string{C} + string(N - 1, ' ');
  NewTens += "|";

  for (char C : Ones)
    NewOnes += string{C} + string(N - 1, ' ');
  NewOnes += "|";

  string EncodingLine = "|";
  for (unsigned I = 0; I != Encodings.size(); ++I) {
    unsigned MaxCharNum = Columns[I] * N + N - 1;
    unsigned EncodingCharNum = Encodings[I].size();
    if (MaxCharNum < EncodingCharNum) {
      N += 1;
      goto adjust;
    }
    unsigned PrefixSpaceNum = (MaxCharNum - EncodingCharNum) / 2;
    EncodingLine +=
        string(PrefixSpaceNum, ' ') + Encodings[I] +
        string(PrefixSpaceNum + ((MaxCharNum - EncodingCharNum) % 2 ? 1 : 0),
               ' ') +
        "|";
  }

  string Res = string{"\t"} + HorizontalLine + "\n\t" + NewTens + "\n\t" +
               NewOnes + "\n\t" + HorizontalLine + "\n\t" + EncodingLine +
               "\n\t" + HorizontalLine;
#ifdef DEBUG
  string EL;
  for (unsigned I = 1; I != EncodingLine.size() - 1; ++I) {
    char Ch = EncodingLine[I];
    if (Ch == '|')
      EL += ' ';
    else if (Ch != ' ')
      EL += Ch;
  }

  Res += "\n\n{" + EL + "}\n";
#endif //! DEBUG
  return Res;
}
