/* CSCI 347 micro-make
 *
 * 09 AUG 2017, Aran Clauson
 */

#include "target.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "arg_parse.h"

/* CONSTANTS */

/* PROTOTYPES */

/* Process Line
 * line   The command line to execute.
 * This function interprets line as a command line.  It creates a new child
 * process to execute the line and waits for that process to complete.
 */
void processline(char* line);

void runargs(list_t* list, char* targetName){
char* target_value = NULL;
    char** dependency_value = NULL;
    char** rule_value = NULL;
    list_iterator_t it;
    for (it = list_first(list); it; iterator_next(&it)){
      target_value     = (char *)iterator_target(&it);
      dependency_value = (char **)iterator_dependency(&it);
      rule_value       = (char **)iterator_rule(&it);
      if(strcmp(targetName, target_value) == 0){

        int i = 0;
        while(dependency_value != NULL && dependency_value[i]!=NULL){
          runargs(list, dependency_value[i]);
          i++;
        }

        int j = 0;
        while(rule_value != NULL && rule_value[j]!= NULL){
          processline(strdup(rule_value[j]));
          j++;
        }
      }
    }
}


int expand(char* orig, char* new, int newsize){

  char* copy = (char*)malloc(100);

  copy = strdup(orig);

  int y = 0;
  int x = 0;
  int returnval = 0;
  for(int i = 0; copy[i] != '\0'; i++) {
    if(copy[i] == '$'){
      returnval = 1;
      y = i;
      memcpy(new, &orig[0], y);
    }

    if(copy[i] == '}'){
      x = i;
      copy[x] = '\0';
    }
  }

  if(returnval == 1){
    strcat(new, getenv(&copy[y+2]));
    strcat(new, &orig[x+1]);
  }

  return returnval;
}

char** charchardup(char** args){
  int i = 0;
  const char *string2 = "|||";
  while (args[i] != NULL && strcmp(args[i], string2) != 0){
      i++;
  }

  char** args2 = (char**)malloc(100 * sizeof(char*));

  for(int j = 0; j < i; j++){
      args2[j] = strdup(args[j]);
  }

  return args2;
}
/* Main entry point.
 * argc    A count of command-line arguments
 * argv    The command-line argument valus
 *
 * Micro-make (umake) reads from the uMakefile in the current working
 * directory.  The file is read one line at a time.  Lines with a leading tab
 * character ('\t') are interpreted as a command and passed to processline minus
 * the leading tab.
 */

