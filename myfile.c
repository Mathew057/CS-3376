#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

char *getinput(char *buffer, size_t buflen) {
	printf("$$ ");
	return fgets(buffer, buflen, stdin);
}

int main(int argc, char **argv) {
	char buf[1024];
	pid_t pid;
	int status;

	while (getinput(buf, sizeof(buf))) {
		buf[strlen(buf) - 1] = '\0';
		if(strcmp(buf, "exit\0") == 0) {
			printf("Exiting loop\n");
			break;
		}


		if((pid=fork()) == -1) {
			fprintf(stderr, "shell: can't fork: %s\n", strerror(errno));
			continue;
		} else if (pid == 0) {
			/* child process to do each command 
    			place your code here to handle read, write, append */
			char* token = strtok(buf, "|");
			while (token) {
				char* command = strtok(token, " ");
				strtok(NULL," ");
				char* file = strtok(NULL, " ");
				if (strcmp(command, "read") == 0) {
					FILE* userfile = fopen(file, "rb");
					FILE* tempfile = fopen("temp.txt", "wb");
					
					fseek(userfile, 0, SEEK_END);
					long size = ftell(userfile);
					rewind(userfile);

					char *string = malloc(size + 1);
					fread(string, 1, size, userfile);
	
					fclose(userfile);
					fclose(tempfile);
				}	
				else if (strcmp(command, "write") == 0) {
					FILE* userfile = fopen(file, "wb");
					FILE* tempfile = fopen("temp.txt", "rb");
					
					fseek(tempfile, 0, SEEK_END);
					long size = ftell(tempfile);
					rewind(tempfile);
					fseek(tempfile, 0, SEEK_SET);

					char *string = malloc(size + 1);
					fread(string, 1, size, tempfile);
					fwrite(string, 1, size, userfile);
					
					fclose(userfile);
					fclose(tempfile);
				}	
				else if (strcmp(command, "append") == 0) {
					FILE* userfile = fopen(file, "ab");
					FILE* tempfile = fopen("temp.txt", "rb");
					
					fseek(tempfile, 0, SEEK_END);
					long size = ftell(tempfile);
					rewind(tempfile);

					char *string = malloc(size + 1);
					fread(string, 1, size, tempfile);
					fwrite(string, 1, size, userfile);

					fclose(userfile);
					fclose(tempfile);
				}
				token = strtok(buf, "|");	
			}

			exit(EX_OK);
		}

		if ((pid=waitpid(pid, &status, 0)) < 0)
			fprintf(stderr, "shell: waitpid error: %s\n",
					strerror(errno));
	}
	exit(EX_OK);
}

