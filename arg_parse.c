#include "arg_parse.h"
#include <stdlib.h>

static int helper(char* line){
  int numWords = 0;
  int word = 0;
  int posLine = 0;

  while(line[posLine] != '\0'){
    if((line[posLine] != ' ') && (word == 0)){
      numWords++;
      word = 1;
    }
    if(line[posLine] == ' '){
      word = 0;
    }
    posLine++;
  }
  return numWords;
}


char** arg_parse(char* line, int* argcp){
  int numWords = helper(line);
  *argcp = numWords;
  
  char** args = malloc ((numWords+1) * sizeof(char*));
  int word = 0;
  int posLine = 0;
  int currArgs = 0;

  while(line[posLine] != '\0'){
    if((line[posLine] != ' ') && (word == 0)){
      word = 1;
      args[currArgs] = &line[posLine];
      currArgs++;
    }
    if((line[posLine] == ' ') && (word == 1)){
      line[posLine] = '\0';
      word = 0;
    }
    posLine++;
  }
  args[currArgs] = '\0';

  return args;
}
