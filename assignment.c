#include <stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
//#include "../include/global.h"
//#include "../include/logger.h"

#define MSG_LENGTH 2056
#define STDIN 0
#define CMD_SIZE 1000
#define BUFFER_SIZE 900
#define EPHEMERAL_PORT 53
#define AUTHOR "AUTHOR\n"
#define IP "IP\n"
#define PORT "PORT\n"
#define LIST "LIST\n"
#define LOGIN "LOGIN\n"
struct client{
    int list_id;
    char *ip;
    int client_fd;
    char *hostname;
    int block_status;
    int client_status;
    int port_no;
    int num_msg_recv;
    int num_msg_sent;
    struct client *next;    
};
struct client *cli_ref = NULL;
int port_no[5];
int port_noss[5] = {0,0,0,0,0};
int portnoo[5] = {0,0,0,0,0};
char *string[400];
char logout_buffer[800];
char logout_buffer4[800];
char logout_buffer5[800];
char logout_buffer6[800];
char logout_buffer7[800];
int block4[5]={0,0,0,0,0};
int block5[5]={0,0,0,0,0};
int block6[5]={0,0,0,0,0};
int block7[5]={0,0,0,0,0};
char clientoo[4][200]; //Used for refresh and List creating
char buff[4][100]; //Used for refresh and List creating
int p[4]; //Used for refresh and List creating
int logout4 =0, logout5 = 0, logout6 = 0, logout7 = 0;
char client_ip[5][100];
int c =1;
int adder =0;
int server(struct client **head_ref, int port_no, int listening_fd, struct client **c_ref);
int client(struct client **c_ref , int port_no, int listening_fd);
void exiting(struct client **head_ref,int sock_index);
void print_cli_list();
void blocking(int sock_index,char *buffer,struct client *node);
void print_author();
void send_to_loginlist(int sock_index);
void print_portno(int port_no);
void send_to_client(int sock_index, char *send_to_ip, char *buffer, struct client *temp,int n);
char* find_ip(char *str);
void Ip_addr();
void send_port(int listening_port, int server_fd);
void unblocking(int sock_index,char *buffer,struct client *node);
void assign_port(char * buffer, struct client *temp,int fdaccept);
void blocked(struct client *node,char * buffer);
void tostring(char str[], int num);
void refresh(int server_fd);
void send_client_list(struct client* headref,char client_list[]);
void current_list(struct client *node);
void display_stats(struct client *node);
void createclient(char *recv_buf); //creates client list
void send_list(struct client *head_ref, int fdaccept);
void broadcast(int sock_index,char *buffer,struct client *head_ref);
void add_new_client(struct client **head_ref,int fdaccept, struct sockaddr_in client_addr,int port_no,struct client **c_ref);
void logout(struct client *head_ref,int sock_index);
void loginagain(struct client *head_ref,int sock_index);
int isValidIP(char *ip);
int valid_digit(char *ip_str);
char from_ip[25];
void listoo(struct client *c_ref);

