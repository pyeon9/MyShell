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
#define COLOR_RED       "\x1b[31m" // 빨간색
#define COLOR_YELLOW    "\x1b[33m" // 노란색
#define COLOR_PURPLE    "\x1b[35m" // 보라색
#define COLOR_RESET     "\x1b[0m"  // 색 초기화

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
	
        printf(COLOR_PURPLE "%s@%s$ " COLOR_RESET, getpwuid(getuid())->pw_name,hostname); // username과 hostname을 보라색으로 출력

        s = fgets(command, MAX_LEN_LINE, stdin);

        if (s == NULL) {
            fprintf(stderr, "fgets failed\n");
            exit(1);
        }

	else if (!strcmp(s,"exit\n")) { // exit 입력시 shell 종료
	    break;
	}

	else if (!strcmp(s,"clear\n")) { // clear 구현
	    system("clear");
	    continue;
	}

	else if (!strcmp(s,"\n")) { // 그냥 enter만 입력 시 아래 수행하지 않음
	    continue;
	}

        len = strlen(command);
        printf("%d\n", len);

        if (command[len - 1] == '\n') {
            command[len - 1] = '\0'; 
        }

        printf("[%s]\n", command);

	k = 0;
	tok[k] = strtok(args[0], "; "); // 공백(space bar입력)과 ; 을 구분자로 하여 입력 command 쪼개기
	while(tok[k]) {
	    tok[++k] = strtok(NULL, "; ");
	}	
	int n = k;

	for (k = 0; k < n; k++) { // 여러 프로그램 실행 명령시 여러 프로그램 실행
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
		printf(COLOR_YELLOW "\n>>프로그램 %s 실행<<\n" COLOR_RESET, tok[k]); // 실행할 프로그램 이름 출력(노란색)
                ret = execve(tok[k], args, NULL);
                if (ret < 0) {	 
                    fprintf(stderr, COLOR_RED " There is no such program! execve failed\n" COLOR_RESET); // 해당 프로그램이 없는 경우 안내 메세지 출력(빨간색)
                    return 1;
                }
            }
	} 
    }
    return 0;
}
