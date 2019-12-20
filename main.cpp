#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cctype>
#include <cstring>
#include <unistd.h>

#include "../Tree/Tree.h"

const char *input = "input.chem";
const char *output = "output.ast";

const int SPECIAL_SYMBOLS_LENGTH = 4;
const char specialSymbols[] = {'(', ')', ';', ','};

enum SPECIAL_SYMBOLS {
    left,
    right,
    semicolon,
    comma
};

#include "digits.h"

#include "keywords.h"

enum TOKEN_TYPE {
    END,
    KEYWORD,
    SPECIAL_SYMBOL,
    IDENTIFIER,
    NUMBER
};

struct token_t {
    TOKEN_TYPE type;
    int id;
};

enum NODE_TYPE {
    D,
    DEF,
    VARLIST,
    ID,
    P,
    OP,
    C,
    B,
    IF,
    WHILE,
    E,
    ASSIGN,
    VAR,
    RETURN,
    CALL,
    ARITHM_OP,
    NUM,
    INPUT,
    OUTPUT,
    EXPLODE,
    RAMEXPLODE
};

struct value_t {
    NODE_TYPE type;
    int id;
};

char **IDs = nullptr;

size_t getFilesize(FILE *f);

void parseArgs(int argc, char *argv[]);

char *loadFile(const char *filename, size_t *fileSize = nullptr);

token_t *tokenize(char *raw, size_t size, char ***ids, int *tNum, int *idNum);

node_t *getB(token_t **tokens);

tree_t *getP(token_t *tokens);

node_t *getCall(token_t **tokens);

node_t *getE(token_t **tokens);

void saveASTree(tree_t *tree, const char *filename);

char *dumpNode(void *v) {
    value_t *value = (value_t *) v;

    char *buffer = (char *) calloc(1024, sizeof(char));
    switch (value->type) {
        case D:
            sprintf(buffer, "{ DEFINITION }");
            break;
        case OP:
            sprintf(buffer, "{ OPERATION }");
            break;
        case VARLIST:
            sprintf(buffer, "{ VARLIST }");
            break;
        case ID:
            sprintf(buffer, "{ ID } | %s", IDs[value->id]);
            break;
        case C:
            sprintf(buffer, "{ BRANCHING }");
            break;
        case B:
            sprintf(buffer, "{ BLOCK }");
            break;
        case DEF:
            sprintf(buffer, "{ FUNCTION }");
            break;
        case IF:
            sprintf(buffer, "{ IF }");
            break;
        case WHILE:
            sprintf(buffer, "{ WHILE }");
            break;
        case ASSIGN:
            sprintf(buffer, "{ = }");
            break;
        case VAR:
            sprintf(buffer, "{ VAR }");
            break;
        case RETURN:
            sprintf(buffer, "{ RETURN }");
            break;
        case CALL:
            sprintf(buffer, "{ CALL }");
            break;
        case ARITHM_OP:
            switch(value->id) {
                case sourer:
                    sprintf(buffer, "{ \\< }");
                    break;

                case bitterer:
                    sprintf(buffer, "{ \\> }");
                    break;

                case justlike:
                    sprintf(buffer, "{ == }");
                    break;

                case mix:
                    sprintf(buffer, "{ * }");
                    break;

                case steal:
                    sprintf(buffer, "{ / }");
                    break;

                case add:
                    sprintf(buffer, "{ + }");
                    break;

                case filter:
                    sprintf(buffer, "{ - }");
                    break;

                case sqrt:
                    sprintf(buffer, "{ sqrt }");
                    break;
            }
            break;
        case NUM:
            sprintf(buffer, "{ INTEGER: %d }", value->id);
            break;
        case INPUT:
            sprintf(buffer, "{ INPUT }");
            break;
        case OUTPUT:
            sprintf(buffer, "{ OUTPUT }");
            break;
    }

    return buffer;
}

