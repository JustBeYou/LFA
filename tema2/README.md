Tema 2 - Transformare AFN-lambda in AFD

Ruleaza cu `g++ nfa.cpp -std=c++2a -o nfa; ./nfa`. Se va citi un AFN dintr-un fisier, se va parcurge pentru cateva siruri, apoi va fi convertit intr-un AFD si se vor verifica sirurile din nou. De asemenea se va afisa graful.

Algoritm:
- elimina toate legaturile lambda prin transformarea legaturilor indirecta in legaturi directe, adica daca `q0 (lambda)-> q1 -> q2,q3,...`, se va elimina legatura dintr `q0` si `q1` si se va lega `q0` la toti vecinii lui `q1` (`q2, q3, etc`)
- construieste tabelul tuturor starilor
- creeaza noi noduri pentru starile noi generate si leaga-le corespunzator
- elimina nodurile vechi care au ramas fara nicio legatura