int main(int argc, char *argv[]) {
    
    /*Init. Logger*/
    //cse4589_init_log(argv[2]);
    
    /* Clear LOGFILE*/
   // fclose(fopen(LOGFILE, "w"));
    
    int listening_fd=0,temp_result;
    struct client *head_ref = NULL;
    struct client *c_ref = NULL;
    struct sockaddr_in listen_addr;
    //struct ClientLists *cli_list = NULL;
    
    if(argc!=3)
    {
        //printf("\nInvalid Input, Enter either s or c with port number");
        exit(-1);
    }
    listening_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(listening_fd < 0)
    {
        
        printf("%d",listening_fd);
        printf("Server Socket creation failed\n");
        exit(EXIT_FAILURE);
    }
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(atoi(argv[2]));
    
    // Binding the socket to a port
    temp_result = bind(listening_fd, (struct sockaddr *)&listen_addr,sizeof(listen_addr));
    if(temp_result <0)
    {
        printf("Bind failed\n");
        exit(EXIT_FAILURE);
    }
    temp_result =listen(listening_fd, 4);
    if(temp_result< 0)
    {
        printf( "Listen function failed\n");
        exit(EXIT_FAILURE);
    }
    
    if(*argv[1] == 's'){
        //printf("In server");
        server(&head_ref,atoi(argv[2]), listening_fd, &c_ref);
        }
    else{
      //printf("In client");
        client(&c_ref, atoi(argv[2]), listening_fd);
    
    }
    return 0;
}
int server(struct client **head_ref, int port_no, int listening_fd, struct client **c_ref)
{
    //head_ref = (struct client*) malloc(sizeof(struct client));
    
    int server_head,selret, caddr_len=0, fdaccept=0;
    struct sockaddr_in client_addr;
    fd_set master_list, watch_list;
    
    /* Zero select FD sets */
    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);
    /* Register the listening socket */
    FD_SET(listening_fd, &master_list);
    /* Register STDIN */
    FD_SET(STDIN, &master_list);
    
    server_head = listening_fd;
    
    while(1){
        memcpy(&watch_list, &master_list, sizeof(master_list));
        
        //printf("\n[PA1-Server@CSE489/589]$ ");
        //fflush(stdout);
        
        /* select() system call. This will BLOCK */
        selret = select(server_head + 1, &watch_list, NULL, NULL, NULL);
        if(selret < 0)
            perror("select failed.");
        
        /* Check if we have sockets/STDIN to process */
        if(selret > 0){
            /* Loop through socket descriptors to check which ones are ready */
            for(int sock_index=0; sock_index<=server_head; sock_index+=1){
                
                if(FD_ISSET(sock_index, &watch_list)){
                    /* Check if new command on STDIN */
                    if (sock_index == STDIN){
                        char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);
                        char *input = (char*) malloc(sizeof(char)*CMD_SIZE);
                        char *command = (char*) malloc(sizeof(char)*CMD_SIZE);
                        memset(cmd, '\0', CMD_SIZE);
                        
                        if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
                            exit(-1);
                            //printf("%s",cmd);
                            strcpy(command,cmd);
                       // printf("CMD: %s\n",cmd);
                        input = strtok(cmd, " ");
                        //printf("After strtok:%s\n",input);
                        //Process PA1 commands here ...
                        if(strcmp(cmd, "AUTHOR\n")==0)
                            print_author();
                            
                        else if(strcmp(cmd, "IP\n")==0)
                        {
                            char ip_str[INET_ADDRSTRLEN];
                            //strcpy(ip_str,find_ip(ip_str));
                            //printf("%s",ip_str);
                            //find_ip();
                            Ip_addr();
                        }
                        else if(strcmp(cmd, "PORT\n")==0)
                        {
                            //printf("PORT");
                            print_portno(port_no);
                        }
                        else if(strcmp(cmd, "LIST\n")==0)
                        {
                            //printf("LIST\n");
                            current_list(*head_ref);
                            //print(*head_ref);
                        }
                        else if(strcmp(cmd,"STATISTICS\n")==0)
                        {
                         //printf("STATISTICS\n");
                         printf("[%s:SUCCESS]\n","STATISTICS");
                         display_stats(*head_ref);
                         printf("[%s:END]\n","STATISTICS");
                         //prints(*c_ref);
                        } 
                        else if(strcmp(input,"BLOCKED")==0)
                        {
                         //printf("In Blocked %s",input);
                         //printf("\n%d",strlen(input));
                         cmd = strtok(NULL,"\n");
                         //printf("%s\n",cmd);
                         printf("[%s:SUCCESS]\n","BLOCKED");
                         //display_stats(*head_ref);
                         
                         blocked(*head_ref,cmd);
                         printf("[%s:END]\n","BLOCKED");
                         }
                        else{
                        	cmd[strcspn(cmd, "\n")] = 0;
                        	printf("[%s:ERROR]\n",cmd);
                               printf("[%s:END]\n",cmd);
                       
                    }
                        //printf("\n Wrong command");
                        
                        //free(cmd);
                    }
                    /* Check if new client is requesting connection */
                    else if(sock_index == listening_fd){
                   // printf("In accept\n");
                        caddr_len = sizeof(client_addr);
                        fdaccept = accept(listening_fd, (struct sockaddr *)&client_addr, (socklen_t*)&caddr_len);
                        if(fdaccept < 0)
                            perror("Accept failed.");
                        //push(&head_ref, client_addr.sin_port, client_addr.sin_addr.s_addr);
                        else
                        {
                           // printf("\nRemote Host connected!\n");
                            
                            /* Add to watched socket list */
                            FD_SET(fdaccept, &master_list);
                            if(fdaccept > server_head)
                             server_head = fdaccept;
                            add_new_client(head_ref, fdaccept, client_addr,port_no,c_ref);
                            adder++;
                            
                        }
                        
                    }
                    /* Read from existing clients */
                    else{
                        
                        /* Initialize buffer to receieve response */
                        if(sock_index == 0)
                            break;
                        char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                        memset(buffer, '\0', BUFFER_SIZE);
                        int recd_bytes;
                        if(recv(recd_bytes = sock_index, buffer, BUFFER_SIZE, 0) <= 0)
                        {
                            
                            close(sock_index);
                            
                            /* Remove from watched list */
                            FD_CLR(sock_index, &master_list);
                            // printf("Removed sock_index %d", sock_index);
                        }
                        
                        else
                        {                           
                            struct client *temp = *head_ref;
                            char *send_to_ip = (char*) malloc(sizeof(char)*INET_ADDRSTRLEN);
                            char * msg = (char*) malloc(sizeof(char)*MSG_LENGTH);
                            char * msg1 = (char*) malloc(sizeof(char)*MSG_LENGTH);
                            char * msg2 = (char*) malloc(sizeof(char)*MSG_LENGTH);
                           // buffer[recd_bytes]='\0';
                            strcpy(msg1,buffer);
                            strcpy(msg2,buffer);
                            //printf("\nMsg: %s\n", msg2);
                            //printf("Buffer: %s\n",buffer);
                            send_to_ip = strtok(buffer," ");
                            if(strcmp(send_to_ip, "Port") == 0)
                            {
                                //printf("Inside Port");
                                int success =0;
                                //printf("\n%d",head_ref->port_no);
                                //printf("\n%d",temp->port_no);
                                assign_port(buffer, temp,fdaccept);
                                send_list(*head_ref,fdaccept);
                                    success = 1;
                            }
                            else if(strcmp(send_to_ip,"LOGOUT")==0)
                            {
                             //printf("In logout\n");
                             //printf("Sock_Index: %d\n",sock_index);
                             logout(*head_ref,sock_index);
                             }
                             else if(strcmp(send_to_ip,"EXIT")==0)
                              exiting(head_ref,sock_index);
                             else if(strcmp(send_to_ip,"REFRESH")==0)
                             {
                             //printf("sending refresh\n");
                             send_list(*head_ref, sock_index);
                             }
                             else if(strcmp(send_to_ip,"BLOCK")==0)
                             {
                              //printf("Inside block");
                              buffer=strtok(NULL," ");
                              //printf("Msg1 %s\n",buffer);
                              blocking(sock_index,buffer,*head_ref);
                             }
                             else if(strcmp(send_to_ip,"UNBLOCK")==0)
                             {
                              buffer=strtok(NULL," ");
                              //printf("Msg1 %s\n",buffer);
                              unblocking(sock_index,buffer,*head_ref);
                             }
                             else if(strcmp(send_to_ip,"BROADCAST")==0)
                             {
                              buffer = strtok(NULL," ");
                              buffer = strtok(buffer,"\n");
                              broadcast(sock_index,buffer,*head_ref);
                              }
                             else if(strcmp(send_to_ip,"AAAA") ==0)
                             {
                              //printf("In Login AAAA\n");
                              loginagain(*head_ref,sock_index);
                              
                               if(sock_index == 4 && strlen(logout_buffer4)!=0){
                                char * msg4 = (char*) malloc(sizeof(char)*MSG_LENGTH);
                                char * msg41 = (char*) malloc(sizeof(char)*MSG_LENGTH);
                                char *msg42 = (char *) malloc(sizeof(char)*MSG_LENGTH);
                                //logout_buffer4[strlen(logout_buffer4)]='\0';
                                     //printf("%s",logout_buffer4);
                                     logout4 = 0;
                                     int i=0,j=0;
                                     //strcpy(msg41,logout_buffer4);
                                     strcpy(msg42,logout_buffer4);
                                     
    					while(msg42 != NULL)
    					{
        					//printf("\n%s \n", msg42);
        					msg41=strtok(msg42,",");
        					msg42 = strtok(NULL,",");
        					//printf("\n%s",msg42);
        					send_to_ip = strtok(msg41," ");
        					//printf("Send to IP:%s\n", send_to_ip);
        					msg41= strtok(NULL, ","); 
        					strcpy(msg,msg41);
        					//printf("msg41%s\n",msg41);					
                                     		msg4 = strtok(msg41," ");
                                     		//printf("msg4%s\n",msg4);
                                     		msg41 = strtok(NULL,",");
                                     		
                                            //  printf("logout_buffer4: %s\n",msg42);
                                              struct client *temp = *head_ref;
 					       temp = *head_ref;
 					      while(temp!=NULL)
 					     {
 					      if(temp->client_fd == sock_index){
 					      temp->num_msg_recv++;
 					      //printf("Printing sock_index: %d\n",temp->client_fd);
 					     break; 
 					    }
 					     temp=temp->next;
 					   }
 					  temp = *head_ref;
                                	send_to_client(sock_index, send_to_ip, msg41 ,temp,1);
                                	//printf("Send to client is done");
                                      send(sock_index,msg,strlen(msg),0); 
                                    }
                                    strcpy(logout_buffer4,"");
                                    }
                                    
                                   if(sock_index == 5 && strlen(logout_buffer5)!=0){
		                        char * msg5 = (char*) malloc(sizeof(char)*MSG_LENGTH);
		                        char * msg51 = (char*) malloc(sizeof(char)*MSG_LENGTH);
		                        char *msg52 = (char *) malloc(sizeof(char)*MSG_LENGTH);
		                        logout_buffer5[strlen(logout_buffer5)]='\0';
		                             //printf("%s",logout_buffer5);
		                             logout5 = 0;
		                             int i=0,j=0;
		                             //strcpy(msg51,logout_buffer5);
	    					while(msg52 != NULL)
    					{
        					//printf("\n%s \n", msg52);
        					msg51=strtok(msg52,",");
        					msg52 = strtok(NULL,",");
        					send_to_ip = strtok(msg51," ");
        					//printf("Send to IP:%s\n", send_to_ip);
        					msg51= strtok(NULL, ","); 
        					strcpy(msg,msg51);
        					//printf("msg51%s\n",msg51);					
                                     		msg5 = strtok(msg51," ");
                                     		//printf("msg5%s\n",msg5);
                                     		msg51 = strtok(NULL,",");
                                     		
                                     			//printf("logout_buffer5: %s\n",msg);
		                             struct client *temp = *head_ref;
	 					temp = *head_ref;
	 					while(temp!=NULL)
	 					{
	 					 if(temp->client_fd == sock_index){
	 					  temp->num_msg_recv++;
	 					  //printf("Printing sock_index: %d\n",temp->client_fd);
	 					  break; 
	 					  }
	 					 temp=temp->next;
	 					 }
	 					  temp = *head_ref;
		                        	send_to_client(sock_index, send_to_ip, msg51 ,temp,1);
		                            send(sock_index,msg,strlen(msg),0); 
		                            }
		                            strcpy(logout_buffer5,"");
		                            }
                                  if(sock_index == 6 && strlen(logout_buffer6)!=0){
                                       				 char * msg6 = (char*) malloc(sizeof(char)*MSG_LENGTH);
                                		char * msg61 = (char*) malloc(sizeof(char)*MSG_LENGTH);
                               		 char *msg62 = (char *) malloc(sizeof(char)*MSG_LENGTH);
                                logout_buffer6[strlen(logout_buffer6)]='\0';
                                     //printf("%s",logout_buffer6);
                                     logout6 = 0;
                                     int i=0,j=0;
                                     //strcpy(msg61,logout_buffer6);
                                     strcpy(msg62,logout_buffer6);
                                     while(msg62 != NULL)
    					{
        					//printf("\n%s \n", msg62);
        					msg61=strtok(msg62,",");
        					msg62 = strtok(NULL,",");
        					send_to_ip = strtok(msg61," ");
        					//printf("Send to IP:%s\n", send_to_ip);
        					msg61= strtok(NULL, ","); 
        					strcpy(msg,msg61);
        					//printf("msg61%s\n",msg61);					
                                     		msg6 = strtok(msg61," ");
                                     		//printf("msg6%s\n",msg6);
                                     		msg61 = strtok(NULL,",");
                                     		
                                     //printf("logout_buffer6: %s\n",msg);
                                     struct client *temp = *head_ref;
 					temp = *head_ref;
 					while(temp!=NULL)
 					{
 					 if(temp->client_fd == sock_index){
 					  temp->num_msg_recv++;
 					  //printf("Printing sock_index: %d\n",temp->client_fd);
 					  break; 
 					  }
 					 temp=temp->next;
 					 }
 					  temp = *head_ref;
                                	send_to_client(sock_index, send_to_ip, msg61 ,temp,1);
                                    send(sock_index,msg,strlen(msg),0); 
                                    }
                                    strcpy(logout_buffer6,"");
                                    }
                                   if(sock_index == 7 && strlen(logout_buffer7)!=0){
                                     				 char * msg7 = (char*) malloc(sizeof(char)*MSG_LENGTH);
                               	 char * msg71 = (char*) malloc(sizeof(char)*MSG_LENGTH);
                               	 char *msg72 = (char *) malloc(sizeof(char)*MSG_LENGTH);
                                	logout_buffer7[strlen(logout_buffer7)]='\0';
                                     //printf("%s",logout_buffer7);
                                     logout7 = 0;
                                     int i=0,j=0;
                                     //strcpy(msg71,logout_buffer7);
                                     strcpy(msg72,logout_buffer7);
                                 	while(msg72 != NULL)
    					{
        					//printf("\n%s \n", msg72);
        					msg71=strtok(msg72,",");
        					msg72 = strtok(NULL,",");
        					send_to_ip = strtok(msg71," ");
        					//printf("Send to IP:%s\n", send_to_ip);
        					msg71= strtok(NULL, ","); 
        					strcpy(msg,msg71);
        					//printf("msg71%s\n",msg71);					
                                     		msg7 = strtok(msg71," ");
                                     		//printf("msg7%s\n",msg7);
                                     		msg71 = strtok(NULL,",");
                                     		
                                     //printf("logout_buffer7: %s\n",msg);
                                     struct client *temp = *head_ref;
 					temp = *head_ref;
 					while(temp!=NULL)
 					{
 					 if(temp->client_fd == sock_index){
 					  temp->num_msg_recv++;
 					  //printf("Printing sock_index: %d\n",temp->client_fd);
 					  break; 
 					  }
 					 temp=temp->next;
 					 }
 					  temp = *head_ref;
                                	send_to_client(sock_index, send_to_ip, msg71 ,temp,1);
                                    send(sock_index,msg,strlen(msg),0); 
                                    }
                                    strcpy(logout_buffer7,"");
                                    }
    				    }
                                 
		                    else
		                    {
		                        int i=0,space = 0;
		                        char * msg = (char*) malloc(sizeof(char)*MSG_LENGTH);
						msg = strtok(msg1," ");
						//printf("Msg and Msg1 aftertok: %s %s\n",msg,msg1);
						msg1 = strtok(NULL," ");
						strcpy(msg,msg1);
					while(msg1 != NULL)
	    				{
	       					//printf("\n Print Msg1 and Msg:%s %s\n", msg1,msg);
						msg1= strtok(NULL, " ");
	      				        if(msg1!=NULL){
	      				        strcat(msg," ");
							strcat(msg,msg1);}

	   				 }

		                       // strcpy(msg,buffer);
		                       //printf("\n%s", msg);
		                        struct client *temp = *head_ref;
		                        
		                        //printf("\n from_ip\n");
	 				temp = *head_ref;
		                        send_to_client(sock_index, send_to_ip, msg ,temp,0);
		                    }
		                    
		                    fflush(stdout);
		                    
		                }
		                
                        // free(buffer);
                        
                    }
                    
                }
            }
        }
    }
    
    return 0;
}
void listoo(struct client *c_ref)
{
 struct client *temp = c_ref;
 while(temp!=NULL)
 {
  printf("Print listoo %d",c_ref->list_id);
  temp= temp->next;
 }
 }
