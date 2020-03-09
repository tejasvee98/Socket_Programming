#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"string.h"
#include"netinet/in.h"
#include "unistd.h"
#include <arpa/inet.h>
#include <time.h>
#define BUF_SIZE 512
#define CLADDR_LEN 100
#define MS_LEN 3


void error(int ret){
	if (ret < 0) {  
     printf("Error in Sending or Receiving Data!\n");  
     exit(1);  
    }
}



int check_username_password(char *usr,char *pass){
	FILE *fp=fopen("login","r");
	if(fp == NULL){
		printf("problem in opening user_login file\n"); // if user login file is corrupted 
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
			char *tok=strtok(NULL," ");
			char *p=malloc(strlen(token));
			strncpy(p,token,strlen(token));
			if (strcmp(p,pass)==0){
				//password matches!
				printf("Correct Credentials!\n");
				fclose(fp);
				return tok[0];
			}
        }
    }
    printf("Wrong Username or Password\n");
    fclose(fp);
    return 0;
}

char* get_customer_balance(char* usr)
{
  FILE *fp=fopen(usr,"r");
  if(fp == NULL){
    printf(" problem in opening Transaction file\n"); // if user login file is corrupted 
    return NULL;
  }
  char *line=NULL;
  size_t line_size=0;
  ssize_t read;
  if((read = getline(&line, &line_size, fp)) != -1)
  {
    char *token=strtok(line," ");
    if(token == NULL){
        fclose(fp); 
        printf(" Error in Transaction file\n");
         return "0.00";
    }
    for (int i = 0; i < 5; ++i)
    {
      token=strtok(NULL," ");
    }
    token=strtok(NULL," ");
    return token;
  }
  else {
    //In Case File is empty
    return "0.00";
  }
}

