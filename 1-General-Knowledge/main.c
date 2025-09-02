#include <stdio.h>
#include "strutils.h"

void main()
{
    char str1[] = "Embedded Linux";
    printf("Truoc khi dao: %s\n", str1);
    str_reverse(str1);
    printf("Sau khi dao: %s\n", str1);

    char str2[] = "   \t  Hello Embedded Linux      \n\n";
    printf("Truoc khi trim: '%s'\n", str2);
    str_trim(str2);
    printf("Sau khi trim: '%s'\n", str2);

    const char *test[] = {"123", "-456", "+789", "42abc", "999999999999999", NULL};
    for(int i = 0; test[i] != NULL; i++)
    {
        int value;
        if(str_to_int(test[i], &value))
        {
            printf("'%s' -> %d\n", test[i], value);
        }
        else
        {
            printf("'%s' -> Loi chuyen doi\n", test[i]);
        }
    }
}