void current_list(struct client *node){
printf((char *)"[%s:SUCCESS]\n","LIST");
struct client *temp = node;
//struct sockaddr
//*temp = *node;
int num=1;
int t, g=0,i,j;
for(i=0;i<5;i++)
{
 portnoo[i] = port_noss[i];
} 
while(temp!=NULL){
//printf("%d",temp->client_status);
	//if(temp->client_status == 1)
	//{
		// printf("Inside if\n");
		 if(temp->client_fd == 4)
            	temp->port_no = port_noss[0];
            else if(temp->client_fd == 5)
            	temp->port_no = port_noss[1];
            else if(temp->client_fd == 6)
            	temp->port_no = port_noss[2];
            else if(temp->client_fd == 7)
            	temp->port_no = port_noss[3];	
		//printf("%-5d%-35s%-20s%-8d\n",num,temp->hostname,client_ip[temp->list_id],temp->port_no);
	//}	
	temp=temp->next;
	
   }
   for(i=0;i<4;i++){
   for(j=0;j<4-i;j++){
 /*  if(portnoo[j+1]==0){
  // printf("Inside if2r\n");
    g=1;
    //break;
    }*/
    if(portnoo[j]>portnoo[j+1] && portnoo[j+1]!=0)
    {
    // printf("Inside ifew2\n");
     t=portnoo[j];
     portnoo[j]=portnoo[j+1];
     portnoo[j+1]=t;
    }
    }
    //if(g==1){
    // printf("g is 1");
     //break;
    } 
    t=adder;
    i=0;
    temp = node;
    while(t!=0)
    {
     while(temp!=NULL){
      //printf("Inside if2\n");
     if(temp->port_no == portnoo[i] && temp->client_status ==1)
     {
      printf("%-5d%-35s%-20s%-8d\n",num,temp->hostname,client_ip[temp->list_id],temp->port_no);
      i++;
      t--;
      num++;
      temp=node;
      break;
    }
    else if(temp->port_no == portnoo[i] && temp->client_status == 0){
    //printf("Inside if2\n");
     i++;
     t--;
    temp=node;
    }
    else
    temp=temp->next;
   }
   }
   //for(int i=0;i<4;i++){
  // printf("Inside for");
        //printf("\nPort nos%d",port_noss[i]);}
 printf((char*)"[%s:END]\n","LIST");
}

 void display_stats(struct client *node){
 //printf("Displaying statistics\n");
 int num =1;
 char state[20];
 struct client *temp = node;
int t, g=0,i,j;
for(i=0;i<5;i++)
{
 portnoo[i] = port_noss[i];
} 
while(temp!=NULL){
		 if(temp->client_fd == 4)
            	temp->port_no = port_noss[0];
            else if(temp->client_fd == 5)
            	temp->port_no = port_noss[1];
            else if(temp->client_fd == 6)
            	temp->port_no = port_noss[2];
            else if(temp->client_fd == 7)
            	temp->port_no = port_noss[3];	
		
	temp=temp->next;
	
   }
   for(i=0;i<4;i++){
   for(j=0;j<4-i;j++){
    if(portnoo[j]>portnoo[j+1] && portnoo[j+1]!=0)
    {
     t=portnoo[j];
     portnoo[j]=portnoo[j+1];
     portnoo[j+1]=t;
    }}
    //if(g==1)
     //break;
    }
    t=adder;
    i=0;
    temp = node;
    while(t!=0)
    {
     while(temp!=NULL){
     if(temp->port_no == portnoo[i])
     {
      if(temp->client_status == 1)
        strcpy(state,"logged-in");
       else
      strcpy(state,"logged-out"); 
      printf("%-5d%-35s%-8d%-8d%-8s\n", num, temp->hostname, temp->num_msg_sent, temp->num_msg_recv,state);
      i++;
      t--;num++;
      temp=node;
      break;
    }
    temp=temp->next;
    }
   }
  }
