#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "strutils.h"

void str_reverse(char *str)
{
    if (str == NULL) return;

    int left = 0;
    int right = strlen(str) - 1;

    while(left < right)
    {
        char temp = str[left];
        str[left] = str[right];
        str[right] = temp;
        left++;
        right--;
    }
}

void str_trim(char *str) 
{
    if (str == NULL) return;

    char *start = str;
    char *end;

    while (isspace((unsigned char)*start)) 
    {
        start++;
    }

    if (*start == '\0') 
    {
        *str = '\0';
        return;
    }

    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) 
    {
        end--;
    }

    *(end + 1) = '\0';

    if (start != str) 
    {
        memmove(str, start, strlen(start) + 1);
    }
}

int str_to_int(const char *str, int *out_value)
{
    if (str == NULL || out_value == NULL) return 0; 

    while (isspace((unsigned char)*str)) 
    {
        str++;
    }

    int sign = 1;
    if (*str == '+' || *str == '-') 
    {
        if (*str == '-') sign = -1;
        str++;
    }

    if (!isdigit((unsigned char)*str)) 
    {
        return 0; 
    }

    long result = 0;
    while (isdigit((unsigned char)*str)) 
    {
        int digit = *str - '0';

        if (result > (LONG_MAX - digit) / 10) 
        {
            return 0; 
        }

        result = result * 10 + digit;
        str++;
    }

    while (isspace((unsigned char)*str)) 
    {
        str++;
    }

    if (*str != '\0') 
    {
        return 0;
    }

    long signed_result = result * sign;
    if (signed_result > INT_MAX || signed_result < INT_MIN) 
    {
        return 0; 
    }

    *out_value = (int)signed_result;
    return 1; 
}