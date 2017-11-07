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

static int helper2(char* line){
  int numLines = 0;
  int word = 0;
  int posLine = 0;

  while(line[posLine] != '\0'){
    if((line[posLine] != '\n') && (word == 0)){
      numLines++;
      word = 1;
    }
    if(line[posLine] == ' '){
      word = 0;
    }
    posLine++;
  }
  return numLines;
}

char** arg_parse2(char* line, int* argcp){
  int numLines = helper2(line);
  *argcp = numLines;

  char** args = malloc ((numLines+1) * sizeof(char*));
  int word = 0;
  int posLine = 0;
  int currArgs = 0;

  while(line[posLine] != '\0'){
    if((line[posLine] != '\n') && (word == 0)){
      word = 1;
      args[currArgs] = &line[posLine];
      currArgs++;
    }
    if((line[posLine] == '\n') && (word == 1)){
      line[posLine] = '\0';
      word = 0;
    }
    posLine++;
  }
  const char *c = "$";
  args[currArgs] = (char*)c;

  return args;
}