void assign_port(char *buffer, struct client *temp,int fdaccept)
{
    char *port,*clientip;int count =0;
    //printf("\n Assigning ports\n");
    while(count != 2)
    {
        char *a = (char*) malloc(sizeof(char)*MSG_LENGTH);
        strcpy(a,buffer);
        if(count == 0)
        {
            buffer = strtok(NULL, " ");
            strcpy(a,buffer);
            clientip = a;
            //printf(" %s ", clientip);
        }
        else if(count == 1)
        {
            buffer = strtok(NULL, "\n");
            strcpy(a,buffer);
            port = a;
            break;
        }
        
        count +=1;
    }
    int j;
    for(j =1; j<=5; j++)
    {
        if(strcmp(clientip,client_ip[j])==0)
        {
            //printf("\n %d %s", j, client_ip[j]);
            break;
        }
    }
    
    if(j==6)
    { printf("\n Could not find receiver");
    }
    else
    {
        while(temp->list_id != j)
            temp = temp->next;
        if(temp == NULL);
        //printf("\n Could not find receiver!");
        else{
            temp->port_no = atoi(port);
        
            if(fdaccept == 4)
            	port_noss[0]=atoi(port);
            else if(fdaccept == 5)
            	port_noss[1] = atoi(port);
            else if(fdaccept == 6)
            	port_noss[2] = atoi(port);
            else if(fdaccept == 7)
            	port_noss[3] = atoi(port);			
            //printf("\nClientfd:%d\n",temp->client_fd);
        }
        
    }
   
}
void print_portno(int port_no)
{
    printf("[%s:SUCCESS]\n","PORT");
    printf("PORT:%d\n",port_no);
    printf("[%s:END]\n","PORT");
}
 void exiting(struct client **head_ref,int sock_index){
 //printf("Display Logout\n");
 //printf("inside exiting1\n");
 struct client *temp1;
  //printf("inside exiting2\n");
 struct client *temp = *head_ref;
  //printf("inside exiting3\n");
 struct client *temp2 = *head_ref;
  //printf("inside exiting4\n");
 struct client *prev;
 int k =0;
 while(temp!=NULL){

	 if (adder==1){
	 	//printf("before free\n");
	 	*head_ref=NULL;
	 	//free(temp2);
	 	//printf("after free\n");
	 	k=1;
	 	break;
	 	}
	 else if(temp2->client_fd==sock_index){
	 	temp1=temp2->next;
	 	*head_ref=temp1;
	 	//free(temp);
	 	//printf("\n1\n");
	 	k=1;
	 	break;}
	 	
	
	 else if((temp->next)->client_fd==sock_index && temp->next!=NULL){
	 	prev=temp;
	 	temp1=temp->next;
	 	prev->next=temp1;
	 	//free(temp);
	 	//printf("\n2\n");
	 	k=1;
	 	break;
	 	}
	 else if((temp->next)->client_fd==sock_index && temp->next==NULL){
	 	prev=temp;
	 	prev->next=NULL;
	 	//free(temp);
	 	//printf("\n3\n");
	 	k=1;
	 	break;
	 }
	temp=temp->next;
}
adder--;

if(k==1){
printf("[EXIT:SUCCESS]\n");
printf("[EXIT:END]\n");
}
else
printf("Error\n");
}
void add_new_client(struct client **head_ref,int fdaccept, struct sockaddr_in client_addr,int port_no,struct client **c_ref)
{
    
    //printf("Inside Add_new_client\n");
    unsigned int client_port = ntohs(client_addr.sin_port);
    char str[INET_ADDRSTRLEN];
    //char *str1 = (char *)malloc(sizeof(char));
    char str1[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), str, INET_ADDRSTRLEN);
    //printf("\nCheck Port\n");//checking the print value
    //printf("%d", port_no);//Printing the server port
    struct hostent *hostname = NULL;
    struct in_addr ipv4addr;
    int flag=0;
    inet_ntop(AF_INET, &(client_addr.sin_addr), str1, INET_ADDRSTRLEN);
    inet_pton(AF_INET, str1, &ipv4addr);
    hostname = gethostbyaddr(&ipv4addr, sizeof(ipv4addr), AF_INET);
    int hlen = (int)strlen(hostname->h_name);
    //printf("fdaccept: %d\n",fdaccept); 
    struct client* new_node = (struct client*) malloc(sizeof(client));
    struct client * temp = *head_ref;
    //new_node->port_no = port_no;
    new_node->ip = (char*)malloc(sizeof(MSG_LENGTH));
    //printf("%s", str);
    strcpy(new_node->ip,str);
    //printf("%s", new_node->ip);
    strcpy(client_ip[c],str);
  //  printf("\n %s", client_ip[c]);
    if(strcmp(client_ip[c],"128.205.36.35") == 0){
     new_node->hostname = (char*)malloc(100);
     strcpy(new_node->hostname,"embankment.cse.buffalo.edu");
     //printf("Yes");
     }
     else if(strcmp(client_ip[c],"128.205.36.34") == 0){
     new_node->hostname = (char*)malloc(100);
     strcpy(new_node->hostname,"euston.cse.buffalo.edu");
     //printf("Yes");
     }
     else if(strcmp(client_ip[c],"128.205.36.33") == 0){
     new_node->hostname = (char*)malloc(100);
     strcpy(new_node->hostname,"highgate.cse.buffalo.edu");
     //printf("Yes");
     }
     else if(strcmp(client_ip[c],"128.205.36.36") == 0){
     new_node->hostname = (char*)malloc(100);
     strcpy(new_node->hostname,"underground.cse.buffalo.edu");
     //printf("Yes");
     }
     else if(strcmp(client_ip[c],"128.205.36.46") == 0){
     new_node->hostname = (char*)malloc(sizeof(hlen));
     strcpy(new_node->hostname,"stones.cse.buffalo.edu");
     //printf("Yes");
     }
     else 
     printf("No %d /n",strcmp(client_ip[c],"128.205.36.35"));
    new_node->list_id = c;
    c++;
    new_node->client_fd = fdaccept;
    new_node->client_status =1;
    new_node->num_msg_sent = 0;
    new_node->num_msg_recv = 0;
    new_node->block_status = 0;
    //new_node->next = NULL;
    //printf("%d",new_node->client_status);
    
    if(*head_ref==NULL){
       // printf("Hola if\n");
        
       // *c_ref = *head_ref;
        new_node->next = NULL;
        *head_ref = new_node;
    }
    else{
        //printf("\nHola else!\n");
        while(temp!=NULL)
        {
         if(temp->client_fd == fdaccept){
        //  printf("Already present\n");
          flag = 1;
          break;
          }
          temp=temp->next;
        }
        if(flag == 0){
       // printf("Creating a new node\n");
        new_node->next = *head_ref;
        *head_ref = new_node;
       // *c_ref = new_node;
    }
    }
    
}
void logout(struct client *node,int sock_index){
 //printf("Display Logout\n");
 //int num =1;
 struct client *temp = node;
 while(temp!=NULL){
  if(temp->client_fd == sock_index){
  temp->client_status = 0;
  //printf("Client status: %d\n",temp->client_status);
  // printf("Success");
   break;
   }
  temp=temp->next;
  }
 } 
 void loginagain(struct client *node,int sock_index){
 //printf("Login again 2nd time\n");
 //int num =1;
 struct client *temp = node;
 while(temp!=NULL){
  if(temp->client_fd == sock_index){
  temp->client_status = 1;
  //printf("Client status: %d\n",temp->client_status);
   //printf("Success");
   break;
   }
  temp=temp->next;
  }
 } 
void Ip_addr(){

   char host[256];
   char *ip_addr;
   int check = 0;
   struct hostent *host_entry;
   int hostname;
   hostname = gethostname(host, sizeof(host)); //find the host name
   if (hostname == -1)
   	check=1;
   host_entry = gethostbyname(host); //find host information
   if (host_entry == NULL)
   	check=1;
   if (check ==1){
   	printf("[IP:ERROR]\n");
   	printf("[IP:END]\n");
   	
   }
   else{
	   ip_addr = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); //Convert into IP string
	   printf("[IP:SUCCESS]\n");
	   printf("IP:%s\n", ip_addr);
	   printf("[IP:END]\n");
	}

}
 
