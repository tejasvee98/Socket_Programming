#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"string.h"
#include"netinet/in.h"

#define PORT 4444
#define BUF_SIZE 2000
#define CLADDR_LEN 100

int check_username_password(char *usr,char *pass){
	FILE *fp=fopen("login","r");
	if(fp == NULL){
		printf(" problem in opening user_login file\n"); // if user login file is corrupted 
		return -1;
	}
	char *line=NULL;
	size_t line_size=0;
	ssize_t read;
	while((read = getline(&line, &line_size, fp)) != -1)
	{
		char *token=strtok(line," ");
		if(token == NULL){
			fclose(fp); 
			printf(" Error in login file\n");
			 return 0;
			}
		if(strcmp(token,usr)==0)
		{
			//User Found!
			token=strtok(NULL," ");
			char *passwd=token;
			if (strcmp(passwd,pass)==0)
			fclose(fp);
			return 1;
        }
    }
    printf("Wrong Username or Password\n");
    fclose(fp);
    return 0;
}

void main() {
 struct sockaddr_in addr, cl_addr;
 int sockfd, len, ret, newsockfd;
 char buffer[BUF_SIZE];
 pid_t childpid;
 char clientAddr[CLADDR_LEN];
 
 sockfd = socket(AF_INET, SOCK_STREAM, 0);
 if (sockfd < 0) {
  printf("Error creating socket!\n");
  exit(1);
 }
 printf("Socket created...\n");
 
 memset(&addr, 0, sizeof(addr));
 addr.sin_family = AF_INET;
 addr.sin_addr.s_addr = INADDR_ANY;
 addr.sin_port = PORT;
 
 ret = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
 if (ret < 0) {
  printf("Error binding!\n");
  exit(1);
 }
 printf("Binding Done...\n");

 printf("Waiting for a Connection...\n");
 listen(sockfd, 5);

 while(1) { //infinite loop
  len = sizeof(cl_addr);
  newsockfd = accept(sockfd, (struct sockaddr *) &cl_addr, &len);
  if (newsockfd < 0) {
   printf("Error Accepting Connection!\n");
   exit(1);
  }
  printf("Connection accepted\n");

  inet_ntop(AF_INET, &(cl_addr.sin_addr), clientAddr, CLADDR_LEN);
  if ((childpid = fork()) == 0) { //creating a child process

   close(sockfd); 
//stop listening for new connections by the main process. 
//the child will continue to listen. 
//the main process now handles the connected client.

   while(1) {
    memset(buffer, 0, BUF_SIZE);
    ret = recvfrom(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, &len);
    if(ret < 0) {
     printf("Error receiving data!\n");  
     exit(1);
    }
    char *buf,*usr,*pass;
    usr=buffer;
    ret = recvfrom(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, &len);
    if(ret < 0) {
     printf("Error receiving data!\n");  
     exit(1);
    }
    pass=buffer;
    int usr_found=check_username(usr,pass);
    if (usr_found==0) buf="Username Not found";
    else buf="Username Found";
    ret = sendto(newsockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);   
    if (ret < 0) {  
     printf("Error sending data!\n");  
     exit(1);  
    }  
    printf("Sent data to %s: %s\n", clientAddr, buffer);
   }
  }
  close(newsockfd);
 }
}
	