int main(int argc, char *argv[]) {
    parseArgs(argc, argv);
    size_t progSize = 0;
    char *raw_input = loadFile(input, &progSize);

    printf("Input filename: %s\nOutput filename: %s\n", input, output);
    char **identifiers = nullptr;

    printf("Performing text tokenizing...\n");
    int tokenNum = 0;
    int identifierNum = 0;

    token_t *tokens = tokenize(raw_input, progSize, &identifiers, &tokenNum, &identifierNum);
    printf("Found %d tokens, %d identifiers.\nList of program tokens:\n", tokenNum, identifierNum);
    for (int i = 0; i < tokenNum; i++) {
        printf("%d:\t", i);
        switch (tokens[i].type) {
            case NUMBER:
                printf("NUMBER\t\t%d", tokens[i].id);
                break;
            case IDENTIFIER:
                printf("IDENTIFIER\t%s", identifiers[tokens[i].id]);
                break;
            case KEYWORD:
                printf("KEYWORD\t\t%s", keywords[tokens[i].id]);
                break;
            case SPECIAL_SYMBOL:
                printf("SYMBOL\t\t%c", specialSymbols[tokens[i].id]);
                break;
        }
        printf("\n");
    }

    IDs = identifiers;

    tree_t *ASTree = getP(tokens);
    treeDump(ASTree, "dump.dot", dumpNode);
    saveASTree(ASTree, output);
}

void saveASNode(node_t *node, FILE *f) {
    assert(f);

    fprintf(f, "{ ");

    if(!node) {
        fprintf(f, "@ } ");
        return;
    }



    auto v = (value_t *) node->value;
    switch (v->type) {
        case D:
            fprintf(f, "DECLARATION ");
            break;
        case ID:
            fprintf(f, "%s ", IDs[v->id]);
            break;
        case NUM:
            fprintf(f, "%d ", v->id);
            break;
        case IF:
            fprintf(f, "IF ");
            break;
        case WHILE:
            fprintf(f, "WHILE ");
            break;
        case DEF:
            fprintf(f, "FUNCTION ");
            break;
        case VARLIST:
            fprintf(f, "VARLIST ");
            break;
        case OP:
            fprintf(f, "OP ");
            break;
        case ASSIGN:
            fprintf(f, "ASSIGN ");
            break;
        case RETURN:
            fprintf(f, "RETURN ");
            break;
        case VAR:
            fprintf(f, "INITIALIZE ");
            break;
        case CALL:
            fprintf(f, "CALL ");
            break;
        case INPUT:
            fprintf(f, "INPUT ");
            break;
        case OUTPUT:
            fprintf(f, "OUTPUT ");
            break;
        case P:
            fprintf(f, "PROGRAM_ROOT ");
            break;
        case C:
            fprintf(f, "C ");
            break;
        case B:
            fprintf(f, "BLOCK ");
            break;
        case EXPLODE:
            fprintf(f, "EXPLODE ");
            break;
        case RAMEXPLODE:
            fprintf(f, "RAMEXPLODE ");
            break;
        case ARITHM_OP:
            switch (v->id) {
                case sourer:
                    fprintf(f, "BELOW ");
                    break;
                case bitterer:
                    fprintf(f, "ABOVE ");
                    break;
                case justlike:
                    fprintf(f, "EQUAL ");
                    break;
                case mix:
                    fprintf(f, "MUL ");
                    break;
                case steal:
                    fprintf(f, "DIV ");
                    break;
                case add:
                    fprintf(f, "ADD ");
                    break;
                case filter:
                    fprintf(f, "SUB ");
                    break;

                case sqrt:
                    fprintf(f, "SQR ");
                    break;
            }
            break;
    }

    if (node->left || node->right) {
        saveASNode(node->left, f);
        saveASNode(node->right, f);
    }

    fprintf(f, "} ");

}

void saveASTree(tree_t *tree, const char *filename) {
    assert(tree);
    assert(filename);

    FILE *f = fopen(filename, "w");
    saveASNode(tree->head, f);
    fclose(f);

}

value_t *makeValue(NODE_TYPE type, int id) {
    auto val = (value_t *) calloc(1, (sizeof(value_t)));
    val->type = type;
    val->id = id;

    return val;
}