char* find_ip(char *str)
{
    struct sockaddr_in udp;
    int temp_udp =socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int len = sizeof(udp);
    //char str[INET_ADDRSTRLEN];
    int result;
    
    if (temp_udp == -1)
    {
        //printf("Socket creation failed!");
    }
    
    memset((char *) &udp, 0, sizeof(udp));
    udp.sin_family = AF_INET;
    udp.sin_port = htons(EPHEMERAL_PORT);
    inet_pton(AF_INET, "8.8.8.8", &udp.sin_addr);
    //udp.sin_addr.s_addr = inet_addr("8.8.8.8");
    
    if (connect(temp_udp, (struct sockaddr *)&udp, sizeof(udp)) < 0)
    {
        //printf("\nConnection Failed \n");
        result = 0;
    }
    if (getsockname(temp_udp,(struct sockaddr *)&udp,(unsigned int*) &len) == -1)
    {
        perror("getsockname");
        result = 0;
    }
    
    inet_ntop(AF_INET, &(udp.sin_addr), str, len);
    //printf("%s", str);
    
    //Success
    
    return str;
}
int isValidIP(char *ip)
{
    //char *ptr = (char*)malloc(sizeof(char)*MSG_LENGTH);
   char *ptr = ip;
    strcat(ptr,"\0");
    int dots=0;
    
    if(ip == NULL)
        return 0;
    
    ptr = strtok(ptr, ".");
    while(ptr != NULL )
    {
        if (!valid_digit(ptr))
            return 0;
        
        if(atoi(ptr)>=0 && atoi(ptr)<=255)
        {
            ptr = strtok(NULL, ".");
            if (ptr != NULL)
                ++dots;
        }
        else
            return 0;
    }
    if (dots != 3)
        return 0;
    return 1;
}
int valid_digit(char *ip_str)
{
    while (*ip_str) {
        if (*ip_str >= '0' && *ip_str <= '9')
            ++ip_str;
        else
            return 0;
    }
    
    return 1;
}
void broadcast(int sock_index,char *buffer,struct client *head_ref)
{
    //char ip_str[INET_ADDRSTRLEN];
    //strcpy(ip_str,find_ip(ip_str));
   printf("%d\n",sock_index); 
   char *send_to_ip = (char*) malloc(sizeof(char)*INET_ADDRSTRLEN);
   int count =0;
   int i=1,flag=0;
   struct client *temp = head_ref;
   struct client *temp1 = head_ref;
   while(temp!=NULL)
   {
    if(temp->client_fd == sock_index)
    {
     //flag = 1;
     flag = temp->list_id;
     break;
     }
     temp=temp->next;
    }
    temp=temp1;
    while(temp!=NULL){
    if(flag != temp->list_id){
     //printf("temp%d\n",temp->client_fd);
     strcpy(send_to_ip,client_ip[temp->list_id]);
     printf("%s\n",send_to_ip);
     send_to_client(sock_index,send_to_ip,buffer,temp1,2);
     count++;
     //break;
    }
    temp=temp->next;
   }  
}
void send_to_client(int sock_index, char *send_to_ip, char *buffer, struct client *temp,int n)
{
    char *str = (char*)malloc(sizeof(MSG_LENGTH));
    char *str1 = (char*)malloc(sizeof(MSG_LENGTH));
    char *str2 = (char*)malloc(sizeof(MSG_LENGTH));
    char *str3 = (char*)malloc(sizeof(MSG_LENGTH));
    int i =0,sock,flag =0;
    buffer[strlen(buffer)]='\0';
    //str3 = strtok(buffer," ");
    //buffer = buffer(NULL,"\0");
    char *from_ip = (char*)malloc(sizeof(MSG_LENGTH));
    int j;int success = 1;
    //printf("Buffer: %s",buffer);
    struct client *temp1 = temp;
    for(j =1; j<=5; j++)
    {
        if(strcmp(send_to_ip,client_ip[j])==0)
        {
          //printf("\n %d %s", j, client_ip[j]);
            
            break;
        }
    }
while(temp1!= NULL)
                                {
                                    if(temp1->client_fd == sock_index){
                                        strcpy(from_ip,client_ip[temp1->list_id]);
                                        //printf("from_ip%s\n",from_ip);
                                        if(n==0||n==2)
                                         temp1->num_msg_sent++;
                                        }
					temp1 = temp1->next;
                                }
   temp1=temp;
   while(temp1!=NULL)
   {
     if(strcmp(send_to_ip,client_ip[temp1->list_id])==0)
     {
      //printf("In if");
      sock = temp1->client_fd;
     break;
     }
     temp1=temp1->next;
   }
    if(sock == 4)
    {
     for(i=0;i<4;i++)
     {
      if(block4[i]==sock_index){
      //printf("In block\n");
       flag = 1;
       break;
       }
      }
     } 
     else if(sock == 5)
    {
     for(i=0;i<4;i++)
     {
      if(block5[i]==sock_index){
     // printf("In block\n");
       flag = 1;
       break;
       }
      }
     }    
      else if(sock == 6)
    {
     for(i=0;i<4;i++)
     {
      if(block6[i]==sock_index){
       //printf("In block\n");
       flag = 1;
       break;
       }
      }
     }    
      else if(sock == 7)
    {
     for(i=0;i<4;i++)
     {
      if(block7[i]==sock_index){
      //printf("In block\n");
       flag = 1;
       break;
       }
      }
     }       
	if(flag!=1){
        strcat(str,from_ip);
        strcat(str1,from_ip);
        strcat(str1, " ");
        strcat(str, " ");
        strcat(str1, send_to_ip);
        strcat(str1, " ");
        strcat(str, buffer);
        strcat(str1, buffer);
        //printf("Str value: %s",str);
	//strcat(str, "\0")
         while(temp->list_id != j)
           temp = temp->next;
        //printf("\ni'M here %d, %d, %d\n",j,temp->client_fd,temp->client_status);
	if(temp->client_status == 1){
	 //printf("In if");
	 //printf("%d\n",temp->client_fd);
        if(n==0||n==2){
        if(write(temp->client_fd,str,strlen(str)) == -1)
          success = 0;//perror("send");
        else 
        {
         success = 1;
         temp->num_msg_recv++;
         }
    }
    }
    else if(temp->client_status == 0)
    {
     //printf("Client status is zero\n");
     if(temp->client_fd == 4){
     if(logout4++ == 0){
      strcpy(logout_buffer4,str1);
      strcat(logout_buffer4,",");
      //printf("Logout buffer: %s\n", logout_buffer4);
      } 
      else{
      strcat(logout_buffer4,str1);
      strcat(logout_buffer4,",");
      //strcat(logout_buffer4,"\n");
      //printf("%s",logout_buffer4);
      }
      }
     else if(temp->client_fd == 5){
       if(logout5++ == 0){
      strcpy(logout_buffer5,str1);
      strcat(logout_buffer5,",");
      //printf("Logout buffer: %s\n", logout_buffer5);
      } 
      else{
      
      strcat(logout_buffer5,str1);
      strcat(logout_buffer5,",");
      //printf("%s",logout_buffer5);
      }
      //strcpy(logout_buffer5[logout5++],buffer);
     }
     else if(temp->client_fd == 6){
     if(logout6++ == 0){
     //printf("Printing client status: %d\n",temp->client_fd);
      //printf("In logout buffer4\n");
      //printf("STR: %s %d\n",buffer,strlen(buffer));
      strcpy(logout_buffer6,str1);
      strcat(logout_buffer6,",");
      //printf("Logout buffer: %s\n", logout_buffer5);
      } 
      else{
      strcat(logout_buffer6,str1);
      strcat(logout_buffer6,",");
      //printf("%s",logout_buffer5);
      }
     }
     else if(temp->client_fd == 7){
                if(logout7++ == 0){
      strcpy(logout_buffer7,str1);
      strcat(logout_buffer7,",");
      //printf("Logout buffer: %s\n", logout_buffer5);
      } 
      else{
      strcat(logout_buffer7,str1);
      strcat(logout_buffer7,",");
      }
    }
   }
   } 
    if(success == 1 && temp->client_status == 1 && flag ==0 && n!=2)
    {
        printf("[RELAYED:SUCCESS]\n");
        printf("msg from:%s, to:%s\n[msg]:%s\n",from_ip,send_to_ip,buffer);
        printf("[RELAYED:END]\n");
    }
    else if(temp->client_status == 0){}
    else if(flag == 1){
    //printf("None\n");
    }
    else
    {
        printf("[RELAYED:ERROR]\n");
        //printf("msg from:%s, to:%s\n[msg]:%s\n",from_ip,send_to_ip,buffer);
        printf("[RELAYED:END]\n");

    }
       if(success == 1 && n==2)
    {
        printf("[RELAYED:SUCCESS]\n");
        printf("msg from:%s, to:255.255.255.255\n[msg]:%s\n",from_ip,buffer);
        printf("[RELAYED:END]\n");
    }
    else if(success == 0 && n ==2)
    {
        printf("[RELAYED:ERROR]\n");
        printf("[RELAYED:END]\n");
        
    }
}
void blocked(struct client *node,char *buffer){
	//printf("%s",buffer);
int i,j,g=0,f=0,fd,t,num=1;
int found =0;
struct client *temp1 = node;
  for(i=0;i<4;i++){
   for(j=0;j<4-i;j++){
    if(portnoo[j]>portnoo[j+1] && portnoo[j+1]!=0)
    {
     t=portnoo[j];
     portnoo[j]=portnoo[j+1];
     portnoo[j+1]=t;
     f++;
    }
    }
   // if(g==1)
    // break;
    }
    while(temp1!=NULL){
    // printf("In while\n");
        if(strcmp(buffer,client_ip[temp1->list_id])==0){
      //      printf("Found\n");
            //found = 1;
            fd = temp1->client_fd;
           // printf("Client fd %d\n",fd);
            }
            temp1=temp1->next;
        }
    t=adder;
    i=0;j=0;
    struct client *temp = node;
    if(fd == 4)
    {
    // printf("In fd =4\n");
     for(i=0;i<adder;i++)
     {
       //printf("%d\n",i);
       temp = node;
      while(temp!=NULL){
      if(portnoo[i]==temp->port_no && block4[j]==temp->client_fd)
      {
        j++;
        //temp=temp->next;
        printf("%-5d%-35s%-20s%-8d\n",num,temp->hostname,client_ip[temp->list_id],temp->port_no);
      }
      temp=temp->next;
      }
      }
}
if(fd == 5)
    {
    // printf("In fd =4\n");
     for(i=0;i<adder;i++)
     {
       //printf("%d\n",i);
       temp = node;
      while(temp!=NULL){
      if(portnoo[i]==temp->port_no && block5[j]==temp->client_fd)
      {
        j++;
        //temp=temp->next;
        printf("%-5d%-35s%-20s%-8d\n",num,temp->hostname,client_ip[temp->list_id],temp->port_no);
      }
      temp=temp->next;
      }
      }
}
if(fd == 6)
    {
    // printf("In fd =4\n");
     for(i=0;i<adder;i++)
     {
       //printf("%d\n",i);
       temp = node;
      while(temp!=NULL){
      if(portnoo[i]==temp->port_no && block6[j]==temp->client_fd)
      {
        j++;
        //temp=temp->next;
        printf("%-5d%-35s%-20s%-8d\n",num,temp->hostname,client_ip[temp->list_id],temp->port_no);
      }
      temp=temp->next;
      }
      }
}
if(fd == 7)
    {
    // printf("In fd =4\n");
     for(i=0;i<adder;i++)
     {
       //printf("%d\n",i);
       temp = node;
      while(temp!=NULL){
      if(portnoo[i]==temp->port_no && block7[j]==temp->client_fd)
      {
        j++;
        //temp=temp->next;
        printf("%-5d%-35s%-20s%-8d\n",num,temp->hostname,client_ip[temp->list_id],temp->port_no);
      }
      temp=temp->next;
      }
      }
}
}
void blocking(int sock_index,char *buffer,struct client *node){
  char *str = (char*)malloc(sizeof(MSG_LENGTH));
  //printf("Block Buffer: %s\n",buffer);
  str = strtok(buffer,"\n");
  struct client *temp = node;
  int found = 0;
  int i;
  while(temp!=NULL){
  //printf("In while\n");
        if(strcmp(buffer,client_ip[temp->list_id])==0 && temp->block_status ==0){
            //printf("Found\n");
            found = 1;
            temp->block_status = 1;
            break;
            }
            temp=temp->next;
        }
        if(found ==1 ){
        if(sock_index == 4){
             for(i=0;i<4;i++)
             {
              if(block4[i]==0){
               //printf("Block4:%d",block4[i]);
               block4[i]=temp->client_fd;
               //printf("Block4:%d",block4[i]);
               break;
               }
              }
             } 
          else if(sock_index == 5){
             for(i=0;i<4;i++)
             {
              if(block5[i]==0){
               block5[i]=temp->client_fd;
               break;
               }
              } 
             }
              else if(sock_index == 6){
             for(i=0;i<4;i++)
             {
              if(block6[i]==0){
               block6[i]=temp->client_fd;
               break;
               }
              } 
             }
              else if(sock_index == 7){
             for(i=0;i<4;i++)
             {
              if(block7[i]==0){
               block7[i]=temp->client_fd;
               break;
               }
              } 
             }
             }   
        if(found ==1)
        {
            send(sock_index,"1",3,0);
          }
         else{
          send(sock_index,"0",3,0);               
          }
    }
