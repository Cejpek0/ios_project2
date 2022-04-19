#include <stdio.h>
#include <stdlib.h>

int checkArguments(int argc, char **arguments, unsigned long* NO, unsigned long* NH, unsigned int* TI, unsigned int* TB) {
  if(argc == 5) {
        char* error;
        *NO = strtoul(arguments[1], &error,10);
        printf("%s", error);
        if (*error != '\0') return 0;
        *NH = strtoul(arguments[2], &error,10);
        if (*error != '\0') return 0;
        *TI = strtoul(arguments[3], &error,10);
        if (*error != '\0') return 0;
        *TB = strtoul(arguments[4], &error,10);
        if (*error != '\0') return 0;
        if(*TI > 1000) return 0;
        if(*
        TB > 1000) return 0;
  } else {
      return 0;
  }
  return 1;
}