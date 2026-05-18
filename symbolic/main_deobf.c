#include <stdio.h>
#include <stdint.h>

int validate(char *input) {
    unsigned char a = input[0];
    unsigned char b = input[1];
    unsigned char c = input[2];
    unsigned char d = input[3];

    if (a + b != 162) {
        return 0;
    }
    if (b - c != 3) {
        return 0;
    }
    if ((c ^ d) != 26) {
        return 0;
    }

    return 1;
}

int main() {
    char input[16];

    printf("Enter key: ");
    scanf("%4s", input);

    if (validate(input)) {
        printf("success!!!\n");
    } else {
        printf("fail!!!\n");
    }

    return 0;
}
