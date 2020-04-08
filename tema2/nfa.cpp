#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cstddef>
#include <cstdint>

using namespace std;

typedef unsigned int uint;
const char lambda = '~';

struct AutomatonNode {
    map<char, vector<char>> characterTransition;
    bool isFinal;
};

typedef map<char, AutomatonNode> AutomatonGraph;

class NFA {
    private:
        AutomatonGraph transitions;
        char startNode;

        bool checkIfValid(string &input, uint inputIndex, char currentNode) {
            //cout << "node: " << currentNode << " " << input[inputIndex] << endl;
            bool isAnyValid = false;
            // empty transition
            if (transitions[currentNode].characterTransition.find(lambda) !=
                transitions[currentNode].characterTransition.end()) {
                
                auto nextNodeList = this->transitions[currentNode].characterTransition[lambda];
                for (char nextNode: nextNodeList) {
                    //cout << "lambda next: " << nextNode << endl;
                    isAnyValid = isAnyValid or checkIfValid(input, inputIndex, nextNode);
                }
            }

            if (inputIndex == input.size()) {
                return this->checkFinal(currentNode) or isAnyValid;
            }

            char currentChar = input[inputIndex];
            
            if (transitions[currentNode].characterTransition.find(currentChar) ==
                transitions[currentNode].characterTransition.end()) {
                    return false;
            }

            auto nextNodeList = this->transitions[currentNode].characterTransition[currentChar];
            for (char nextNode: nextNodeList) {
                //cout << "next node: " << nextNode << endl;
                isAnyValid = isAnyValid or checkIfValid(input, inputIndex + 1, nextNode);
            }

            return isAnyValid;    
        }

        bool checkFinal(char currentNode) {
            return this->transitions[currentNode].isFinal;
        }

    public:
        NFA(AutomatonGraph &transitions, char startNode) {
            this->transitions = transitions;
            this->startNode = startNode;
        }

        bool check(string &input) {
            return this->checkIfValid(input, 0, this->startNode);
        }
};

class InputReader {
    public:
        static pair<AutomatonGraph, char> readFromStream(istream &stream) {
            AutomatonGraph transitions;
            char startNode;
            uint transitionsCount;
            stream >> transitionsCount;

            for (uint i = 0; i < transitionsCount; i++) {
                char sourceNode;
                uint edgesCount;
                bool isFinal;
                stream >> sourceNode >> isFinal >> edgesCount;

                if (i == 0) {
                    startNode = sourceNode;                    
                }

                transitions[sourceNode].isFinal = isFinal;

                for (uint j = 0; j < edgesCount; j++) {
                    char destinationNode;
                    char inputChar;
                    stream >> inputChar >> destinationNode;
                    transitions[sourceNode].characterTransition[inputChar].push_back(destinationNode);                    
                }
            }

            return {transitions, startNode};
        }
};

int main() {
    //ifstream inputFile("automaton_det.txt");
    
    /*ifstream inputFile("automaton_not_det.txt");
    auto parsedInput = InputReader::readFromStream(inputFile);
    AutomatonGraph transitions = parsedInput.first;
    char startNode = parsedInput.second;
    
    NFA automaton(transitions, startNode);

    string s1 = "acacd";
    string s2 = "acacaddd";
    string s3 = "acad";
    string s4 = "abac";*/

    /*string s1 = "bbabaaa";
    string s2 = "bbbabb";*/
    
    /*cout << automaton.check(s1) << endl;
    cout << automaton.check(s2) << endl;   
    cout << automaton.check(s3) << endl;
    cout << automaton.check(s4) << endl;*/

    ifstream inputFile("automaton_lambda.txt");
    auto parsedInput = InputReader::readFromStream(inputFile);
    AutomatonGraph transitions = parsedInput.first;
    char startNode = parsedInput.second;
    
    NFA automaton(transitions, startNode);

    string s = "aa";
    cout << automaton.check(s) << endl;
    s = "ab";
    cout << automaton.check(s) << endl;
    s = "b";
    cout << automaton.check(s) << endl;
    s = "ba";
    cout << automaton.check(s) << endl;
    s = "bb";
    cout << automaton.check(s) << endl;

    return 0;
}