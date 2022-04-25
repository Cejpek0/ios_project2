#include <stdio.h>
#include <stdlib.h>

int checkArguments(int argc, char **arguments, unsigned long* NO, unsigned long* NH, unsigned int* TI, unsigned int* TB) {
  if(argc == 5) {
        char* error;
        *NO = strtoul(arguments[1], &error,10);
        if (*error != '\0') return 0; // Not number
        *NH = strtoul(arguments[2], &error,10);
        if (*error != '\0') return 0; // Not number
        *TI = strtoul(arguments[3], &error,10);
        if (*error != '\0') return 0; // Not number
        *TB = strtoul(arguments[4], &error,10);
        if (*error != '\0') return 0; // Not number
        
        if(arguments[1][0] == '-' || arguments[1][0] == '\0') return 0; // Not < 0 && not empty
        if(arguments[2][0] == '-' || arguments[2][0] == '\0') return 0; // Not < 0 && not empty
        if(arguments[3][0] == '-' || arguments[3][0] == '\0') return 0; // Not < 0 && not empty
        if(arguments[4][0] == '-' || arguments[4][0] == '\0') return 0; // Not < 0 && not empty
        if(*TI > 1000) return 0;
        if(*TB > 1000) return 0;
        if(*NO == 0) return 0;
        if(*NH == 0) return 0;
  } else {
      return 0;
  }
  return 1;
}