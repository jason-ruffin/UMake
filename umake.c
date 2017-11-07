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

char** charchardup(char** args){
  int i = 0;
  while (args[i] != NULL){
      i++;
  }
  char** args2 = malloc(i * sizeof *args2);

  for(int j = 0; j < i; j++)
  {
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
     list_iterator_t it;
     for (it = list_first(target_list); it; iterator_next(&it)){

      char* target_value      = (char *)iterator_target(&it);
      //char** dependency_value = (char **)iterator_dependency(&it);
      char** rule_value       = (char **)iterator_rule(&it);

      int j = 0;
      if(strcmp(argv[i], target_value) == 0){
        const char *string = "$";

        while(strcmp(rule_value[j], string) != 0){
          processline(rule_value[j]);
          j++;
        }
      }
    }

  }

  free(line);
  return EXIT_SUCCESS;
}

/* Process Line
 *
 */
void processline (char* line) {
  int argCount;
  char** args = arg_parse(line, &argCount);

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
      //printf("%s,%s",args[0],args[1]);
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
