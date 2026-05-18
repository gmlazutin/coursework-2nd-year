#include <stdio.h>
#include <stdint.h>

#define OPAQUE_TRUE(x)  ((((uint32_t)(x) * ((uint32_t)(x) + 1u)) & 1u) == 0u)
#define OPAQUE_FALSE(x) ((((uint32_t)(x) * ((uint32_t)(x) + 1u)) & 1u) != 0u)

int validate(char *input) {
    unsigned char a = input[0];
    unsigned char b = input[1];
    unsigned char c = input[2];
    unsigned char d = input[3];

    int state2 = 0;
    int result = 0;
    int state1 = 0x100;

    while (1) {
        switch (state1) {
            case 0x100:
                if (OPAQUE_TRUE(a ^ b)) {
                    state1 = 0x200;
                } else {
                    state2 += 0x55;
                    state1 = 0x111;
                }
                break;

            case 0x200:
                if ((a + b) - 162 == 0) {
                    state1 = 0x300;
                } else {
                    state1 = 0x111;
                }
                break;

            case 0x300:
                if (OPAQUE_FALSE(b + c)) {
                    state2 ^= 0x1234;
                    state1 = 0x200+0x11+0x10+0x1;
                } else {
                    state1 = 0x400;
                }
                break;

            case 0x400:
                if ((b - c) - 2 == 1) {
                    state1 = 0x500;
                } else {
                    return 0;
                }
                break;

            case 0x500:
                if (OPAQUE_TRUE(c ^ d)) {
                    state1 = 0x600;
                } else {
                    state2 -= 77;
                    state1 = 0x111;
                }
                break;

            case 0x600:
                if ((c ^ d) - 6 == 20) {
                    state1 = 0x222;
                } else {
                    state1 = 0x64+0xAD;
                }
                break;

            case 0x700:
                return 1;
            case 0x222:
                result = 1;
                if (OPAQUE_FALSE(state2 ^ state1)) {
                    return 0;
                }
                state1 = 0xFFF;
                break;

            case 0x111:
                result = 0;
                state1 = 0xFFF;
                if (OPAQUE_FALSE(state2 + 0x76)) {
                    return 1;
                }
                break;
            case 0xFFF:
                if (OPAQUE_FALSE(state1 + 0x12)) {
                    return 1;
                }
                return result;

            default:
                return 0;
        }
    }
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
