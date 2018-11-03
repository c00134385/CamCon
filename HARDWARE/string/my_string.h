#ifndef _MY_STRING_H_
#define _MY_STRING_H_

bool isdec(char in);
bool ischar(char in);
bool isalphabet(char in);
bool isnumber(char in);
int isdecstring(const char *in);
char* Int2DecStr(int num);
unsigned int DecStr2Int(char *in, unsigned char k);

void test_arm_lib(void);

#endif

