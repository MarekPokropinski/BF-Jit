#include <iostream>
#include <cstdio>
#include <unordered_map>
using namespace std;

const char OPEN_LOOP = '[';
const char CLOSE_LOOP = ']';

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

    // printf("Brackets:\n");
    // for (auto &b : brackets)
    // {
    //     printf("%d:%d", b.first, b.second);
    //     printf("%c %c\n", code[b.first], code[b.second]);
    // }
    // printf("\n");
    return brackets;
}

void interpret(const char code[], size_t size)
{
    char memory[30000];
    char *ptr = memory;

    unordered_map<size_t, size_t> matched_brackets = match_brackets(code, size);
    unordered_map<size_t, size_t> loop_jumpbacks;

    for (auto key_val : matched_brackets)
    {
        loop_jumpbacks[key_val.second] = key_val.first;
    }

    for (size_t ip = 0; ip < size; ++ip)
    {
        switch (code[ip])
        {
        case '>':
            ++ptr;
            break;
        case '<':
            --ptr;
            break;
        case '+':
            ++(*ptr);
            break;
        case '-':
            --(*ptr);
            break;
        case '.':
            putchar(*ptr);
            break;
        case ',':
            *ptr = getchar();
            break;
        case '[':
            if (!*ptr)
            {
                // if condition is not met jump after closing bracket.
                ip = matched_brackets[ip];
            }
            break;
        case ']':
            ip = loop_jumpbacks[ip] - 1;
            break;

        default:
            break;
        }
    }
}

const char code1[] = "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.";

int main()
{
    const char filename[] = "hello_world.bf";
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        printf("File not found: %s\n", filename);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* code = new char[filesize+1];
    fread(code, 1, filesize, fp);
    code[filesize] = '\0';
    interpret(code, filesize);
}