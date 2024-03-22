#include "dfa.h"
#include "lexio.h"
#include "nfa.h"
#include "gtest/gtest.h"
#include <iostream>
// TEST(lex, lexcpp) {
int main() {
  try {
    InOut io{"c.lex", "Lexxy.cpp"};
    io.getCodeBlocks();
    io.getAllMacros();
    io.printMacros();
    io.getAllExprs();
    io.printRAs();
    io.replaceAllRegWithMacros();

    NfaStates nfas{io};
    nfas.printRAs();
    nfas.thompson();
    printNfa(nfas);

    DfaStates dfas{};
    dfas.setNfaArrays(nfas.getNfaArrays(), nfas.getNfaSize());
    dfas.setCmd(nfas.getCmd());
    printNfa(dfas);
    dfas.makeDfa();
    dfas.min_dfa();

    // io.printRAs();
  } catch (const std::exception &e) {
    std::cout << e.what();
  }
}
