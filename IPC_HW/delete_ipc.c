#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
  int client1, client2, client3;
  int shmid;

  int flg = IPC_CREAT | 0666;

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

  if(msgctl(client1, IPC_RMID, NULL) < 0){
    perror("msgctl error");
    exit(1);
  }
  if(msgctl(client2, IPC_RMID, NULL) < 0){
    perror("msgctl error");
    exit(1);
  }
  if(msgctl(client3, IPC_RMID, NULL) < 0){
    perror("msgctl error");
    exit(1);
  }

  if((shmid = shmget((key_t)999, (size_t)1028 * 101, flg)) < 0){
    perror("shmget error");
    exit(1);
  }

  if(shmctl(shmid, IPC_RMID, 0) < 0){
    perror("shmctl error\n");
    exit(1);
  }
}
