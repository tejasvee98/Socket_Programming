#include"stdio.h"  
#include"stdlib.h"  
#include"sys/types.h"  
#include"sys/socket.h"  
#include"string.h"  
#include"netinet/in.h"  
#include"netdb.h"
  

#define BUF_SIZE 512 
void error(int ret,char *msg){
	if (ret < 0) {  
     printf("Error in %s!\n",msg);  
     exit(1);  
    }
}
int main(int argc, char**argv) {  
 struct sockaddr_in addr, cl_addr;  
 int sockfd, ret;  
 char buffer[BUF_SIZE];  
 struct hostent * server;
 char * serverAddr;
int port;
 if (argc < 3) {
  printf("usage: client < ip address > <port_no>\n");
  exit(1);  
 }

 serverAddr = argv[1]; 
 port =atoi(argv[2]);
 sockfd = socket(AF_INET, SOCK_STREAM, 0);  
 error(sockfd,"creating socket");  
 printf("Socket created...\n");   

 memset(&addr, 0, sizeof(addr));  
 addr.sin_family = AF_INET;  
 addr.sin_addr.s_addr = inet_addr(serverAddr);
 addr.sin_port = port;     

 ret = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));  
 error(ret,"connecting to server");  
 printf("Connected to the server...\n");  

 memset(buffer, 0, BUF_SIZE);
 printf("Enter your Username: ");
 if (fgets(buffer, BUF_SIZE, stdin) != NULL) {
  ret = sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr)); 
 } 
  error(ret,"Sending Data");
  printf("Enter your Password: ");
  if (fgets(buffer, BUF_SIZE, stdin) != NULL) {
  ret = sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr));  
  error(ret,"sending data");
}

  ret = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
  if (ret < 0) {  
   printf("3Error receiving data!\n");    
  } 
	else {
    char *type,*token;
    type=strtok_r(buffer," ",&token);
    fputs(token, stdout);
    printf("\n");
	//Customer
    if (strcmp(type,"C")==0){
      printf("Enter your Query: ");
      if (fgets(buffer, BUF_SIZE, stdin) != NULL) {
      ret = sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr));  
      if (ret < 0) {  
        printf("Error sending data!\n\t-%s", buffer);  
      }
		}
			ret = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
			if (ret < 0) {  
				printf("3Error receiving data!\n");    
			} 
			else{
				fputs(buffer, stdout);
				printf("\n");
			}  
		}
		//Admin
		else if (strcmp(type,"A")==0){
			printf("Username: ");
      if (fgets(buffer, BUF_SIZE, stdin) != NULL){
      	ret = sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr));  
			}
      if (ret < 0) {  
        printf("Error sending data!\n\t-%s", buffer);  
      }
			ret = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
  		if (ret < 0) {  
   			printf("3Error receiving data!\n");    
  		}
			else{
				fputs(buffer,stdout);
				printf("\n");
			}
		 
		 if (fgets(buffer, BUF_SIZE, stdin) != NULL) {
      	ret = sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr));
		 }  
      if (ret < 0) {  
        printf("Error sending data!\n\t-%s", buffer);  
      }
			ret = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
  		if (ret < 0) {  
   			printf("3Error receiving data!\n");    
  		}
			else{
				fputs(buffer,stdout);
				printf("\n");
			}
  }
	//Police
	else if(strcmp(type,"P")==0){
		if (fgets(buffer, BUF_SIZE, stdin) != NULL){
        ret = sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr));  
      }
      if (ret < 0) {  
        printf("Error sending data!\n\t-%s", buffer);  
      }
      ret = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
      if (ret < 0) {  
        printf("3Error receiving data!\n");    
      }
      else{
        fputs(buffer,stdout);
        printf("\n");
      }

	}
}
  
  
 
 return 0;    
}  
