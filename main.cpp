#include <iostream>
#include "lex_automata.hpp"

int main(int, char**) {
    std::cout << "Hello, world!\n";

    LexAutomata lex("pupa.txt");
    std::vector<LexToken> out = {};
    lex.scanTokens(&out);
}
