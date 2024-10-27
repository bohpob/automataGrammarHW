# Assignment: Determinization of Finite Automata

The algorithm for determinizing finite automata is essential in 
formal language theory, with applications in text processing, 
data analysis, and various computer science fields. 
It converts nondeterministic finite automata (NFA) to deterministic 
finite automata (DFA), enabling efficient language analysis, 
which is crucial in compilers, text analysis, regular expression searching,
network management, and database systems.

## Task: 
Implement a function in C++ with the signature: `DFA determinize(const MISNFA &nfa);` 
The function should take an NFA with multiple initial states as input and output a DFA 
without unnecessary or unreachable states, maintaining language equivalence.

## Requirements:
* Input NFA structures (MISNFA) must have non-empty state, initial state, and alphabet sets.
* The resulting DFA must adhere to the same conditions as the NFA.
* If the automaton's language is empty, return a single-state automaton over the same alphabet.
