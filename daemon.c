#include <stdio.h> 
#include <string.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>

int sigterm_h = 0;                                             // Определяем SIGINT and SIGTERM для переопределенных функций.
int sigint_h = 0;

void sigterm_handler(int signum){
    sigterm_h = 1;
}

void sigint_handler(int signum)
{
    sigint_h = 1;
}

int Daemon(char** argv){
	openlog("MyDaemon", LOG_PID, LOG_DAEMON);
    signal(SIGINT, sigint_handler);								// перехват SIGINT. 
    signal(SIGTERM, sigterm_handler);							// перехват SIGTERM.
	char logText[] = "Daemon caught SIGINT\n";
	char buffer[256] = "";
	int fd_read = open(argv[1], O_CREAT|O_RDWR, S_IRWXU);       
	int count = read(fd_read, buffer, sizeof(buffer)-1);
	close(fd_read);
	buffer[count-1] = '\0';                                     
	char** res = (char**)malloc(sizeof(char) * 100);
	count = 0;
    char separator = ' ';
    char* tmp = strtok(buffer, &separator); 

	do {
		res[count++] = tmp;
	} while(tmp = strtok(NULL, &separator));
    
    while(1) {
        if(sigint_h) {
            int fd = open("log.txt", O_CREAT|O_RDWR, S_IRWXU);  			
            lseek(fd, 0, SEEK_END);											
            write(fd, logText, sizeof(logText));				            // Пишем в лог.			
            close(fd);
            syslog(LOG_NOTICE, "Daemon caught SIGINT");	
            pid_t p;
            if((p = fork()) == 0 ){											// Если демон после execv умрет, то делаю форк заранее.

                int fd_write = open("out.txt", O_CREAT|O_RDWR, S_IRWXU);
                close(1);
                dup2(fd_write, 1);											// Делаем вывод в файл, переопределяем.
                execv(res[0], res);
            }
            sigint_h = 0;
        }
        if(sigterm_h) {
            free(res);
            char logText[] = "Daemon caught SIGTERM and dead\n";
            int fd = open("log.txt", O_CREAT|O_RDWR, S_IRWXU);
            lseek(fd, 0, SEEK_END);
            write(fd, logText, sizeof(logText));

            syslog(LOG_NOTICE, "Daemon caught SIGTERM and dead\n");
			close(fd);
            exit(0);
        }

        pause();
    }
}


int main(int argc, char* argv[])
{
	pid_t pid;
	if(pid = fork() == 0)  
	{
		setsid();
		Daemon(argv);
	}
	else if(pid < 0){
		printf("Error!");
		exit(1);
	}

	return 0;
}