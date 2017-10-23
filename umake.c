/* CSCI 347 micro-make
 *
 * 09 AUG 2017, Aran Clauson
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "arg_parse.h"
#include "target.h"
/* CONSTANTS */


/* PROTOTYPES */

/* Process Line
 * line   The command line to execute.
 * This function interprets line as a command line.  It creates a new child
 * process to execute the line and waits for that process to complete.
 */
void processline(char* line);

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


  while(-1 != linelen) {
    int colonPos = 0;
    int boolRules = 0;

    //create the temp data to be put into the linked list
    char* tempTarget = NULL;
    char* tempRule = NULL;
    char* tempDependency = NULL;


    if(line[linelen-1]=='\n') {
      linelen -= 1;
      line[linelen] = '\0';
    }

    //if the line doesnt start with a tab
    if(line[0] != '\t'){
      //AND if the line just finished parseing rules, then add the current target, dependencies and rules
      if(boolRules == 1){
        list_append(target_list, tempTarget, tempDependency, tempRule);
        boolRules = 0;
      }

      //iterate through the line until the colon. Everything before is the target
      for(int i=0; line[i] != ':'; i++){
        tempTarget[i] = line[i];
        colonPos++;
      }

      //everything after the colon is the dependencies
      for(int i=colonPos+1; line[i] == '\0'; i++){
        tempDependency[i] = line[i];
      }
    }

    //if the line started with a tab and was in a target, then add it to the rules
    if(line[0] == '\t'){
      boolRules = 1;
      tempRule = &line[1];
    }

    linelen = getline(&line, &bufsize, makefile);
  }

/*
  //now take the command line arguments and compare them to the target_list in our linked list
  for(int i = 1; i < argc; i++){
    while(target_list != NULL){
      //if there is a match, run the rules associated with that target
      if(strcmp(argv[i], target_list -> first -> target) == 0 ){
        processline(target_list -> first -> rules);
      }else{
        printf("There is no target named %s in uMakefile", argv[i]);
      }
      //iterate to the next node in the list
      target_list = target_list -> first -> next;
    }

  }
  */
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
    return;
  }

  case 0: {
    execvp(args[0], args);
    perror("execvp");
    exit(EXIT_FAILURE);
    free(args);
    return;
  }

  default: {
    int   status;
    const pid_t pid = wait(&status);
    if(-1 == pid) {
      perror("wait");
    }
    else if (pid != cpid) {
      fprintf(stderr, "wait: expected process %d, but waited for process %d",
              cpid, pid);
    }
    free(args);
    return;
  }
  }

  free(args);
  return;
}
