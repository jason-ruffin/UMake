/* CSCI 347 micro-make
 * 
 * 09 AUG 2017, Aran Clauson
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* CONSTANTS */


/* PROTOTYPES */

/* Process Line
 * line   The command line to execute.
 * This function interprets line as a command line.  It creates a new child
 * process to execute the line and waits for that process to complete. 
 */
void processline(char* line);

char** arg_parse(char* line);
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

  size_t  bufsize = 0;
  char*   line    = NULL;
  ssize_t linelen = getline(&line, &bufsize, makefile);
    

  while(-1 != linelen) {

    if(line[linelen-1]=='\n') {
      linelen -= 1;
      line[linelen] = '\0';
    }

    if(line[0] == '\t') 
      processline(&line[1]);


    linelen = getline(&line, &bufsize, makefile);
  }

  free(line);
  return EXIT_SUCCESS;
}


/* Process Line
 * 
 */
void processline (char* line) {
  char** args = arg_parse(line);

  const pid_t cpid = fork();
  switch(cpid) {

  case -1: {
    perror("fork");
    break;
  }

  case 0: {
    execvp(args[0], args);
    perror("execvp");
    exit(EXIT_FAILURE);
    break;
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
    break;
  }
  }

  free(args);
}

int helper(char* line){
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


char** arg_parse(char* line){
  int numWords = helper(line);
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
