#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <set>
#include <fstream>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <utility>

#ifdef DEBUG
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)   
#endif

using namespace std;

typedef unsigned int uint;
const char lambda = '~';

struct AutomatonNode {
    map<char, list<string>> characterTransition;
    bool isFinal;
    bool isStart;
};

typedef map<string, AutomatonNode> AutomatonGraph;

class NFA {
    private:
        AutomatonGraph transitions;
        set<string> startingNodes;

        bool checkIfValid(string &input, uint inputIndex, string& currentNode) {
            DEBUG_ONLY(cout << "node: " << currentNode << " " << input[inputIndex] << endl;)
            bool isAnyValid = false;
            // empty transition
            if (transitions[currentNode].characterTransition.find(lambda) !=
                transitions[currentNode].characterTransition.end()) {
                
                auto& nextNodeList = this->transitions[currentNode].characterTransition[lambda];
                for (auto& nextNode: nextNodeList) {
                    DEBUG_ONLY(cout << "lambda next: " << nextNode << endl;)
                    isAnyValid = isAnyValid or checkIfValid(input, inputIndex, nextNode);
                }
            }

            if (inputIndex == input.size()) {
                return this->checkFinal(currentNode) or isAnyValid;
            }

            char currentChar = input[inputIndex];
            
            if (transitions[currentNode].characterTransition.find(currentChar) ==
                transitions[currentNode].characterTransition.end()) {
                    return isAnyValid or false;
            }

            auto& nextNodeList = this->transitions[currentNode].characterTransition[currentChar];
            for (auto& nextNode: nextNodeList) {
                DEBUG_ONLY(cout << "next node: " << nextNode << endl;)
                isAnyValid = isAnyValid or checkIfValid(input, inputIndex + 1, nextNode);
            }

            return isAnyValid;    
        }

        bool checkFinal(string& currentNode) {
            DEBUG_ONLY(cout << "check final " << currentNode << " " << this->transitions[currentNode].isFinal << endl;)
            return this->transitions[currentNode].isFinal;
        }

        void filterLambdas(string& currentNode, map<string, bool>& visited) {
            visited[currentNode] = true;

            for (auto& edge: this->transitions[currentNode].characterTransition) {
                auto& nextNodeList = edge.second;
                for (auto& nextNode: nextNodeList) {
                    if (not visited[nextNode]) {
                        filterLambdas(nextNode, visited);
                    }
                }
            }

            if (this->transitions[currentNode].characterTransition.find(lambda) != 
                this->transitions[currentNode].characterTransition.end()) {
                
                bool setStartingPoint = this->transitions[currentNode].isStart;

                auto& nextNodeList = this->transitions[currentNode].characterTransition[lambda];
                for (auto& nextNode: nextNodeList) {
                    if (this->transitions[nextNode].isFinal) {
                        this->transitions[currentNode].isFinal = true;
                    }
                    if (setStartingPoint) {
                        this->transitions[nextNode].isStart = true;
                        this->startingNodes.insert(nextNode);
                    }

                    auto& secondGradeTransitions = this->transitions[nextNode].characterTransition;
                    for (auto& secondGradeTransition: secondGradeTransitions) {
                        auto& secondGradeCharacter = secondGradeTransition.first;
                        auto& secondGradeNodeList = secondGradeTransition.second;

                        if (secondGradeCharacter == lambda) continue;
                        // merge
                        this->transitions[currentNode].characterTransition[secondGradeCharacter].insert(
                            this->transitions[currentNode].characterTransition[secondGradeCharacter].end(),
                            secondGradeNodeList.begin(), secondGradeNodeList.end()
                        );
                    }
                }

                DEBUG_ONLY(cout << "Delete lambda of " << currentNode << endl;) 
                this->transitions[currentNode].characterTransition.erase(lambda);
            }
        }

    public:
        NFA(AutomatonGraph &transitions, string& startNode) {
            this->transitions = transitions;
            this->startingNodes.insert(startNode);
        }

        bool check(string &input) {
            bool anyValidPath = false;
            for (auto startNode: this->startingNodes) {
                anyValidPath = anyValidPath or this->checkIfValid(input, 0, startNode);
            }
            return anyValidPath;
        }

        void eliminateLambda() {
            map<string, bool> visited;

            for (auto startNode: this->startingNodes) {
                filterLambdas(startNode, visited);
            }
        }

        friend ostream& operator<<(ostream &out, NFA& automaton);
};

ostream& operator<<(ostream &out, NFA& automaton) {
    out << "Automaton" << endl;
    map<string, bool> visited;
    queue<string> nodes;

    for (auto& startNode: automaton.startingNodes) {
        nodes.push(startNode);
    }

    while (not nodes.empty()) {
        auto node = nodes.front();
        nodes.pop();

        if (visited[node]) continue;
        visited[node] = true;

        bool isStart = automaton.transitions[node].isStart;
        bool isFinal = automaton.transitions[node].isFinal;
        out << "Node(" << (isStart ? "S" : "") << (isFinal ? "F" : "") << "): " << node << endl;
        for (auto& transition: automaton.transitions[node].characterTransition) {
            auto& character = transition.first;
            auto& nextNodeList = transition.second;
            out << character << ": ";
            for (auto& nextNode: nextNodeList) {
                nodes.push(nextNode);
                out << nextNode << " ";
            }
            out << endl;
        }
    }

    return out;
}

class InputReader {
    public:
        static pair<AutomatonGraph, string> readFromStream(istream &stream) {
            AutomatonGraph transitions;
            string startNode;
            uint transitionsCount;
            stream >> transitionsCount;

            for (uint i = 0; i < transitionsCount; i++) {
                string sourceNode;
                uint edgesCount;
                bool isFinal;
                stream >> sourceNode >> isFinal >> edgesCount;

                if (i == 0) {
                    startNode = sourceNode;                    
                }

                transitions[sourceNode].isFinal = isFinal;
                transitions[sourceNode].isStart = false;

                for (uint j = 0; j < edgesCount; j++) {
                    string destinationNode;
                    char inputChar;
                    stream >> inputChar >> destinationNode;
                    transitions[sourceNode].characterTransition[inputChar].push_back(destinationNode);                    
                }
            }

            transitions[startNode].isStart = true;
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
    auto startNode = parsedInput.second;
    
    NFA automaton(transitions, startNode);

    string s = "aa";
    cout << s << " : " << automaton.check(s) << endl;
    s = "ab";
    cout << s << " : " << automaton.check(s) << endl;
    s = "b";
    cout << s << " : " << automaton.check(s) << endl;
    s = "ba";
    cout << s << " : " << automaton.check(s) << endl;
    s = "bb";
    cout << s << " : " << automaton.check(s) << endl;
    s = "aac";
    cout << s << " : " << automaton.check(s) << endl;
    s = "bc";
    cout << s << " : " << automaton.check(s) << endl;
    cout << automaton << endl;

    automaton.eliminateLambda();

    cout << "---------------" << endl;
    s = "aa";
    cout << s << " : " << automaton.check(s) << endl;
    s = "ab";
    cout << s << " : " << automaton.check(s) << endl;
    s = "b";
    cout << s << " : " << automaton.check(s) << endl;
    s = "ba";
    cout << s << " : " << automaton.check(s) << endl;
    s = "bb";
    cout << s << " : " << automaton.check(s) << endl;
    s = "aac";
    cout << s << " : " << automaton.check(s) << endl;
    s = "bc";
    cout << s << " : " << automaton.check(s) << endl;
    cout << automaton << endl;

    return 0;
}