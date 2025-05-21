#include <stdio.h>
#include <stdlib.h>

typedef unsigned char* pointer;

void print_bit(pointer a, int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (a[i] >> j) & 1);
        }
        printf(" ");
    }
    printf("\n");
}

// Problem 2
void reverse_bit(pointer a, int len) {
    for (int i = 0; i < len / 2; i++) {
        unsigned char temp = a[i];
        a[i] = a[len - 1 - i];
        a[len - 1 - i] = temp;
    }
    for (int i = 0; i < len; i++) {
        unsigned char temp = a[i];
        a[i] = 0;
        for (int j = 0; j < 8; j++) {
            a[i] |= ((temp >> j) & 1) << (7 - j);
        }
    }
}

// Problem 3
void split_bit(pointer a, pointer out1, pointer out2, int len) {
    for (int i = 0; i < len; i +=2 ) {
        int k = i + 1;
        int divi = i / 2;
        int oddcounter = 0;
        int evencounter = 0;
        for (int j = 0; j < 8; j++) {
            if (j != 0 && j % 2 != 0) {
                out1[divi] |= ((a[k] >> j) & 1) << oddcounter;
                oddcounter++;
            }
            else {
                out2[divi] |= ((a[k] >> j) & 1) << evencounter;
                evencounter++;
            }
        }
        for (int j = 0; j < 8; j++) {
            if (j != 0 && j % 2 != 0) {
                out1[divi] |= ((a[i] >> j) & 1) << oddcounter;
                oddcounter++;
            }
            else {
                out2[divi] |= ((a[i] >> j) & 1) << evencounter;
                evencounter++;
            }    
        }
    }
}

// Problem 4
unsigned int convert_endian(unsigned int a) {
    return ((a >> 24) & 0xFF) |
        ((a << 8) & 0xFF0000) |
        ((a >> 8) & 0xFF00) |
        ((a << 24) & 0xFF000000);
}

// Problem 5
void get_date(unsigned int date, int* pYear, int* pMonth, int* pDay) {
    *pYear = date >> 9;        
    *pMonth = (date >> 5) & 0xF; 
    *pDay = date & 0x1F;        
}

int main() {
    // You don't need to touch the main function
    printf("Problem 1\n");
    unsigned int v1 = 0x1234CDEF;
    print_bit((pointer)&v1, sizeof(v1));
    reverse_bit((pointer)&v1, sizeof(v1));
    print_bit((pointer)&v1, sizeof(v1));

    printf("Problem 2\n");
    unsigned int v2 = 0x1234CDEF;
    unsigned short out1 = 0, out2 = 0;
    print_bit((pointer)&v2, sizeof(v2));
    split_bit((pointer)&v2, (pointer)&out1, (pointer)&out2, sizeof(v2));
    print_bit((pointer)&out1, sizeof(out1));
    print_bit((pointer)&out2, sizeof(out2));

    printf("Problem 3\n");
    unsigned int v4 = 0x12345678;
    unsigned int v4_ret = convert_endian(v4);
    print_bit((pointer)&v4, sizeof(v4));
    print_bit((pointer)&v4_ret, sizeof(v4_ret));

    printf("Problem 4\n");
    unsigned int date = 1035391;
    int year, month, day;
    print_bit((pointer)&date, sizeof(date));
    get_date(date, &year, &month, &day);
    printf("%d -> %d/%d/%d\n", date, year, month, day);

    return 0;
}