#include <stdio.h>
#include <stdlib.h>

int compare(const char *, const char *);

int main(int argc, char **argv) {
    if (argc != 3) {
        puts("Usage: diff <etalon file> <compared file>\n");
        return 1;
    }
    return compare(argv[1], argv[2]);
}

int compare(const char *file1, const char *file2) {
    int numchar1 = 0;
    int numstr1 = 1;
    int numchar2 = 0;
    int numstr2 = 1;
    int res = 0;
    FILE *fin;
    FILE *fout;
    int ch1, ch2;

    fin = fopen(file1, "rb");
    fout = fopen(file2, "rb");
    if (fin == NULL) {
        printf("Can't open file %s\n", file1);
        exit(1);
    }
    if (fout == NULL) {
        printf("Can't open file %s\n", file2);
        exit(1);
    }
    while (1) {
        if (feof(fin) && feof(fout))
            break;
        if (feof(fin) || feof(fout)) {
            res=1;
            break;
        }
        while (1) {
            ch1 = getc(fin);
            numchar1++;
            if (ch1 == ' ' || ch1 == '\r')
                continue;
            if (ch1 == '\n') {
                numstr1++;
                numchar1 = 0;
                continue;
            }
            break;
        }
        while (1) {
            ch2 = getc(fout);
            numchar2++;
            if (ch2 == ' ' || ch2 == '\r')
                continue;
            if (ch2 == '\n') {
                numstr2++;
                numchar2 = 0;
                continue;
            }
            break;
        }
        if (ch1 != ch2) {
            res = 1;
            break;
        }
    }
    if (res) {
        printf("=== File \"%s\" is not equal to file \"%s\"\n"
               "    str=%d char %d ('%c' '0x%x') in file \"%s\"\n"
               "    str=%d char %d ('%c' '0x%x') in file \"%s\"\n"
               "===\n", file1, file2,
                numstr1, numchar1, ch1, ch1, file1,
                numstr2, numchar2, ch2, ch2, file2);
    }
    fclose(fin);
    fclose(fout);
    return res;
}