void parseArgs(int argc, char *argv[]) {
    int res = 0;
    while ((res = getopt(argc, argv, "i:o:")) != -1) {
        switch (res) {
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

char *skipSpaces(char *str) {
    assert(str);

    while (isspace(*str))
        str++;

    return str;
}

char *parseToken(char *raw, int *length) {
    int len = 0;
    if (isalpha(*raw) || *raw == '_') {
        while (isalpha(*(raw + len)) || *(raw + len) == '_')
            len++;

        auto token = (char *) calloc(len, sizeof(char));
        strncpy(token, raw, len);
        *length = len;
        return token;
    } else {

        for (int i = 0; i < SPECIAL_SYMBOLS_LENGTH; i++) {
            if (specialSymbols[i] == *raw) {
                auto token = (char *) calloc(len, sizeof(char));
                strncpy(token, raw, 1);
                *length = 1;
                return token;
            }
        }

        return nullptr;
    }
}

int getKeywordNum(char *token) {
    assert(token);

    for (int i = 0; i < KEYWORDS_NUMBER; i++) {
        if (strcmp(keywords[i], token) == 0) {
            return i;
        }
    }

    return -1;
}

int getSpecialSymbolNum(char *token) {
    for (int i = 0; i < SPECIAL_SYMBOLS_LENGTH; i++)
        if (*token == specialSymbols[i] && !*(token + 1))
            return i;

    return -1;
}

int parseNumber(char *token, bool *success) {
    assert(token);
    assert(success);

    *success = false;
    int parsed = 0;
    int len = 0;
    char tmp = 0;
    while (*token != '\0') {
        if (islower(*token)) return 0;
        else {
            len = 1;
            while (islower(*(token + len))) len++;
            tmp = *(token + len);
            *(token + len) = '\0';
            for (int i = 0; i < DIGITS_NUM; i++) {
                if (strcmp(token, digits[i]) == 0) {
                    parsed = parsed * DIGITS_NUM + i;
                    token += len;
                    *token = tmp;
                    len = 0;
                }
            }
            if (len)
                return 0;
        }
    }
    *success = true;
    return parsed;
}

char **findIdentifier(char **ids, char *id) {
    assert(ids);
    assert(id);

    while (*ids) {
        if (strcmp(*ids, id) == 0)
            return ids;

        ids++;
    }
    return ids;
}

void makeToken(token_t *token, TOKEN_TYPE type, int id) {
    token->type = type;
    token->id = id;
}

void addKeywordToken(token_t *tokens, int *tokenNum, int keywordID) {
    assert(tokens);
    assert(tokenNum);

    makeToken(tokens + *tokenNum, KEYWORD, keywordID);
    (*tokenNum)++;
}

void addSpecialSymbolToken(token_t *tokens, int *tokenNum, int specialSymbolID) {
    assert(tokens);
    assert(tokenNum);

    makeToken(tokens + *tokenNum, SPECIAL_SYMBOL, specialSymbolID);
    (*tokenNum)++;
}

void addIntegerToken(token_t *tokens, int *tokenNum, int integer) {
    assert(tokens);
    assert(tokenNum);

    makeToken(tokens + *tokenNum, NUMBER, integer);
    (*tokenNum)++;
}

void addIdentifierToken(token_t *tokens, char **identifiers, int *tokenNum, int *identifierNum, char *token) {
    assert(tokens);
    assert(token);
    assert(tokenNum);

    char **id = findIdentifier(identifiers, token);
    if (*id) {
        makeToken(tokens + *tokenNum, IDENTIFIER, id - identifiers);
    } else {
        int len = strlen(token);
        if (strcmp(token, "main_babka_labka") == 0) {
            *(identifiers + *identifierNum) = "main";
        }
        else {
            auto string = (char *) calloc(len, sizeof(char));
            strcpy(string, token);
            *(identifiers + *identifierNum) = string;
        }
        makeToken(tokens + *tokenNum, IDENTIFIER, *identifierNum);
        (*identifierNum)++;
    }
    (*tokenNum)++;
}

token_t *tokenize(char *raw, size_t size, char ***ids, int *tNum, int *idNum) {
    assert(raw);

    auto tokens = (token_t *) calloc(size, sizeof(token_t));
    auto identifiers = (char **) calloc(size, sizeof(char *));
    int tokenNum = 0;
    int identifierNum = 0;

    raw = skipSpaces(raw);

    int len = 0;

    while (*raw != '\0') {
        char *substring = parseToken(raw, &len);
        raw += len;
        int num = 0;
        if ((num = getKeywordNum(substring)) != -1) {
            addKeywordToken(tokens, &tokenNum, num);
        } else if ((num = getSpecialSymbolNum(substring)) != -1) {
            addSpecialSymbolToken(tokens, &tokenNum, num);
        } else {
            bool success = false;
            num = parseNumber(substring, &success);
            if (success) {
                addIntegerToken(tokens, &tokenNum, num);
            } else {
                addIdentifierToken(tokens, identifiers, &tokenNum, &identifierNum, substring);
            }
        }
        raw = skipSpaces(raw);
    }

    tokens = (token_t *) realloc(tokens, sizeof(token_t) * (tokenNum + 1));
    identifiers = (char **) realloc(identifiers, sizeof(char *) * (identifierNum + 1));

    *ids = identifiers;
    *tNum = tokenNum;
    *idNum = identifierNum;

    return tokens;
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
    if (fileSize)
        *fileSize = size;

    char *content = (char *) calloc(size, sizeof(char));
    fread(content, sizeof(char), size, input);

    fclose(input);

    return content;
}

node_t *getVarlist(token_t **tokens) {
    assert(tokens);

    value_t *headVal = makeValue(VARLIST, 0);
    node_t *head = makeNode(nullptr, nullptr, nullptr, headVal);

    node_t *current = head;

    while ((*tokens)->type == IDENTIFIER) {
        value_t *idVal = makeValue(ID, (*tokens)->id);
        value_t *varlistVal = makeValue(VARLIST, 0);

        node_t *idNode = makeNode(nullptr, nullptr, nullptr, idVal);
        node_t *varlistNode = makeNode(current, nullptr, idNode, varlistVal);
        current->left = varlistNode;

        current = varlistNode;

        (*tokens)++;

        if ((*tokens)->type == SPECIAL_SYMBOL && (*tokens)->id == comma)
            (*tokens)++;
    }

    if(head->left) {
        head = head->left;
        head->parent = nullptr;
    }

    return head;
}

node_t *getId(token_t **tokens) {
    assert(tokens);

    if ((*tokens)->type == IDENTIFIER) {
        value_t *val = makeValue(ID, (*tokens)->id);
        node_t *node = makeNode(nullptr, nullptr, nullptr, val);
        (*tokens)++;

        return node;
    }

    return nullptr;
}

node_t *getParenthesis(token_t **tokens) { // Parse parenthesis in arithmetic/logical equations (Function call, variable, parenthesis)
    assert(tokens);
    if ((*tokens)->type == SPECIAL_SYMBOL && (*tokens)->id == left) {
        (*tokens)++;
        node_t *subeq = getE(tokens);

        if (!subeq)
            return nullptr;

        if ((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != right)
            return nullptr;
        (*tokens)++;

        return subeq;
    } else if((*tokens)->type == KEYWORD) {
        if((*tokens)->id == sqrt){
            (*tokens)++;

            if (((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != left))
                return nullptr;
            (*tokens)++;

            node_t *exp= getE(tokens);
            if (!exp)
                return nullptr;

            if (((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != right))
                return nullptr;
            (*tokens)++;

            value_t *sqrtVal = makeValue(ARITHM_OP, sqrt);
            node_t *sqrtNode = makeNode(nullptr, nullptr, exp, sqrtVal);

            return sqrtNode;

        }
        else {
            return nullptr;
        }
    } else if ((*tokens)->type == IDENTIFIER) {
        if (!*(tokens + 1))
            return nullptr;

        if ((*(tokens) + 1)->type == SPECIAL_SYMBOL && (*(tokens) + 1)->id == left) {
            node_t *call = getCall(tokens);
            return call;
        } else {
            node_t *idNode = getId(tokens);
            return idNode;
        }
    } else if ((*tokens)->type == NUMBER) {
        value_t *numVal = makeValue(NUM, (*tokens)->id);
        node_t *numNode = makeNode(nullptr, nullptr, nullptr, numVal);

        (*tokens)++;

        return numNode;
    }
}

node_t *getM(token_t **tokens) { // Parse multiplication and division i. e. high priority operators
    assert(tokens);

    node_t *subtree = getParenthesis(tokens);
    if (!subtree)
        return nullptr;

    while ((*tokens)->type == KEYWORD && ((*tokens)->id == mix || (*tokens)->id == steal)) {
        value_t *arithm = makeValue(ARITHM_OP, (*tokens)->id);
        (*tokens)++;
        node_t *subtree2 = getParenthesis(tokens);
        node_t *suptree = makeNode(nullptr, subtree, subtree2, arithm);
        subtree = suptree;
    }

    return subtree;
}

node_t *getT(token_t **tokens) { // Parse addition and subtraction i. e. middle priority operators
    assert(tokens);

    node_t *subtree = getM(tokens);
    if (!subtree)
        return nullptr;

    while ((*tokens)->type == KEYWORD && ((*tokens)->id == add || (*tokens)->id == filter)) {
        value_t *arithm = makeValue(ARITHM_OP, (*tokens)->id);
        (*tokens)++;
        node_t *subtree2 = getM(tokens);
        node_t *suptree = makeNode(nullptr, subtree, subtree2, arithm);
        subtree = suptree;
    }

    return subtree;
}

node_t *getE(token_t **tokens) { // Parse logical operators i. e. lowe priority
    assert(tokens);

    node_t *subtree = getT(tokens);
    if (!subtree)
        return nullptr;

    while ((*tokens)->type == KEYWORD &&
           ((*tokens)->id == sourer || (*tokens)->id == bitterer || (*tokens)->id == justlike)) {
        value_t *arithm = makeValue(ARITHM_OP, (*tokens)->id);
        (*tokens)++;
        node_t *subtree2 = getT(tokens);
        node_t *suptree = makeNode(nullptr, subtree, subtree2, arithm);
        subtree = suptree;
    }

    return subtree;
}

node_t *getCall(token_t **tokens) {
    assert(tokens);

    node_t *id = getId(tokens);

    if (!id)
        return nullptr;

    if ((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != left)
        return nullptr;

    (*tokens)++;

    node_t *varlist = getVarlist(tokens);

    if ((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != right)
        return nullptr;

    (*tokens)++;

    value_t *callVal = makeValue(CALL, 0);
    node_t *callNode = makeNode(nullptr, id, varlist, callVal);

    return callNode;
}

node_t *getOp(token_t **tokens) {
    assert(tokens);

    if ((*tokens)->type == KEYWORD) {
        if ((*tokens)->id == testtube) {
            (*tokens)++;

            node_t *id = getId(tokens);

            if (!id)
                return nullptr;

            value_t *val = makeValue(VAR, 0);
            node_t *exp = nullptr;

            if ((*tokens)->type == KEYWORD && (*tokens)->id == is) {
                (*tokens)++;
                exp = getE(tokens);
                if (!exp)
                    return nullptr;

            }

            node_t *varNode = makeNode(nullptr, exp, id, val);

            if (((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != semicolon))
                return nullptr;

            (*tokens)++;

            return varNode;
        }
        else if ((*tokens)->id == taste) {
            (*tokens)++;

            if (((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != left))
                return nullptr;
            (*tokens)++;

            node_t *cond = getE(tokens);
            if (!cond)
                return nullptr;

            if (((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != right))
                return nullptr;
            (*tokens)++;

            node_t *ifTrue = getB(tokens);
            if (!ifTrue)
                return nullptr;

            node_t *ifFalse = nullptr;

            if ((*tokens)->type == KEYWORD && (*tokens)->id == emergencyroom) {
                (*tokens)++;
                ifFalse = getB(tokens);
                if (!ifFalse)
                    return nullptr;
            }

            value_t *altBranchesVal = makeValue(C, 0);
            node_t *altBranches = makeNode(nullptr, ifFalse, ifTrue, altBranchesVal);

            value_t *ifVal = makeValue(IF, 0);
            node_t *ifNode = makeNode(nullptr, cond, altBranches, ifVal);

            return ifNode;

        } else if ((*tokens)->id == eat) {
            (*tokens)++;

            if (((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != left))
                return nullptr;
            (*tokens)++;

            node_t *cond = getE(tokens);
            if (!cond)
                return nullptr;

            if (((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != right))
                return nullptr;
            (*tokens)++;

            node_t *repeated = getB(tokens);
            if (!repeated)
                return nullptr;

            value_t *cycleVal = makeValue(WHILE, 0);
            node_t *whileNode = makeNode(nullptr, cond, repeated, cycleVal);

            return whileNode;
        } else if ((*tokens)->id == synthesize) {
            (*tokens)++;

            node_t *exp = getE(tokens);

            if ((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != semicolon)
                return nullptr;

            (*tokens)++;

            value_t *synthVal = makeValue(RETURN, 0);
            node_t *returnNode = makeNode(nullptr, nullptr, exp, synthVal);

            return returnNode;
        } else if ((*tokens)->id == report) {
            (*tokens)++;

            node_t *id = getId(tokens);
            value_t *outputValue = makeValue(OUTPUT, 0);

            node_t *outputNode = makeNode(nullptr, nullptr, id, outputValue);

            if ((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != semicolon)
                return nullptr;

            (*tokens)++;

            return outputNode;

        } else if ((*tokens)->id == getorder) {
            (*tokens)++;

            node_t *id = getId(tokens);
            value_t *inputValue = makeValue(INPUT, 0);

            node_t *inputNode = makeNode(nullptr, nullptr, id, inputValue);

            if ((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != semicolon)
                return nullptr;

            (*tokens)++;

            return inputNode;
        } else if ((*tokens)->id == explode) {
            (*tokens)++;

            value_t *explodeValue = makeValue(EXPLODE, 0);
            node_t *explodeNode = makeNode(nullptr, nullptr, nullptr, explodeValue);

            if ((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != semicolon)
                return nullptr;

            (*tokens)++;

            return explodeNode;
        } else if ((*tokens)->id == ramexplode) {
            (*tokens)++;

            value_t *explodeValue = makeValue(RAMEXPLODE, 0);
            node_t *explodeNode = makeNode(nullptr, nullptr, nullptr, explodeValue);

            if ((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != semicolon)
                return nullptr;

            (*tokens)++;

            return explodeNode;
        }else {
            return nullptr;
        }
    } else if ((*tokens)->type == IDENTIFIER) {
        node_t *id = getId(tokens);
        if ((*tokens)->type == KEYWORD && (*tokens)->id == is) {

            (*tokens)++;

            node_t *val = getE(tokens);

            if (!val)
                return val;

            value_t *assVal = makeValue(ASSIGN, 0);
            node_t *assignNode = makeNode(nullptr, id, val, assVal);

            if ((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != semicolon)
                return nullptr;

            (*tokens)++;

            return assignNode;
        } else if ((*tokens)->type == SPECIAL_SYMBOL && (*tokens)->id == left) {
            (*tokens)++;
            node_t *arg = getVarlist(tokens);
            value_t *callVal = makeValue(CALL, 0);
            node_t *callNode = makeNode(nullptr, id, arg, callVal);

            if ((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != semicolon)
                return nullptr;

            (*tokens)++;

            return callNode;
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

node_t *getB(token_t **tokens) {
    assert(tokens);
    if ((*tokens)->type != KEYWORD || (*tokens)->id != labprotocol)
        return nullptr;

    (*tokens)++;

    value_t *bVal = makeValue(B, 0);

    node_t *top = nullptr;
    node_t *current = nullptr;

    if ((*tokens)->type != KEYWORD || (*tokens)->id != endprotocol) {
        value_t *topVal = makeValue(OP, 0);
        node_t *op = getOp(tokens);
        if (!op)
            return nullptr;

        top = makeNode(nullptr, nullptr, op, topVal);
        current = top;

        while (*tokens) {
            if ((*tokens)->type == KEYWORD && (*tokens)->id == endprotocol)
                break;

            op = getOp(tokens);
            if (!op)
                return nullptr;

            value_t *val = makeValue(OP, 0);
            node_t *node = makeNode(current, nullptr, op, val);
            current->left = node;
            current = node;
        }
    }

    if (!(*tokens))
        return nullptr;

    node_t *bNode = makeNode(nullptr, nullptr, top, bVal);

    (*tokens)++;

    return bNode;
}

node_t *getD(token_t **tokens) {
    assert(tokens);
    if ((*tokens)->type != KEYWORD || (*tokens)->id != labassistant)
        return nullptr;

    (*tokens)++;

    node_t *id = getId(tokens);
    if (!id)
        return nullptr;

    if ((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != left)
        return nullptr;

    (*tokens)++;

    node_t *varlist = getVarlist(tokens);

    if (!varlist)
        return nullptr;

    if ((*tokens)->type != SPECIAL_SYMBOL || (*tokens)->id != right)
        return nullptr;

    (*tokens)++;

    node_t *b = getB(tokens);

    if (!b)
        return nullptr;

    value_t *val = makeValue(DEF, 0);
    node_t *defNode = makeNode(nullptr, varlist, id, val);

    id->right = b;
    b->parent = id;

    value_t *d = makeValue(D, 0);
    node_t *node = makeNode(nullptr, nullptr, defNode, d);

    return node;
}

tree_t *getP(token_t *tokens) {
    assert(tokens);
    node_t *subtree1 = getD(&tokens);
    if (!subtree1)
        return nullptr;

    while (tokens->id != END) {
        node_t *subtree2 = getD(&tokens);
        subtree2->left = subtree1;
        subtree1->parent = subtree2;

        subtree1 = subtree2;
    }

    value_t *val = makeValue(P, 0);
    node_t *progroot = makeNode(nullptr, nullptr, subtree1, val);

    tree_t *tree = makeTree(nullptr);
    tree->head = progroot;

    return tree;
}