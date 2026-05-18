#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

int validate_text(const char* data, const unsigned char* cmp, int sz)
{
    int i = 0;
    while (i < sz && data[i] != '\0')
    {
        if ((unsigned char)data[i] != (cmp[i] ^ 0x33))
        {
            return 0;
        }
        i++;
    }
    return i == sz && data[i] == '\0';
}

int main()
{
    struct passwd *pw = getpwuid(getuid());
    if(!pw)
    {
        return 1;
    }

    if(strcmp(pw->pw_name, "test12345") != 0) {
        printf("username is not equal to test12345 (current: %s)!\n", pw->pw_name);
        return 1;
    }

    FILE* fp = fopen("check.txt", "r");

    if (!fp)
    {
        return 1;
    }

    char buffer[128] = {0};

    fgets(buffer, sizeof(buffer), fp);
    fclose(fp);

    //KEY_12345_TEST
    unsigned char valid[] = {0x78, 0x76, 0x6a, 0x6c, 0x02, 0x01, 0x00, 0x07, 0x06, 0x6c, 0x67, 0x76, 0x60, 0x67};

    if (validate_text(buffer, valid, sizeof(valid)))
    {
        printf("====================SUCCESS!=========================\n");
    }else{
        return 1;
    }

    return 0;
}