char* get_mini_stat(char* usr)
{
  FILE *fp=fopen(usr,"r");
  if(fp == NULL){
    printf(" problem in opening Transaction file\n"); // if user login file is corrupted 
    return NULL;
  }
  char *mini_stat = (char*) malloc(10000*sizeof(char));
  mini_stat[0] = '\0';
  int lines = 0;
  char *line=NULL;
  size_t line_size=0;
  ssize_t read;
  while(lines<MS_LEN && ((read = getline(&line, &line_size, fp)) != -1))
  {
    strcat(mini_stat,line);
    lines++;
  }
  return mini_stat;
}
char *handle_admin(char *customer,char *transaction){
  char *tr_type,*amt;
  char *ans=malloc(sizeof(char)*10000);
  tr_type=strtok(transaction," ");
  amt=strtok(NULL," ");
  strncpy(amt,amt,strlen(amt)-1);
  double cur_bal = atof(get_customer_balance(customer));
  double new_bal = cur_bal;
  if (strcmp(tr_type,"C")==0){
    new_bal=cur_bal+atof(amt);
  }
  else if(strcmp(tr_type,"D")==0){
    if (cur_bal-atof(amt)<0){
      sprintf(ans,"Insufficient Balance in account to debit this amount.Current Balance = Rs.%.2f\n",cur_bal);
      return ans;
    }
    else{
      new_bal=cur_bal-atof(amt);
    }
  }
  char *line=NULL;
  char *new_line=(char *)malloc(10000*sizeof(char));
  size_t len = 0;
  ssize_t read;
	time_t ltime; /* calendar time */
	ltime=time(NULL); /* get current cal time */
  sprintf(new_line,"%.*s %s %f\n",(int)strlen(asctime(localtime(&ltime)))-1,asctime(localtime(&ltime)),tr_type,new_bal);
  FILE *fp=fopen(customer,"r");
  while(read=getline(&line,&len,fp)!=-1) strcat(new_line,line);
  fclose(fp);
  fp=fopen(customer,"w");
  fwrite(new_line,sizeof(char),strlen(new_line),fp);
  fclose(fp);
  char * new_bal_ans=get_customer_balance(customer);
  
  sprintf(ans,"Transaction Performed Successfully.Current Balance = Rs.%.2f\n",atof(new_bal_ans));
  return ans;
}
int check_user_login(char *customer){
  FILE *fp=fopen("login","r");
  char *line=NULL;
  size_t len = 0;
  ssize_t read;
  while(read=getline(&line,&len,fp)!=-1){
    char *token,*usr,*type;
    usr=strtok(line," ");
    token=strtok(NULL," ");
    type=strtok(NULL,"\n");
    if (strcmp(type,"C")==0 && strcmp(usr,customer)==0){
      return 0;
    }
  }
  return -1;
}
int get_customer_names(char **ans){
  FILE *fp=fopen("login","r");
  int i=0;
  char *line=NULL;
  size_t len = 0;
  ssize_t read;
  while(read=getline(&line,&len,fp)!=-1){
    char *token,*usr,*type;
    usr=strtok(line," ");
    token=strtok(NULL," ");
    type=strtok(NULL,"\n");
    if (strcmp(type,"C\n")==0){
      strcpy(ans[i],usr);
      i++;
    }
  }
  return i;
}
void main(int argc,char **argv) {
 if (argc<2){
 	printf("usage- ./server <port_no>\n");
 	return;
 }
 struct sockaddr_in addr, cl_addr;
 int sockfd, len, ret, newsockfd;
 char buffer[BUF_SIZE];
 pid_t childpid;
 char clientAddr[CLADDR_LEN];
 
 sockfd = socket(AF_INET, SOCK_STREAM, 0);
 error(sockfd);
 printf("Socket created...\n");
 
 memset(&addr, 0, sizeof(addr));
 addr.sin_family = AF_INET;
 addr.sin_addr.s_addr = INADDR_ANY;
 addr.sin_port = atoi(argv[1]);
 
 ret = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
 error(ret);
 printf("Binding Done...\n");

 printf("Waiting for a Connection...\n");
 listen(sockfd, 5);

 while(1) { //infinite loop
  len = sizeof(cl_addr);
  newsockfd = accept(sockfd, (struct sockaddr *) &cl_addr, &len);
  error(newsockfd);
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
    error(ret);
    char *buf,*usr,*pass;
    usr=malloc(BUF_SIZE);
    pass=malloc(BUF_SIZE);
    strncpy(usr,buffer,strlen(buffer)-1);
    memset(buffer, 0, BUF_SIZE);
    ret = recvfrom(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, &len);
    error(ret);
    strncpy(pass,buffer,strlen(buffer)-1);
    memset(buffer, 0, BUF_SIZE);
    int usr_found=check_username_password(usr,pass);
    if (usr_found==0) buf="Wrong Username or Password";
    //Customer
    else if (usr_found=='C') 
    {
      buf = "C Welcome User!\nType 'Balance' to know balance.\nType 'Mini_stat' to get mini statement.\n";
      ret = sendto(newsockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);
      error(ret);
      ret = recvfrom(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, &len);
      error(ret);
      //printf("Hi        %s1", buffer);
      //customer_queries(usr);
      char *response = (char*) malloc(5000*sizeof(char));
      strcpy(response,"Response:\n");
      if(strcmp(buffer,"Balance\n")==0)
      {
        printf("%s\n", "Fetching balance\n");
        char* balance = (char*) malloc(500*sizeof(char));
        strcat(response,"Available balance: ");
        strcpy(balance,get_customer_balance(usr));
        printf("%s\n", balance);
        strcat(response,balance);
      }
      else if(strcmp(buffer,"Mini_stat\n")==0)
      {
        printf("%s\n", "Fetching Mini statement\n");
        char* mini_stat = (char*) malloc(5000*sizeof(char));
        strcat(response,"Mini Statement:\n");
        strcpy(mini_stat,get_mini_stat(usr));
        printf("%s\n", mini_stat);
        strcat(response,mini_stat);
      }
      else
      {
        printf("%s\n", "query incorrect\n");
        strcat(response,"Query incorrect");
      }
      ret = sendto(newsockfd, response, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);   
      error(ret);
    }
    //Admin
    else if (usr_found=='A') {
      memset(buffer,0,BUF_SIZE);
      buf = "A Welcome Admin!\nEnter 'Username' of  the Customer to perform transactions\n";
      ret = sendto(newsockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);
      error(ret);
      ret = recvfrom(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, &len);
      error(ret);
      char *customer;
      customer=malloc(BUF_SIZE);
      strncpy(customer,buffer,strlen(buffer)-1);
      customer[strlen(buffer)-1]='\0';     
      if(access( customer, F_OK ) != -1  && check_user_login(customer)==0) {
      // file exists
        buf = "Enter the Transaction to perform in the following format: <C/D> <Amount>\n";
        ret = sendto(newsockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);
        error(ret);
        ret = recvfrom(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, &len);
        error(ret);
        char* res=handle_admin(customer,buffer);
        ret = sendto(newsockfd, res, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);
        error(ret);
      
      } 
      else {
       //file doesn't exist
      char *res = "Wrong Username. Make Sure the Customer entry is there in login File and the corresponding transaction history file exists\n";
      ret = sendto(newsockfd, res, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);
      error(ret);
      }

    }
    //Police
    else if(usr_found=='P'){
      memset(buffer,0,BUF_SIZE);
      buf = "P Welcome Police!\nType 'Balance' to view balance of all users.\nType 'Mini_stat <username>' to get mini statement of a specific customer.\n";
      ret = sendto(newsockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);
      error(ret);
      ret = recvfrom(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, &len);
      error(ret);
      char *token=malloc(BUF_SIZE),*user1=malloc(BUF_SIZE);
      strncpy(token,buffer,strlen(buffer));
      token=strtok_r(token," ",&user1);
      token[10]='\0';
      printf("%s",token);
      if (strcmp(token,"Balance")==0){
        char *user=malloc(sizeof(char)*10000);
        int n=get_customer_names(&user);
        for (int i=0;i<n;i++){
          char *str;
          sprintf(str,"%s's Balance = Rs.%.2f\n",(char *)user[i],atof(get_customer_balance((char *)user[i])));
          ret = sendto(newsockfd, str, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);
          error(ret);
        }
      }
      else if(strcmp(token,"Mini_stat")==0){
        user1=strtok(NULL,"\n");
        printf("%s",user1);
        if (check_user_login(user1)==0) {
          char *str;
          sprintf(str,"%s",get_mini_stat(user1));
          ret = sendto(newsockfd, str, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);
          error(ret);
        }
        else {
          char *str="Enter Valid Username";
          ret = sendto(newsockfd, str, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);
          error(ret);
        }
      }

    }
    ret = sendto(newsockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);   
    error(ret);  
    printf("Sent data to %s: %s\n", clientAddr, buffer);
   }
  }
  close(newsockfd);
 }
}
	