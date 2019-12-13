#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <unistd.h>

const char *input = "input.txt";
const char *output = "output.txt";

size_t getFilesize(FILE *f);

void parseArgs(int argc, char *argv[]);

char *loadFile(const char *filename, size_t *fileSize = nullptr);

enum TOKEN_TYPE {

};

struct token_t {
    TOKEN_TYPE type;
    void *value;
};

int main(int argc, char *argv[]) {
    parseArgs(argc, argv);
    size_t progSize = 0;
    char *raw_input = loadFile(input, &progSize);

    printf("Input filename: %s\nOutput filename: %s\n", input, output);

}

void parseArgs(int argc, char *argv[]) {
    int res = 0;
    while((res = getopt(argc, argv, "i:o:")) != -1) {
        switch(res) {
            case 'i':
                input = optarg;
                break;
            case 'o':
                output = optarg;
                break;
            case '?':
                printf("Invalid argument found!\n");
                break;
        }
    }
}

size_t getFilesize(FILE *f) {
    assert(f);

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    return size;
}

char *loadFile(const char *filename, size_t *fileSize) {
    assert(filename);

    FILE *input = fopen(filename, "r");

    size_t size = getFilesize(input);
    if(fileSize)
        *fileSize = size;

    char *content = (char *) calloc(size, sizeof(char));
    fread(content, sizeof(char), size, input);

    fclose(input);

    return content;
}