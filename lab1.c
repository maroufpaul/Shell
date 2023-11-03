#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define HIST_SIZE 50
#define CMD_SIZE 512
#define MAX_ARGS 512
extern char **environ;
void shell_loop(char *path[], char *cmd_input[]);
void parsestring(char *path, const char *s, char *split_col[]);
void execute(char *arg0, char **argv);
void find_usn_path(char *strpath[], char *cmd[]);

int main(int argc, char **argv) {
  printf("Welcome to MyShell!\nPath:");
  char *args[MAX_ARGS]; // String array args contianing command line arguments.
  char path[CMD_SIZE];  // to store path
  char history[CMD_SIZE][HIST_SIZE]; // to store history
  strcpy(path, getenv("PATH"));      // copy the path into path string.

  char *split_colon[CMD_SIZE]; // String array holding parsed PATH.
  const char s[2] = ":";
  const char spc[2] = " "; // dilimter for parsing
  // parse the path string based on ":" and // put them into split_colon
  parsestring(path, s, split_colon);

  int i = 0;
  while (split_colon[i] != NULL) {
    printf("    %s\n     ", split_colon[i]); // print all the paths
    i++;
  }
  int index = 0;
  while (1) {
    // String containing the user input from the command line
    char cmd_input[CMD_SIZE];
    printf("\n$ ");
    fgets(cmd_input, CMD_SIZE, stdin); // get input from user
    // if newline  at the end exhange with null
    cmd_input[strlen(cmd_input) - 1] = 0;

    char new_inp[sizeof(cmd_input)];
    strcpy(new_inp, cmd_input);
    // run command from the history if !x at index x
    if (strncmp(new_inp, "!", 1) == 0) {
      int num = atoi(new_inp + 1); // if string input is number. use atoi to
                                   // change it to int
      if (num > HIST_SIZE) {       //
        continue;
      }
      // copy command at history index to input.
      // so when it runs again that is the input.
      strcpy(cmd_input, history[num]);
    }
    // copy the input in history at each index.
    strcpy(history[index], cmd_input);
    index++; // increment the index.

    // if index reaches the HIST_SIZE reset the index to zero.
    // So that it replaces the command at that index.
    if (index >= HIST_SIZE) {
      index = 0;

    }
    // if input is history print the history
    else if (strcmp(cmd_input, "history") == 0) {
      int i = 0;
      while (i < HIST_SIZE) {
        int next = (i + index) % HIST_SIZE;
        i++;
        if (strlen(history[next]) < 1) {
          continue;
        }
        printf("%d", HIST_SIZE);
        printf("[%d]: %s\n", next, history[next]);
      }
    }

    if (strlen(cmd_input) <= 1) { // if no input print no command specified
      printf("no command specified");
      continue;
    }

    // exit if the input is "exit"
    else if (strcmp(cmd_input, "exit") == 0) {
      break;
    }
    // parse input on based of space and put them in args
    parsestring(cmd_input, spc, args);

    shell_loop(split_colon, args);
    memset(args, 0, sizeof(args)); // reset args
  }
  return 0;
}

void find_usn_path(char *path[], char *cmd[]) {
  int i = 0;

  while (path[i] != NULL) {
    char path_spl[CMD_SIZE] = {};
    strcpy(path_spl, path[i]); // copy path into path_spl
    strcat(path_spl, "/");     // concatenate "/"
    strcat(path_spl, cmd[0]);  // concatenate the command

    int rv = 0;
    rv = access(path_spl, X_OK); // check if command is executble
    if (rv == 0) {
      execute(path_spl, cmd); // execute the command
    }
    i++;
  }
}
void execute(char *arg0, char **arg) {
  pid_t pid = fork(); // fork the process

  if (pid < 0) { /* fork fails          */
    printf("ERROR: forking child process failed\n");
    exit(1);
  } else if (pid == 0) {                  // for the child process:
                                          // printf("arg[0]: %s", arg[0]);
    if (execve(arg0, arg, environ) < 0) { /* execute the command  */
      printf("ERROR: exec failed\n");
      exit(1);
    }
  } else { /* for the parent:      */
    wait(NULL);
    /* wait for completion  */
  }
}
void parsestring(char *path, const char *s, char *split_col[]) {

  // Returns first token
  char *tok = strtok(path, s);
  int i = 0;
  // Keep adding tokens to split_col at each index
  // while one of the delimiters present in path.
  while (tok != NULL) {
    split_col[i] = tok;
    tok = strtok(NULL, s);
    i++;
  }
}

void shell_loop(char *path[], char *cmd_input[]) {
  char dest[3];
  // char *curr_dir;
  // curr_dir = cwd();
  strncpy(dest, cmd_input[0], 2);
  dest[2] = '\0';
  // printf("\n%s", dest);
  if (strcmp(dest, "./") == 0) { // check for relative commands
    execute(cmd_input[0], cmd_input);
  }
  if (strcmp(cmd_input[0], "cd") == 0) { // in chdir to execute the "somthing"
    char *x = cmd_input[1];
    int rv = chdir(x); // using the command
    if (rv != 0) {
      perror("cd ");
    }
  } else {
    find_usn_path(path, cmd_input);
  }
}
