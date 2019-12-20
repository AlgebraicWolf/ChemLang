
#define KEYWORD(name) + 1

const int KEYWORDS_NUMBER =
#include "keywordlist.h"
;

#undef KEYWORD

char *keywords[] = {
#define KEYWORD(name) #name,
#include "keywordlist.h"
};

#undef KEYWORD

#define KEYWORD(name) name,

enum KEYWORDS {
#include "keywordlist.h"
};

#undef KEYWORD