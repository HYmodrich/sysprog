#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#define MSGSZ 1024
#define SHMSZ ((sizeof(chat_log)) * 101)
/*
* Declare the message structure.
*/

typedef struct msgbuf {
	long    mtype;
  pid_t   sender_pid;
  pid_t   receiver_pid;
	char    mtext[MSGSZ];
} message_buf;

typedef struct log {
  pid_t sender_pid;
  char message[MSGSZ];
} chat_log;

message_buf received[100];
int received_index = -1;

void* receive_message(void *server);

int main(){
	int flg = IPC_CREAT | 0644;
	key_t key;
	message_buf sbuf,rbuf;
  int server;
  char instruction;

  printf("My pid is %d\n", getpid());

  if((server = msgget((key_t)3333, flg)) < 0){
		perror("msgget error");
		exit(1);
	}

  int shmid, i;
  void *shm;
  chat_log *header;
  chat_log *lbuf;
  header = calloc(sizeof(chat_log), 1);
  lbuf = calloc(sizeof(chat_log), 1);
  key = 999;
  if((shmid = shmget(key, SHMSZ, flg)) < 0){
    perror("shmget error");
    exit(1);
  }
  if((shm = shmat(shmid, NULL, 0)) == (void *) -1){
    perror("shmat error");
    exit(1);
  }

	/*
	 * send client's pid to server for initialize
	 */
  sbuf.mtype = 3;
  sbuf.sender_pid = getpid();
  sbuf.receiver_pid = 0;
  char name[] = "client3";
  strcpy(sbuf.mtext, name);
  if(msgsnd(server, &sbuf, sizeof(message_buf) - sizeof(long), IPC_NOWAIT) < 0) {
    perror("init message send perror");
    exit(1);
  }

	pthread_t read_thread;
	if(pthread_create(&read_thread, NULL, receive_message, (void *)server) != 0){
		perror("can't create thread");
		exit(1);
	}

  do{
    printf("Please Input Command :\n");
    printf("  > c : check received messages\n");
    printf("  > s : send message to specific pid\n");
    printf("  > l : check whole chat log\n");
    printf("  > q : quit process\n");
    printf("> ");
    scanf(" %c", &instruction);
    switch(instruction){
      case 'c':
        printf("-----Received Messages-----\n");
        for(i = 0; i <= received_index; i++){
					if(received[i].sender_pid == 0) continue;
          printf("[%d] : %s", received[i].sender_pid, received[i].mtext);
				}
        printf("---------------------------\n");
				// received_index = -1;
				// received[0].sender_pid = 0;
        break;
      case 's':
        sbuf.mtype = 3;
        sbuf.sender_pid = getpid();
        printf("Please Input Receiver's Pid :\n");
        printf("  (Input 0 to broadcast)\n");
        printf("> ");
        scanf("%d", &sbuf.receiver_pid);
        printf("Please Input Message :\n");
        printf("> ");
				while(getchar() != (int)'\n');
				fgets(sbuf.mtext, MSGSZ, stdin);
        if(msgsnd(server, &sbuf, sizeof(message_buf) - sizeof(long), IPC_NOWAIT) < 0) {
          perror("message send perror");
          exit(1);
        }
        break;
      case 'l':
        memcpy(header, shm, sizeof(chat_log));
        printf("-----Chat Log-----\n");
        for(i = 0; i < header -> sender_pid; i++){
          memcpy(lbuf, shm + (i + 1) * sizeof(chat_log), sizeof(chat_log));
          printf("[%d] : %s", lbuf -> sender_pid, lbuf -> message);
        }
        printf("------------------\n");
        break;
      case 'q':
        while(getchar() != (int)'\n');
        if(shmdt(shm)){
          perror("shmdt error");
          exit(1);
        }
				pthread_cancel(read_thread);
        return EXIT_SUCCESS;
        break;
      default:
        printf("Please Input Right Command\n");
        break;
    }
  }while(1);

  exit(1);
}

void* receive_message(void *server){
	int server_ = (int)server;
	while(1){
		msgrcv(server_, &received[++received_index], sizeof(message_buf) - sizeof(long), 4, 0);
	}
}