int main(int argc, const char* argv[]) {

  FILE* makefile = fopen("./uMakefile", "r");

  list_t* target_list = list_new();

  size_t  bufsize = 0;
  char*   line    = NULL;
  ssize_t linelen = getline(&line, &bufsize, makefile);

  char* tempTarget           = (char*)malloc(100);
  char* tempDependency       = (char*)malloc(100);
  char* tempRule             = (char*)malloc(100);
  char** tempDependencyfinal = (char**)malloc(100 * sizeof(char*));
  char** tempRulefinal       = (char**)malloc(100 * sizeof(char*));

  int boolRules = 0;
  int boolTarget = 0;

  while(-1 != linelen) {
    int colonPos = 0;
    int equalPos = 0;

    if(line[linelen-1]=='\n') {
      linelen -= 1;
      line[linelen] = '\0';
    }

    //if the line doesnt start with a tab
    if(line[0] != '\t'){
      //AND if the line just finished parseing rules or a target, then add to the targetl
      if(boolTarget == 1 || boolRules == 1){

        int dependencyNum;
        tempDependencyfinal = arg_parse(strdup(tempDependency),&dependencyNum);

        int ruleNum;
        tempRulefinal = arg_parse2(strdup(tempRule),&ruleNum);
        list_append(target_list, strdup(tempTarget), charchardup(tempDependencyfinal), charchardup(tempRulefinal));
        tempTarget          = realloc(tempTarget, 100);
        tempDependency      = realloc(tempDependency, 100);
        tempRule            = realloc(tempRule, 1000);
        tempDependencyfinal = realloc(tempDependencyfinal, 100 * sizeof(char*));
        tempRulefinal       = realloc(tempRulefinal, 100 * sizeof(char*));
      }

      int enviroBool = 0;
      int targetbool = 0;

      for(int i = 0; line[i] != '\n'; i++){
        if(line[i] == ':'){
          targetbool = 1;
          break;
        }
        if(line[i] == '='){
          enviroBool = 1;
          break;
        }
      }

      if(targetbool == 1){
        boolTarget = 1;
        boolRules  = 0;

        //iterate through the line until the colon. this will be used for separating the rules from the depedencies
        for(int i=0; line[i] != ':'; i++){
          colonPos++;
        }

        //creating the substrings target and dependency from the line
        memcpy(tempTarget, &line[0], colonPos);
        tempTarget[colonPos] = '\0';

        memcpy(tempDependency, &line[colonPos + 1], (linelen-colonPos+1));
        tempDependency[linelen-colonPos] = '\0';
      }
      if(enviroBool == 1){

        char* tempName  = (char*)malloc(100);
        char* tempValue = (char*)malloc(100);

        for(int i=0; line[i] != '='; i++){
          equalPos++;
        }

        //creating the substrings target and dependency from the line
        memcpy(tempName, &line[0], equalPos);
        tempName[equalPos] = '\0';

        memcpy(tempValue, &line[equalPos + 1], (linelen-equalPos+1));
        tempValue[linelen-equalPos] = '\0';

        setenv(strdup(tempName),strdup(tempValue),1);
      }

    }else{
    //if the line started with a tab and was in a target, then add it to the rules
      if(boolRules ==1){
        boolTarget = 0;
        char temp[100];
	      memcpy(temp, &line[1], 100);
        strcat(temp, "\0");
        strcat(tempRule, "\n");
        strcat(tempRule, temp);
      }else{
        boolRules = 1;
        boolTarget = 0;
        memcpy(tempRule, &line[1], 50);
        strcat(tempRule, "\0");

      }
   }
    linelen = getline(&line, &bufsize, makefile);
  }

  int dependencyNum;
  tempDependencyfinal = arg_parse(strdup(tempDependency),&dependencyNum);

  int ruleNum;
  tempRulefinal = arg_parse2(strdup(tempRule),&ruleNum);

  list_append(target_list, strdup(tempTarget), charchardup(tempDependencyfinal), charchardup(tempRulefinal));

  //now take the command line arguments and compare them to the target_list in our linked list

  for(int i = 1; i < argc; i++){
    char* temp = strdup(argv[i]);
    runargs(target_list, temp);
  }

  free(line);
  return EXIT_SUCCESS;
}

/* Process Line
 *
 */

void processline (char* line) {
  int argCount;
  char* newline = (char*)malloc(100);
  int enviroexpand = 0;
  enviroexpand = expand(line, newline, 0);
  char** args = NULL;
  if(enviroexpand == 1){
    args = arg_parse(newline, &argCount);
  }
  if(enviroexpand == 0){
    args = arg_parse(line, &argCount);
  }

  if(args[0] == NULL){
    free(args);
    return;
  }

  const pid_t cpid = fork();
  switch(cpid) {

    case -1: {
      perror("fork");
      free(args);
      break;
    }

    case 0: {
      execvp(args[0], args);
      perror("execvp");
      free(args);
      exit(EXIT_FAILURE);
      break;
    }

    default: {
      free(args);
      int   status;
      const pid_t pid = wait(&status);
      if(-1 == pid) {
        perror("wait");
      }
      else if (pid != cpid) {
        fprintf(stderr, "wait: expected process %d, but waited for process %d",
                cpid, pid);
      }
      break;
    }
  }
}
