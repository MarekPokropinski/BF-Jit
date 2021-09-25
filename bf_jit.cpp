#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <vector>
// #define ASMJIT_STATIC
#include <asmjit/x86.h>

using namespace std;
using namespace asmjit;

const char OPEN_LOOP = '[';
const char CLOSE_LOOP = ']';

/*
Wrap putchar and getchar in case they are macros
*/
void _putchar(uint8_t c)
{
    putchar(c);
}

uint8_t _getchar()
{
    return getchar();
}

unordered_map<size_t, size_t> match_brackets(const char code[], size_t size)
{
    unordered_map<size_t, size_t> brackets;
    unordered_map<size_t, size_t> counts;

    for (size_t i = 0; i < size; i++)
    {
        if (code[i] == OPEN_LOOP)
        {
            for (auto &cnt : counts)
            {
                counts[cnt.first] += 1;
            }
            counts[i] = 1;
        }
        if (code[i] == CLOSE_LOOP)
        {
            size_t to_close = -1;
            for (auto &cnt : counts)
            {
                counts[cnt.first] -= 1;
                if (counts[cnt.first] == 0)
                {
                    to_close = cnt.first;
                }
            }
            counts.erase(to_close);
            brackets[to_close] = i;
        }
    }

    return brackets;
}

void jit(const char code[], size_t size, x86::Assembler &assm, char *memory)
{
    unordered_map<size_t, size_t> matched_brackets = match_brackets(code, size);
    unordered_map<size_t, size_t> loop_jumpbacks;
    unordered_map<size_t, Label> labels;

    x86::Gpq ptr = x86::r12;
    assm.mov(ptr, memory);

    for (auto key_val : matched_brackets)
    {
        loop_jumpbacks[key_val.second] = key_val.first;
    }

    for (size_t ip = 0; ip < size; ++ip)
    {
        switch (code[ip])
        {
        case '>':
            assm.inc(ptr);
            break;
        case '<':
            assm.dec(ptr);
            break;
        case '+':
            assm.inc(x86::byte_ptr(ptr));
            break;
        case '-':
            assm.dec(x86::byte_ptr(ptr));
            break;
        case '.':
            assm.xor_(x86::rcx, x86::rcx);
            assm.mov(x86::cl, x86::byte_ptr(ptr));
            assm.call(putchar);
            break;
        case ',':
            assm.call(getchar);
            assm.mov(x86::byte_ptr(ptr), x86::al);
            break;
        case '[':
        {
            Label openLabel = assm.newLabel();
            Label closeLabel = assm.newLabel();
            labels[ip] = openLabel;
            labels[matched_brackets[ip]] = closeLabel;
            assm.bind(openLabel);
            assm.cmp(asmjit::x86::byte_ptr(ptr), 0);
            assm.je(closeLabel);
        }
        break;
        case ']':
        {
            Label openLabel = labels[loop_jumpbacks[ip]];
            Label closeLabel = labels[ip];
            assm.jmp(openLabel);
            assm.bind(closeLabel);
        }

        break;
        default:
            break;
        }
    }
    assm.ret();
}

vector<char> load_code(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        string exception = "File not found: ";
        exception += filename;
        throw exception;
    }
    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    vector<char> code(filesize);
    fread(code.data(), 1, filesize, fp);
    return code;
}

typedef int (*Func)(void);
typedef int (*Func_charptr)(const char *);


int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    const char * filename = argv[1];
    vector<char> program_code;
    try
    {
        program_code = load_code(filename);
    }
    catch (string s)
    {
        cout << "Error: " << s << endl;
        return -1;
    }

    JitRuntime rt;
    // FileLogger logger(stdout);
    CodeHolder code;
    code.init(rt.environment());
    // code.setLogger(&logger);
    x86::Assembler a(&code);

    char *memory = new char[3000];
    
    memset(memory, 0, 3000);
    jit(program_code.data(), program_code.size(), a, memory);    

    Func fn;
    Error err = rt.add(&fn, &code);
    if (err)
    {
        printf("Error in jit\n");
        return 1;
    }

    fn();

    rt.release(fn);
    return 0;
}
