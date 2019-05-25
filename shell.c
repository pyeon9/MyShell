#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

#define MAX_LEN_LINE    10
#define LEN_HOSTNAME	30
#define COLOR_RED       "\x1b[31m"
#define COLOR_YELLOW    "\x1b[33m"
#define COLOR_PURPLE    "\x1b[35m"
#define COLOR_RESET     "\x1b[0m"

int main(void)
{
    char command[MAX_LEN_LINE];
    char *args[] = {command, NULL};
    char *tok[MAX_LEN_LINE];  int k;
    int ret, status;
    pid_t pid, cpid;

    char hostname[LEN_HOSTNAME + 1];
    memset(hostname, 0x00, sizeof(hostname));
    gethostname(hostname, LEN_HOSTNAME);

    while (true) {
        char *s;
	char aa[MAX_LEN_LINE];
        int len;
	
        printf(COLOR_PURPLE "%s@%s$ " COLOR_RESET, getpwuid(getuid())->pw_name,hostname);

        s = fgets(command, MAX_LEN_LINE, stdin);

        if (s == NULL) {
            fprintf(stderr, "fgets failed\n");
            exit(1);
        }
	
	else if (strcmp(s,"exit\n") == 0) {
	    break;
	}

        len = strlen(command);
        printf("%d\n", len);

        if (command[len - 1] == '\n') {
            command[len - 1] = '\0'; 
        }

        printf("[%s]\n", command);

	k = 0;
	tok[k] = strtok(args[0], "; ");
	while(tok[k]) {
	    tok[++k] = strtok(NULL, "; ");
	}	
	int n = k;

	for (k = 0; k < n; k++) {
            pid = fork();
            if (pid < 0) {
                fprintf(stderr, "fork failed\n");
                exit(1);
            } 
            if (pid != 0) {  /* parent */
                cpid = waitpid(pid, &status, 0);
                if (cpid != pid) {
                    fprintf(stderr, "waitpid failed\n");        
                }
                printf(COLOR_RESET "\nChild process terminated\n");
                if (WIFEXITED(status)) {
                    printf("Exit status is %d\n", WEXITSTATUS(status)); 
                }
            }
            else {  /* child */	        
		printf(COLOR_YELLOW "\n>>프로그램 %s 실행<<\n" COLOR_RESET, tok[k]);
                ret = execve(tok[k], args, NULL);
                if (ret < 0) {	 
                    fprintf(stderr, COLOR_RED " There is no such program! execve failed\n" COLOR_RESET);
                    return 1;
                }
            }
	} 
    }
    return 0;
}
