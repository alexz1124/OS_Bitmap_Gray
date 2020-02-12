#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void){
    int i, tmp;
    pid_t pid[4];
    int round=0;

    for (i=0;i<4;i++) {
        pid[i] = fork();
        if (pid[i] == 0) {
            break;
        }
    }

    if (pid[0] != 0 && pid[1] != 0 && pid[2] != 0 && pid[3] != 0) {
        // That's the father, it waits for all the childs
        printf("I'm the father [pid: %d, ppid: %d]\n",getpid(),getppid());
        for(i=0;i<4;i++) {
            wait(&tmp);
        }
    } else {
        if(getpid()==getppid()+1){
             printf("I'm one of the children [pid: %d, ppid: %d] round 1\n",getpid(),getppid());
        }else if(getpid()==getppid()+2){
            printf("I'm one of the children [pid: %d, ppid: %d] round 2\n",getpid(),getppid());
        }else if(getpid()==getppid()+3){
            printf("I'm one of the children [pid: %d, ppid: %d] round 3\n",getpid(),getppid());
        }else if(getpid()==getppid()+4){
            printf("I'm one of the children [pid: %d, ppid: %d] round 4\n",getpid(),getppid());
        }
        
        // That's the child, it print its pid, its parent pid and exits.
       
    }

    return 0;
}