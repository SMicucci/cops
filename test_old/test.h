#ifndef TEST_H
#define TEST_H

#define D "\x1b[0m"
#define B "\x1b[1m"
#define I "\x1b[3m"
#define U "\x1b[4m"
#define S "\x1b[9m"

#define Rf "\x1b[31m"
#define Gf "\x1b[32m"
#define Yf "\x1b[33m"
#define Bf "\x1b[34m"
#define Pf "\x1b[35m"
#define Cf "\x1b[36m"
#define Rb "\x1b[41m"
#define Gb "\x1b[42m"
#define Yb "\x1b[43m"
#define Bb "\x1b[44m"
#define Pb "\x1b[45m"
#define Cb "\x1b[46m"

static inline int pow_(int base, int exp)
{
        int res = 1;
        for (int i = 0; i < exp; i++) {
                res *= base;
        }
        return res;
}

static inline void format_number(unsigned int n, char res[28])
{
        int dept = 0, tmp = n;
        while (tmp > 9) {
                tmp = tmp / 10;
                dept++;
        }
        char *c = res;
        tmp = n;
        for (int i = dept; i >= 0; i--) {
                int p = pow_(10, i);
                int val = tmp / p;
                *c++ = (char)(val + 48);
                tmp -= val * p;
                if (!(i % 3) && i)
                        *c++ = '\'';
        }
        *c = '\0';
}

#endif /* end of include guard: TEST_H */