void unblocking(int sock_index,char *buffer,struct client *node){
  char *str = (char*)malloc(sizeof(MSG_LENGTH));
  //printf("Block Buffer: %s\n",buffer);
  str = strtok(buffer,"\n");
  struct client *temp = node;
  int found = 0;
  int i;
  while(temp!=NULL){
  //printf("In while\n");
        if(strcmp(buffer,client_ip[temp->list_id])==0 &&temp->block_status ==1){
            //printf("Found\n");
            found = 1;
            temp->block_status = 0;
            break; 
            }
            temp=temp->next;
        }
        if(found == 1){
        if(sock_index == 4){
             for(i=0;i<4;i++)
             {
              if(block4[i]==temp->client_fd){
               //printf("Block4:%d",block4[i]);
               block4[i]=0;
               //printf("Block4:%d",block4[i]);
               break;
               }
              }
             } 
          else if(sock_index == 5){
             for(i=0;i<4;i++)
             {
              if(block5[i]==temp->client_fd){
               //printf("Block4:%d",block4[i]);
               block5[i]=0;
               break;
               }
              } 
             }
              else if(sock_index == 6){
             for(i=0;i<4;i++)
             {
             if(block6[i]==temp->client_fd){
               //printf("Block4:%d",block4[i]);
               block6[i]=0;
               break;
               }
              } 
             }
              else if(sock_index == 7){
             for(i=0;i<4;i++)
             {
              if(block7[i]==temp->client_fd){
               //printf("Block4:%d",block4[i]);
               block7[i]=0;
               break;
               }
              } 
             }
             }   
        if(found == 0)
        {
            send(sock_index,"20",3,0);
          }
         else{
          send(sock_index,"10",3,0);               
          }
    }
void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
    
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}
void send_port(int listening_port, int server_fd)
{
    char send_port[100];
    //printf("\n String temp %s", port);
    char str_cip[INET_ADDRSTRLEN];
    
    struct sockaddr_in udp;
    int temp_udp =socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int len = sizeof(udp);
    //char str[INET_ADDRSTRLEN];
    int result;
    
    if (temp_udp == -1)
    {
        //printf("Socket creation failed!");
    }
    
    memset((char *) &udp, 0, sizeof(udp));
    udp.sin_family = AF_INET;
    udp.sin_port = htons(EPHEMERAL_PORT);
    inet_pton(AF_INET, "8.8.8.8", &udp.sin_addr);
    //udp.sin_addr.s_addr = inet_addr("8.8.8.8");
    
    if (connect(temp_udp, (struct sockaddr *)&udp, sizeof(udp)) < 0)
    {
        //printf("\nConnection Failed \n");
        result = 0;
    }
    if (getsockname(temp_udp,(struct sockaddr *)&udp,(unsigned int*) &len) == -1)
    {
        perror("getsockname");
        result = 0;
    }
    
    inet_ntop(AF_INET, &(udp.sin_addr), str_cip, len);
    //printf("%s", str);
    

    //strcpy(str_cip,str_cip);
    //printf("\n IP: %s ", str_cip);
    char port[INET_ADDRSTRLEN];
    //printf("\n Listening_fd = %d:", listening_port);
    tostring(port, listening_port);
    //printf("%s\n", port);
    strcat(send_port, "Port");
    strcat(send_port, " ");
    strcat(send_port, str_cip);
    strcat(send_port, " ");
    strcat(send_port, port);
    strcat(send_port, "\n");
    
    if( send(server_fd, send_port, strlen(send_port),0) == -1 )
        perror("Send");
}
void send_list(struct client *head_ref, int fdaccept){
//printf("\n Inside Send");
    struct client *temp = head_ref;
    char buf[1000] = "";
	strcat(buf,"REFRESH ");			//first word of the sent string
	//char idx[5]= "";
	char pr[6]="";
	
	while(temp!=NULL){
		if(temp->client_status == 1){
		//tostring(idx,temp->list_id);		// converting to string
		tostring(pr,temp->port_no);	
		//printf("buf bef: %s\n",buf);		// converting to string
		strcat(buf,client_ip[temp->list_id]);
		strcat(buf," ");
		strcat(buf,temp->hostname);
		strcat(buf," ");
		strcat(buf,pr);
		strcat(buf," ");
		}
		temp = temp->next;
		
	}
	//buf now has REFRESH followeed by <ip> <list id> <client name> <client port number> of all logged in clients
	//strcat(buf,"\n");
	if(send(fdaccept,buf,strlen(buf),0) == -1)
		perror("Couldn't send list");
}

