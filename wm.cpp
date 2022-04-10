#include <string.h>
#include <iostream>
#define PATTERN_NUM 5
#define MAX_BLOCK_NUM 100 //字符块个数
#define MAX_HASH_VALUE 0xFFF
#define DEFAULT_VALUE 255
#define MAX_PATTERN_LEN 100
using namespace std;

typedef struct listnode
{
    uint8_t ptr; //在pattern_ptr中的下标
    struct listnode *next;
} listnode, *NodeList;

char *pattern_ptr[PATTERN_NUM] = {"thinner", "shinning", "church", "touching", "thinking"};
uint8_t Shift[MAX_HASH_VALUE];
NodeList Hash[MAX_HASH_VALUE];
uint32_t prefix[PATTERN_NUM];
uint8_t winsize;

char *text = "Tom is thinking how to be thinner.";

uint32_t calhash(const char *, int);
uint8_t calshift(const char *, const char *);
bool Compare(char *, char *);
void preprocess();
void WM();

int main()
{
    preprocess();
    WM();
}
uint32_t calhash(const char *str, int len)
{
    if (str == NULL)
        return -1;
    uint32_t var = 0;
    for (int i = 0; i < len; i++)
        var = *(str + i) + (var << 6) + (var << 16) - var;
    return (var & MAX_HASH_VALUE);
}
void preprocess()
{
    // init
    memset(Hash, NULL, sizeof(Hash));
    memset(prefix, -1, sizeof(prefix));
    // winsize
    uint32_t minsize = (uint32_t)-1;
    for (int i = 0; i < PATTERN_NUM; i++)
        if ((uint32_t)strlen(pattern_ptr[i]) < minsize)
            minsize = strlen(pattern_ptr[i]);
    winsize = minsize;

    memset(Shift, winsize - 1, sizeof(Shift));

    // shift
    for (int i = 0; i < PATTERN_NUM; i++)
    {
        for (int j = 0; j < winsize - 1; j++)
        {
            const char *substr = (pattern_ptr[i] + j);
            if (Shift[calhash(substr, 2)] == winsize - 1)
            {
                int rightestpos = -1;
                //计算shift值
                for (int ii = 0; ii < PATTERN_NUM; ii++)
                {
                    uint8_t pos = calshift(substr, pattern_ptr[ii]);
                    if (pos > MAX_PATTERN_LEN)
                        continue;
                    if ((int)pos > rightestpos)
                        rightestpos = pos;
                }
                Shift[calhash(substr, 2)] = (rightestpos < 0) ? (winsize - 1) : (winsize - 2 - rightestpos);
            }
        }
        // Hash
        uint32_t suffixhashval = calhash(pattern_ptr[i] + winsize - 2, 2);
        listnode *node = (listnode *)malloc(sizeof(listnode));
        node->ptr = i;
        node->next = NULL;
        listnode *np = Hash[suffixhashval];
        if (np == NULL)
            Hash[suffixhashval] = node;
        else
        {
            while (np->next != NULL)
                np = np->next;
            np->next = node;
        }

        // prefix
        prefix[i] = calhash(pattern_ptr[i], 2);
    }
}
uint8_t calshift(const char *substr, const char *pattern)
{
    int pos = -1;
    for (int i = 0; i < winsize - 1; i++)
    {
        if (substr[0] == pattern[i])
            if (substr[1] == pattern[i + 1])
                pos = i;
    }
    return pos;
}

void WM()
{
    int pos = winsize - 1;
    int textlen = strlen(text);
    while (pos < textlen)
    {
        char *winsuffixpos = text + pos - 1;
        char *winpreffixpos = text + pos - winsize + 1;
        uint32_t winsuffixhash = calhash(winsuffixpos, 2);
        uint32_t winprefixhash = calhash(winpreffixpos, 2);
        if (Shift[winsuffixhash] == 0)
        {
            listnode *p = Hash[winsuffixhash];
            while (p != NULL)
            {
                if (winprefixhash == prefix[p->ptr])
                    if (Compare(pattern_ptr[p->ptr], winpreffixpos))
                        cout << "'" << string(pattern_ptr[p->ptr]) << "' at text pos: " << pos - winsize + 1 << endl;
                p = p->next;
            }
            pos += 1;
        }
        else
        {
            pos += Shift[winsuffixhash];
        }
    }
}

bool Compare(char *Pattern, char *Text)
{
    for (int i = 0; i < strlen(Pattern); i++)
    {
        if (Pattern[i] != Text[i])
            return false;
    }
    return true;
}