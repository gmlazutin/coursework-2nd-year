#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>

#define GET_HASH(sig) ((uint32_t)((sig) >> 32))
#define GET_MARKER(sig) ((uint32_t)((sig) & 0xFFFFFFFFu))
#define SET_SIG(hash, marker) ((((uint64_t)(hash)) << 32) | ((uint32_t)(marker)))

int integrity_check(uint64_t counter) {
    return ((uint64_t)__rdtsc() - counter) > 5000;
}

uint64_t generate_signature(const char* input)
{
    uint64_t start = __rdtsc();
    uint32_t hash = 5381;

    while (*input)
    {
        hash = ((hash << 5) + hash) + (unsigned char)(*input);
        input++;
    }

    uint32_t marker;
    if (integrity_check(start))
    {
        marker = hash ^ 0xDEADBEEF;
    }
    else
    {
        marker = (~hash) ^ 0xDEADBEEF;
    }

    return SET_SIG(hash, marker);
}

int verify_signature(uint64_t sig)
{
    uint32_t hash = GET_HASH(sig);
    uint32_t marker = GET_MARKER(sig);

    uint32_t decoded = marker ^ 0xDEADBEEF;

    if (decoded == hash)
    {
        return 1;
    }

    if ((~decoded) == hash)
    {
        return 0;
    }

    return -1;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("usage: %s <input>\n", argv[0]);
        return 0;
    }

    uint64_t sig = generate_signature(argv[1]);

    printf("hash: %016lX\n", sig);

    printf("===VERIFYING THE GENERATED SIGNATURE===\n");

    int result = verify_signature(sig);
    if (result == 1)
    {
        printf("debugger detected during the signing process!!!\n");
    }
    else if (result == 0)
    {
        printf("generated normally\n");
    }
    else
    {
        printf("invalid signature!!!\n");
    }

    return 0;
}