void createclient(char *recv_buf) //this is the function
{
 
                                //printf("inside newlist\n");
				char buf[4][800];
				char *comp;
				
				int i;
				int p_buf;
				int count =0;
				for(i=0;i<4;i++){
					//printf("Client[%d]: %s\n",i,client[i]);
					strcpy(buf[i],clientoo[i]);
					
					if(strcmp(clientoo[i],"0")==0){
					//printf("%d\n",count);
					p[i] = 0;
					break;
				}
				comp = strtok(clientoo[i]," ");
				//printf("comp1: %s\n",clientoo[i]);
				comp = strtok(NULL," ");
				strcpy(buff[i],comp);
				//printf("comp2: %s\n",buff[i]);
				comp = strtok(NULL," ");
				//printf("comp3: %s\n",comp);
				p[i]=atoi(comp);
				//printf("p[%d]: %d\n",i,p[i]);
				count++;
				}
                                
                                 
					  int tempSwap;
						char secTemp[] = "";  
						int j = 0;
						int n=0;
						for(i = 0; i < 4; i++){
						    for(j = 0; j<4-i; j++){
							if(p[j] > p[j+1]&&p[j+1]!=0){
								tempSwap = p[j];
								p[j] = p[j+1];
								p[j+1] = tempSwap;
								strcpy(secTemp, clientoo[j]);
								strcpy(clientoo[j], clientoo[j+1]);
								strcpy(clientoo[j+1], secTemp);
								strcpy(secTemp,buff[j]);
								strcpy(buff[j],buff[j+1]);
								strcpy(buff[j+1],secTemp);
					 			}   
					 		}
						//if(n==1)
						  //  break;
	}
	}				  
void print_cli_list(){
int i=0;
//printf("[LIST:SUCCESS]\n");
while(p[i]!=0)
{

printf("%-5d%-35s%-20s%-8d\n",i+1,buff[i],clientoo[i],p[i]);
i++;
}
//printf("[LIST:END]\n");

}


void refresh(int server_fd){
	char *a = "REFRESH";
	//printf("In refresh\n");
	//printf("%d",server_fd);
	if(send(server_fd,a,strlen(a),0)==-1){
		perror("Couldn't refresh");
		printf("[REFRESH:ERROR]\n");
		printf("[REFRESH:END]\n");}

}         

void print_author(){
printf("[AUTHOR:SUCCESS]\n");
char *ubit_name = "vijayak2";
printf("I, %s, have read and understood the course academic integrity policy.\n", ubit_name);
printf("[AUTHOR:END]\n");
}


