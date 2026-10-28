import time
import concurrent.futures as con
import sys

# Brainfuck Error
class BrainfuckError (Exception):
    pass


# Pointer Class
class Pointer:

    def __init__ (self):
        self.value: int = 0

    def incr (self):
        self.value += 1

    def decr (self):
        if self.value > 0:
            self.value -= 1
        else:
            raise ValueError("Pointer indices must be positive")

    def setValue (self, n):
        if isinstance(n, int):
            self.value = n
        elif isinstance(n, str) and len(n) == 1:
            self.value = ord(n)
        else:
            raise BrainfuckError("Invalid Pointer indices")

    def show (self):
        return chr(self.value)


# Bracket Exception
class NoMatchingPartnerBracket (Exception):
    pass

def get_matching_end_bracket (string: str, start_index: int):
    assert string[start_index] == '['
    depth = 1

    for index in range(start_index+1, len(string)):
        if string[index] == '[':
            depth += 1
        elif string[index] == ']':
            depth -= 1
        else:
            continue
        
        if depth == 0: return index

    raise NoMatchingPartnerBracket()


# Brainfuck Compiler
class Brainfuck:
    valid_operatores = ['>', '<', '+', '-', '.', ',', '[', ']']

    def __init__ (self, code: str):
        self.raw_code = code
        self.processed_code = self.process(self.filter_code(code))
        self.pointers = [Pointer()]
        self.current_ptr = 0

    @classmethod
    def filter_code (cls, code: str):
        return ''.join(c for c in filter((lambda n: n in cls.valid_operatores), code))

    def decode (self):
        result = ""
        for exp in self.processed_code:
            r = self.decode_expression(exp)
            if r is not None:
                result += r
            time.sleep(0.02)
        return result

    def decode_expression (self, exp):
        if exp == '>':
            self.incr_ptr()
        elif exp == '<':
            self.decr_ptr()
        elif exp == '+':
            self.pointers[self.current_ptr].incr()
        elif exp == '-':
            self.pointers[self.current_ptr].decr()
        elif exp == '.':
            return self.pointers[self.current_ptr].show()
        elif exp == ',':
            self.pointers[self.current_ptr].setValue(input('? '))
        elif isinstance(exp, list):
            loop = ""
            for i in exp:
                sub = self.decode_expression(i)
                if sub is not None:
                    loop += sub
            if self.pointers[self.current_ptr].value > 0:
                return loop + self.decode_expression(exp)
            else:
                return loop

    @classmethod
    def process (cls, code: str):
        operatores = []
        i = 0
        while i < len(code):
            if code[i] == '[':
                end = get_matching_end_bracket(code, i)
                operatores.append(cls.process(code[i+1:end]))
                i = end + 1
            elif code[i] in cls.valid_operatores:
                operatores.append(code[i])
                i += 1
            else:
                i += 1
        return operatores

    def incr_ptr (self):
        self.current_ptr += 1
        if self.current_ptr == len(self.pointers):
            self.pointers.append(Pointer())

    def decr_ptr (self):
        if self.current_ptr > 0:
            self.current_ptr -= 1
        else:
            raise BrainfuckError()


def loading (text: str = "Loading"):
    for i in ['|', '/', '-', '\\']:
        print("\r{} [{}]".format(text, i), end='')
        time.sleep(0.1)


if __name__ == "__main__":
    file = open(sys.argv[1], 'r') if len(sys.argv) > 1 else sys.stdin

    with file as f:
        b = Brainfuck(f.read())
    
    print("Load File ...")
    with con.ThreadPoolExecutor() as exc:
        thread = exc.submit(b.decode)
        while not thread.done():
            loading()
        print('\r' + thread.result())
