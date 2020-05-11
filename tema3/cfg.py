from re import split

EPSILON = '~'
START = 'S'

class State:
    def __init__(self, tokens, minLen):
        self.tokens = tokens
        self.minLen = minLen

    def __str__(self):
        return ' '.join(self.tokens)

    def __repr__(self):
        return self.__str__()

class CFG:
    variables = set()
    rules = {}

    def generate(self, maxLen = 2, start = START):
        queue = []
        result = []
        queue.append(State([start], 0))

        while len(queue) != 0:
            currentState = queue.pop(0)

            isTerminal = True
            for i, k in enumerate(currentState.tokens):
                if k in self.variables:
                    isTerminal = False

                    prefix  = currentState.tokens[:i]
                    sufix   = currentState.tokens[i+1:]

                    for product in self.rules[k]:
                        if product.minLen + currentState.minLen > maxLen: continue                        
                        newState = State(prefix + product.tokens + sufix, currentState.minLen + product.minLen)
                        queue.append(newState)
                else:
                    pass

            if isTerminal:
                result.append(''.join(currentState.tokens).replace(EPSILON, ''))

        return result

    def finalize(self):
        for key in self.rules:
            for rule in self.rules[key]:
                rule.finalize(self.variables)

    def __str__(self):
        s = 'vars: {}\n'.format(self.variables)
        for k in self.rules:
            s += '{}: {}\n'.format(
                k,
                ' | '.join([str(x) for x in self.rules[k]])
            )
        return s
    
    def __repr__(self):
        return self.__str__()

class Product:
    def __init__(self, string):
        self.string = string

    def finalize(self, variables):
        tokenizerRule = '([{}])'.format(''.join(list(variables) + [EPSILON]))
        self.tokens = [x for x in split(tokenizerRule, self.string) if x != '']
        self.minLen = 0
        for token in self.tokens:
            if token not in variables and token != EPSILON:
                self.minLen += len(token)

    def __str__(self):
        return self.string

    def __repr__(self):
        return self.__str__()

def main():
    cfg = CFG()

    with open('input.txt') as inputFile:
        for line in inputFile:
            key, result  = [x.strip() for x in line.split('->')]
            results = [x.strip() for x in result.split('|')]

            cfg.variables.add(key)

            if key not in cfg.rules:
                cfg.rules[key] = []
            cfg.rules[key].extend([Product(x) for x in results])
    cfg.finalize()

    print (cfg)

    strings = cfg.generate(3)
    print (strings)
    strings = cfg.generate(4)
    print (strings)

if __name__ == "__main__":
    main()