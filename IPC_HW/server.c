#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#define MSGSZ 1024
#define SHMSZ (sizeof(chat_log) * 101)

/*
 * Declare the message structure.
 */

typedef struct msgbuf {
	long    mtype;
  pid_t   sender_pid;
  pid_t   receiver_pid;
	char    mtext[MSGSZ];
} message_buf;

/*
 * Declare the chat log structure.
 */

typedef struct log {
  pid_t sender_pid;
  char message[MSGSZ];
} chat_log;

int main(){
	int flg = IPC_CREAT | 0666;
	message_buf sbuf,rbuf;
	size_t buf_length;
  chat_log chat;

  int shmid,i;
  void *shm;

	/*
	 * header of chat log / shared memory
	 * use sender_pid field for the numbers of chat_log
	 */
  chat_log *header;
  header = calloc(sizeof(chat_log), 1);

  if((shmid = shmget((key_t)999, (size_t)SHMSZ, flg)) < 0){
    perror("shmget error");
    exit(1);
  }
  if((shm = shmat(shmid, NULL, 0)) == (void *) -1){
    perror("shmat error");
    exit(1);
  }

  header = shm;
  header -> sender_pid = 0;
  memcpy(shm, header, sizeof(chat_log));

  int client1, client2, client3;

  if((client1 = msgget((key_t)1111, flg)) < 0){
		perror("msgget error");
		exit(1);
	}
  if((client2 = msgget((key_t)2222, flg)) < 0){
		perror("msgget error");
		exit(1);
	}
  if((client3 = msgget((key_t)3333, flg)) < 0){
		perror("msgget error");
		exit(1);
	}

	/*
	 * get pid from each client
	 */
  pid_t client1_pid, client2_pid, client3_pid;

  msgrcv(client1, &rbuf, sizeof(message_buf) - sizeof(long), 1, 0);
  client1_pid = rbuf.sender_pid;
  printf("client1 : %d\n", client1_pid);

  msgrcv(client2, &rbuf, sizeof(message_buf) - sizeof(long), 2, 0);
  client2_pid = rbuf.sender_pid;
  printf("client2 : %d\n", client2_pid);

  msgrcv(client3, &rbuf, sizeof(message_buf) - sizeof(long), 3, 0);
  client3_pid = rbuf.sender_pid;
  printf("client3 : %d\n", client3_pid);

	while(1){
		/*
		 * check message queue for client1
		 */
    while(1){
      if(msgrcv(client1, &rbuf, sizeof(message_buf) - sizeof(long),
				1, IPC_NOWAIT) < 0){
        break;
    	}
			/* broadcast */
      if(rbuf.receiver_pid == 0){
        chat.sender_pid = rbuf.sender_pid;
				memset(chat.message, '\0', MSGSZ);
        strncpy(chat.message, rbuf.mtext, MSGSZ);
        memcpy(shm + (header -> sender_pid + 1) * sizeof(chat_log), &chat,
					sizeof(chat_log));
        header -> sender_pid++;
        memcpy(shm, header, sizeof(chat_log));
        printf("[%d] : %s", rbuf.sender_pid, rbuf.mtext);
      }
			/* send to specific client */
      else{
        sbuf.mtype = 4;
        sbuf.sender_pid = rbuf.sender_pid;
        sbuf.receiver_pid = rbuf.receiver_pid;
        strncpy(sbuf.mtext, rbuf.mtext, MSGSZ);
        if(sbuf.receiver_pid == client1_pid){
          if(msgsnd(client1, &sbuf, sizeof(message_buf) - sizeof(long),
						IPC_NOWAIT) < 0) {
    				perror("message send error");
    				exit(1);
    			}
        }
        else if(sbuf.receiver_pid == client2_pid){
          if(msgsnd(client2, &sbuf, sizeof(message_buf) - sizeof(long),
						IPC_NOWAIT) < 0) {
    				perror("message send error");
    				exit(1);
    			}
        }
        else if(sbuf.receiver_pid == client3_pid){
          if(msgsnd(client3, &sbuf, sizeof(message_buf) - sizeof(long),
						IPC_NOWAIT) < 0) {
    				perror("message send error");
    				exit(1);
    			}
        }
        else{
          perror("Receiver's pid doesn't exist");
        }
      }
    }

		/*
		 * check message queue for client2
		 */
    while(1){
      if(msgrcv(client2, &rbuf, sizeof(message_buf) - sizeof(long), 2, IPC_NOWAIT) < 0){
        break;
    	}
			/* broadcast */
      if(rbuf.receiver_pid == 0){
				chat.sender_pid = rbuf.sender_pid;
				memset(chat.message, '\0', MSGSZ);
        strncpy(chat.message, rbuf.mtext, MSGSZ);
        memcpy(shm + (header -> sender_pid + 1) * sizeof(chat_log), &chat, sizeof(chat_log));
        header -> sender_pid++;
        memcpy(shm, header, sizeof(chat_log));
        printf("[%d] : %s", rbuf.sender_pid, rbuf.mtext);
      }
			/* send to specific client */
      else{
        sbuf.mtype = 4;
        sbuf.sender_pid = rbuf.sender_pid;
        sbuf.receiver_pid = rbuf.receiver_pid;
        strncpy(sbuf.mtext, rbuf.mtext, MSGSZ);
        if(sbuf.receiver_pid == client1_pid){
          if(msgsnd(client1, &sbuf, sizeof(message_buf) - sizeof(long), IPC_NOWAIT) < 0) {
    				perror("message send error");
    				exit(1);
    			}
        }
        else if(sbuf.receiver_pid == client2_pid){
          if(msgsnd(client2, &sbuf, sizeof(message_buf) - sizeof(long), IPC_NOWAIT) < 0) {
    				perror("message send error");
    				exit(1);
    			}
        }
        else if(sbuf.receiver_pid == client3_pid){
          if(msgsnd(client3, &sbuf, sizeof(message_buf) - sizeof(long), IPC_NOWAIT) < 0) {
    				perror("message send error");
    				exit(1);
    			}
        }
        else{
          perror("Receiver's pid doesn't exist");
        }
      }
    }

		/*
		 * check message queue for client3
		 */
    while(1){
      if(msgrcv(client3, &rbuf, sizeof(message_buf) - sizeof(long), 3, IPC_NOWAIT) < 0){
        break;
    	}
			/* broadcast */
      if(rbuf.receiver_pid == 0){
				chat.sender_pid = rbuf.sender_pid;
				memset(chat.message, '\0', MSGSZ);
        strncpy(chat.message, rbuf.mtext, MSGSZ);
        memcpy(shm + (header -> sender_pid + 1) * sizeof(chat_log), &chat, sizeof(chat_log));
        header -> sender_pid++;
        memcpy(shm, header, sizeof(chat_log));
        printf("[%d] : %s", rbuf.sender_pid, rbuf.mtext);
      }
			/* send to specific client */
      else{
        sbuf.mtype = 4;
        sbuf.sender_pid = rbuf.sender_pid;
        sbuf.receiver_pid = rbuf.receiver_pid;
        strncpy(sbuf.mtext, rbuf.mtext, MSGSZ);
        if(sbuf.receiver_pid == client1_pid){
          if(msgsnd(client1, &sbuf, sizeof(message_buf) - sizeof(long), IPC_NOWAIT) < 0) {
    				perror("message send error");
    				exit(1);
    			}
        }
        else if(sbuf.receiver_pid == client2_pid){
          if(msgsnd(client2, &sbuf, sizeof(message_buf) - sizeof(long), IPC_NOWAIT) < 0) {
    				perror("message send error");
    				exit(1);
    			}
        }
        else if(sbuf.receiver_pid == client3_pid){
          if(msgsnd(client3, &sbuf, sizeof(message_buf) - sizeof(long), IPC_NOWAIT) < 0) {
    				perror("message send error");
    				exit(1);
    			}
        }
        else{
          perror("Receiver's pid doesn't exist");
        }
      }
    }
	}

	exit(0);
}
