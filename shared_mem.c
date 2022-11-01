#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  ClientProcess(int []);

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
		 int i;
     pid_t  pid;

     ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     printf("Process has received a shared memory of two integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Process has attached the shared memory...\n");

		 ShmPTR[0] = 0; //bank account
     ShmPTR[1] = 0; //turn
		 printf("Orig Bank Account = %d\n", ShmPTR[0]);
     
		 pid = fork();
		 for (i=0; i<25; i++) {
			 if (pid > 0) {
				 int account;
				 int randomNumber = rand() % (5);
				 srand(getpid() + 1233 +i); // ensure different random numbers were chosen
				 sleep(randomNumber);
				 account = ShmPTR[0];
				 while (ShmPTR[1] != 0);
				 if (account <= 100) {
					 int balance = rand() % (100);
					 srand(getpid() + i + 999); // ensure different random numbers were chosen
					 if (balance % 2 == 0) {
						 account = account + balance;
						 printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
					 }
					 else {
						 printf("Dear old Dad: Doesn't have any money to give\n");
					 }

				 }
				 else {
					 printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
				 }
				//  printf("Account: %d\n", account);
				 ShmPTR[0] = account;
				 ShmPTR[1] = 1;
				//  printf("ShmPTR[0]: %d\n", ShmPTR[0]);
			 }
			 else {
				 int balance;
				 int account;
				 int randomNumber;
				 randomNumber = rand() % (5);
				 srand(getpid() + i + 99); // ensure different random numbers were chosen
				 sleep(randomNumber);
				//  printf("Now checking ShmPTR[0]:%d\n", ShmPTR[0]);
				 account = ShmPTR[0];
				 while (ShmPTR[1] != 1);
				 balance = rand() % (50);
				 srand(getpid()+122 +i); // ensure different random numbers were chosen
				 printf("Poor Student needs $%d\n", balance);
				 if (balance <= account) {
					 account = account - balance;
					 printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
				 }
				 else {
					 printf("Poor Student: Not Enough Cash ($%d)\n", account );
				 }
				 ShmPTR[0] = account;
				 ShmPTR[1] = 0;
			 }
		 }

     printf("Process has detected the completion of its child...\n");
     shmdt((void *) ShmPTR);
     printf("Process has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Process has removed its shared memory...\n");
     printf("Process exits...\n");
     exit(0);
}