int client(struct client **c_ref, int port_no, int listening_fd)
{
    struct sockaddr_in server_addr;
    int server_fd, cmax;
    int logincount = 0, logoutcount = 0;
    fd_set masterlist, watchlist;
    char *cmd= (char*) malloc(sizeof(char)*MSG_LENGTH);
    char *recv_buf= (char*) malloc(sizeof(char)*MSG_LENGTH);
    char *recvbuffer = (char*) malloc(sizeof(char)*MSG_LENGTH);
    char *send_buf= (char*) malloc(sizeof(char)*MSG_LENGTH);
    char *input;
    char *in_port= (char*) malloc(sizeof(char)*MSG_LENGTH);
    char *msg=(char*) malloc(sizeof(char)*MSG_LENGTH);
    char *serverip = (char*) malloc(sizeof(char)*MSG_LENGTH);
    char *clientip;
    size_t nbyte_recvd;
    char *command= (char*) malloc(sizeof(char)*MSG_LENGTH);
    int n;
    
    
    FD_ZERO(&masterlist);
    FD_ZERO(&watchlist);
    FD_SET(0,&masterlist);
    // FD_SET(listening_fd, &masterlist);
    ///cmax = listening_fd;
    cmax =0;
    int server;
    server = listening_fd;
    //server = connect_to_host(client_ip, port_no);
    
    while(1)
    {
        //printf("\n[PA1-Client@CSE489/589]$ ");
        //fflush(stdout);
        watchlist = masterlist;
        int selret = select(cmax+1, &watchlist, NULL, NULL, NULL);
        if( selret == -1)
        {
            perror("select");
        }
        
        if(selret > 0)
        {
            
            for(int i=0; i<=cmax; i++)
            {
                if(FD_ISSET(i, &watchlist))
                {
                    memset(cmd, '\0', MSG_LENGTH);
                    memset(recv_buf, '\0', MSG_LENGTH);
                    if (i == STDIN)
                    {
                        if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
                            exit(-1);
                        strcpy(command,cmd);
                        input = strtok(cmd, " ");
                        
                        if(strcmp(cmd, "AUTHOR\n")==0)
                        {
                            print_author();
                        }
                        if(strcmp(cmd,"IP\n")==0)
                        {
                           char ip_str[INET_ADDRSTRLEN];
                            strcpy(ip_str,find_ip(ip_str));
                          // printf("%s",ip_str);
                           if(ip_str!=NULL)
   			 {
   	  		   printf("[%s:SUCCESS]\n","IP");
        			printf("IP:%s\n", ip_str);
      			  printf("[%s:END]\n","IP");
   			 }
   			 else{
     			   printf("[%s:ERROR]\n", "IP");
     			   printf("[%s:END]\n", "IP");
    				}
                            //Ip_addr();
                        }
                        else if(strcmp(cmd,"LIST\n")==0 && logoutcount == 0)
                        {
                          //  printf("LIST\n");
                          printf("[%s:SUCCESS]\n","LIST");
                            print_cli_list();
                            printf("[%s:END]\n","LIST");
                            //print(*c_ref);
                            }
                        else if(strcmp(cmd,"PORT\n")==0)
                        {
                            print_portno(port_no);
                        }
                          else if(strcmp(cmd, "REFRESH\n")==0 && logoutcount == 0)
                            {
                            //printf("REFRESH...ing\n");
                             refresh(server_fd);
                             printf("[%s:SUCCESS]\n","REFRESH");
                              printf("[%s:END]\n","REFRESH");
                              }
                      
                          
                        else if (strcmp(input ,"LOGIN") == 0)
                        {
                           if(logincount == 0) { 
                           logincount = 1;
                            int count =0, result = 1;
                            server_fd = socket(AF_INET, SOCK_STREAM, 0);
                            if(server_fd < 0)
                            {
                                //printf("Client Socket creation failed\n");
                                exit(EXIT_FAILURE);
                            }
                            else
                            {
                                FD_SET(server_fd, &masterlist);
                                if(server_fd > cmax)
                                cmax = server_fd;
                                //printf("Server_fd: %d\n",server_fd);
                            }
                            
                            while(count != 2)
                            {
                                char *a = (char*) malloc(sizeof(char)*CMD_SIZE);
                                
                                if(count == 0)
                                {
                                    input = strtok(NULL, " ");
                                    strcpy(a,input);
                                    serverip = a;
                                }
                                else if(count == 1)
                                {
                                    
                                    input = strtok(NULL, "\n");
                                    if(input == NULL)
                                        result = 0;
                                    else{
                                        strcpy(a,input);
                                        in_port = a;
                                        break;
                                    }
                                }
                                
                                count +=1;
                            }
                            //Check if IP address is valid
                            char *temp = serverip;
                            //result = isValidIP(temp);
                            
                            if(result == 0)
                            {
                                printf("[%s:ERROR]\n", "LOGIN");
                                printf("[%s:END]\n", "LOGIN");
                            }
                            else{
                                
                                //connect using there args
                                server_addr.sin_family = AF_INET;
                                unsigned int port_temp = atoi(in_port);
                                server_addr.sin_port = htons(port_temp);
                                inet_pton(AF_INET, serverip, &(server_addr.sin_addr));
                                if (connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <0){
                                    perror("Connect Failed");
                                    printf("[%s:ERROR]\n", "LOGIN");
                                    printf("[%s:END]\n", "LOGIN");
                                }
                                
                                else{
                                    //printf("\n Sending port");
                                    printf("[%s:SUCCESS]\n","LOGIN");
                                    printf("[%s:END]\n","LOGIN");
                                    send_port(port_no, server_fd);
                                    //printf("\n%d\n",port_no);//check for portnumber
                                }
                                

                                //printf("[%s:SUCCESS]\n", "LOGIN");
                                //printf("[%s:END]\n", "LOGIN");
                            }
                            //}
                         }
                         else{
                          //printf("Already logged in\n"); 
                          strcpy(cmd,"AAAA ");
                          send(server_fd, cmd, strlen(cmd),0);  
                          logoutcount = 0;
                          printf("[%s:SUCCESS]\n","LOGIN");
                          printf("[%s:END]\n","LOGIN"); 
                        }
                       } 
                        else if(strcmp(input, "SEND")==0 && logoutcount ==0)
                        {
                          //printf("Inside Send\n");
                            int count =0;
                            
                            while(count != 2)
                            {
                                char *a = (char*) malloc(sizeof(char)*MSG_LENGTH);
                                strcpy(a,input);
                                if(count == 0)
                                {
                                    input = strtok(NULL, " ");
                                    strcpy(a,input);
                                    clientip = a;
                                }
                                else if(count == 1)
                                {
                                    input = strtok(NULL, "\n");
                                    strcpy(a,input);
                                    msg = a;
                                    break;
                                }
                                
                                count +=1;
                            }
                            memset(send_buf, '\0', MSG_LENGTH);
                            
                            server_addr.sin_family = AF_INET;
                            server_addr.sin_port = port_no;
                            inet_pton(AF_INET, clientip, &(server_addr.sin_addr));
                            
                            strcat(send_buf,clientip);
                            strcat(send_buf," ");
                            strcat(send_buf,msg);
                            send(server_fd, send_buf, strlen(send_buf),0);

                           
                            
                            
                        }
                        else if(strcmp(input, "BROADCAST")==0 && logoutcount ==0)
                        {
                            //char *a = (char*) malloc(sizeof(char)*MSG_LENGTH);
                            send(server_fd, command, strlen(command),0);
			 	
                            
                        }
                        else if(strcmp(input, "BLOCK")== 0 && logoutcount ==0)
                        {
                           // char *a = (char*) malloc(sizeof(char)*MSG_LENGTH);
                           //printf("Block:%s\n",command);
                           send(server_fd, command, strlen(command),0); 
                      
                            
                            
                        }
                        
                        else if(strcmp(input, "UNBLOCK")== 0 && logoutcount ==0)
                        {
                            // char *a = (char*) malloc(sizeof(char)*MSG_LENGTH);
                            send(server_fd, command, strlen(command),0);
                            
                        }

                        
                        else if(strcmp(cmd,"LOGOUT\n")==0  && logoutcount ==0){
                            cmd = strtok(cmd,"\n");
                            printf((char *)"[%s:SUCCESS]\n","LOGOUT");
                            logoutcount = 1;
                            //printf("Server id: %d\n CMD: %s\n",server_fd,cmd);
                            send(server_fd, cmd, strlen(cmd),0);
                            printf((char *)"[%s:END]\n","LOGOUT");
                            //return 0;
                        }
                        
                        else if(strcmp(cmd, "EXIT\n")==0)
                        {
                            cmd = strtok(cmd,"\n");
                            printf((char *)"[%s:SUCCESS]\n", (char*)"EXIT");
                            //send(server_fd, cmd, strlen(cmd),0);
                            send(server_fd, cmd, strlen(cmd),0);
                            close(server_fd);
                            printf((char *)"[%s:END]\n", (char*)"EXIT");
                            exit(0);
                        }
                        else if(logoutcount ==1){}
                       else{
                        	cmd[strcspn(cmd, "\n")] = 0;
                        	printf("[%s:ERROR]\n",cmd);
                               printf("[%s:END]\n",cmd);
                       
                    }
                        
                        
                        
                    }
                    
                    
                    else
                    {
                        //printf("\n In client receive! ");
                        memset(recv_buf, '\0', MSG_LENGTH);
                        if(i == 0 && i==listening_fd)
                            break;
                        else
                        {
                            nbyte_recvd = recv(i, recv_buf, MSG_LENGTH, 0);
                            //printf("recv buff\n%s\n",recv_buf);
                            recv_buf[nbyte_recvd] = '\0';
                           // printf("nbyte_recvd%d\n",nbyte_recvd);
 			    char *input = (char*) malloc(sizeof(char)*MSG_LENGTH);;
                            strcpy(input, recv_buf);
                            struct client *temp = *c_ref;
                            char *identify = (char*) malloc(sizeof(char)*MSG_LENGTH);
                            strcpy(identify,recv_buf);
                            identify = strtok(identify, " ");
                            //printf("\n %s identify: ", identify);
                            if(strcmp(identify, "REFRESH") == 0)
                            {  
                            	
                                char buffer[300];
                                //strcpy(buffer,recv_buf);
                               // printf("Buffer:%s\n",buffer);
                                     char *str;
					//char *stri;
					int i=0;
					int k=0;
					int j=0;
					int flag=0;
					identify = strtok(NULL," ");
				//	printf("Identify:\n%s",identify);
					for(i = 0; i<4 ;i++){
							flag=0;
							strcpy(clientoo[i],"");
							while(j<3 && flag==0){
							
								strcat(clientoo[i],identify);
								strcat(clientoo[i]," ");
								identify=strtok(NULL," ");
								
								//printf("inside %d %d\n",i,j);
								//printf("client[%d]:%s\n",i,clientoo[i]);
								
								j++; 
								if(j==3){
									j=0;
									flag=1;
									} 
								//printf("identify: %s\n",identify);
								//printf("strlen: %d\n",strlen(identify));
								if(identify=='\0'){
									k=1;
									break;
								    }	
								}
								
							if(k==1)
							{
								//printf("K==1\n");
								break;
							}
					    
					    //printf("outside %d %d\n",i,j);
					    }
					//  printf("i: %d\n",i);
					  i++;
					if(i<4){
						while(i<4){
							strcpy(clientoo[i],"0");
							//printf("Clientoo[%d] %s\n:",i,clientoo[i]); 
							i++;	
								}
							}
							
							
					for(int i = 0; i<4;i++){
					//printf("client[%d]:%s\n",i,clientoo[i]);

					}
                                createclient(recv_buf);
                            }
                            else if(strcmp(identify,"1")==0)
                            {
                             //printf("Blocked\n");
                             printf((char *)"[%s:SUCCESS]\n","BLOCK");
            		      printf((char *)"[%s:END]\n","BLOCK");
                           }  
                           else if(strcmp(identify,"0")==0)
                            {
                             //printf("Blocked\n");
                             printf((char *)"[%s:ERROR]\n","BLOCK");
            		      printf((char *)"[%s:END]\n","BLOCK");
                           }  
                            else if(strcmp(identify,"10")==0)
                            {
                             //printf("Blocked\n");
                             printf((char *)"[%s:SUCCESS]\n","UNBLOCK");
            		      printf((char *)"[%s:END]\n","UNBLOCK");
                           }  
                           else if(strcmp(identify,"20")==0)
                            {
                             //printf("Blocked\n");
                             printf((char *)"[%s:ERROR]\n","UNBLOCK");
            		      printf((char *)"[%s:END]\n","UNBLOCK");
                           }  
                            else
                            {
				char *msg_from = (char*)malloc(sizeof(MSG_LENGTH));
				    char *a = (char*)malloc(sizeof(MSG_LENGTH));
				    char *msg = (char*)malloc(sizeof(MSG_LENGTH));
					
                                int count = 0;
                                /*if(nbyte_recvd>31){
                               // printf("%d
                                 n = nbyte_recvd/2;
                                 strncpy(input,recv_buf,n);
                                 input[n]='\0';
     				  printf("input:%s\n",input);	
                                }*/
                              
                                if(recv_buf!=NULL)
                                {
                                       //printf(" recv_buf %s", input);
							
					input = strtok(input, " ");
    					strcpy(msg_from, input);
    					while(input != NULL)
    					{
        					//printf("\n %s here\n", input);
        					input= strtok(NULL, " ");
        					//printf("input %s\n",input);
       						if(input!=NULL)
        					strcat(msg,input);
        					strcat(msg," ");

    					}

                                    int mo = strlen(msg);
                                    msg[mo-2] = '\0';
                                    //printf("Strlen%d\n",mo);
                                    //printf("Strlen%d\n",mo-2);
                                    printf("[RECEIVED:SUCCESS]\n");
                                    printf("msg from:%s\n[msg]:%.*s\n",msg_from,nbyte_recvd,msg);
                                    printf("[RECEIVED:END]\n");
				
                                }
                                else
                                {
                                    printf("[RECEIVED:ERROR]\n");
                                    printf("msg from:%s\n[msg]:%s\n",msg_from,msg);
                                    printf("[RECEIVED:END]\n");
                                }
                              	
                            }
                        }
                        fflush(stdout);
                    }
                }
            }
        }
    }
    
}

