/*************************************************************************
 > File Name: serverA.c
 > Author: Chenyu Peng
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
  
#define SERVER_A_PORT 21391
#define AWS_UDP_PORT 24391
#define BUFFER_SIZE 10000

using namespace std;

int MAX(int *array, int num);
int MIN(int *array, int num);
int SUM(int *array, int num);
int SOS(int *array, int num);
  
int main()
{
    int recvline[10000],buff[512],result_FINAL[1];
    int data_num_sever, i;
    char recvargv[10];

/*************************************************************************
 > Function: UDP initialization, create and bind socket.
 > Reference: http://www.1sohu.com/newsHtm/23/n_49423.shtml
 ************************************************************************/
    /* create UDP address*/
    struct sockaddr_in aws_addr;
    memset(&aws_addr, 0, sizeof(aws_addr));
    aws_addr.sin_family = AF_INET;
    aws_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    aws_addr.sin_port = htons(SERVER_A_PORT);

    /* create socket */
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_fd == -1)
    {
        perror("Create Socket Failed:");
        exit(1);
    }

    /* bind socket */
    if(-1 == (bind(socket_fd,(struct sockaddr*)&aws_addr,sizeof(aws_addr))))
    {
        perror("Server Bind Failed:");
        exit(1);
    }

    cout << "The Server A is up and running using UDP on port " << SERVER_A_PORT << "." << endl;

    while(1)
    { 
/*************************************************************************
 > Function: receive data and reduction type by UDP.
 > Reference: http://www.1sohu.com/newsHtm/23/n_49423.shtml
 ************************************************************************/
        struct sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof(client_addr);

        /* receive data and reduction type by UDP */
        if(recvfrom(socket_fd, recvline, BUFFER_SIZE,0,(struct sockaddr*)&client_addr, &client_addr_length) == -1)
        {
            perror("Receive Data Failed:");
            exit(1);
        }
        if(recvfrom(socket_fd, recvargv, 10,0,(struct sockaddr*)&client_addr, &client_addr_length) == -1)
        {
            perror("Receive Argv Failed:");
            exit(1);
        }
//        cout << recvargv << endl;
        data_num_sever = recvline[0];
        cout << "The Server A has received " << data_num_sever << " numbers." << endl;

/*************************************************************************
 > Function: data processing, send result by UDP.
 > Reference: 
 ************************************************************************/
        for(i=0;i<data_num_sever;i++)
        {
            buff[i] = recvline[i+1];
//            cout << buff[i] << endl;
        }

        if (strcmp(recvargv, "max")==0)
        {
            result_FINAL[0] = MAX(buff,data_num_sever);
//            cout << result_FINAL[0] << endl;
        }
        else if(strcmp(recvargv, "min")==0)
        {
            result_FINAL[0] = MIN(buff,data_num_sever);
//            cout << result_FINAL[0] << endl;
        }
        else if(strcmp(recvargv, "sum")==0)
        {
            result_FINAL[0] = SUM(buff,data_num_sever);
//            cout << result_FINAL[0] << endl;
        }
        else if(strcmp(recvargv, "sos")==0)
        {
            result_FINAL[0] = SOS(buff,data_num_sever);
//            cout << result_FINAL[0] << endl;
        }

        cout << "The Server A has successfully finished the reduction " << recvargv << ": " << result_FINAL[0] << endl;

        if(sendto(socket_fd, result_FINAL, 4,0,(struct sockaddr*)&client_addr,sizeof(client_addr)) < 0)
        {
            perror("Send File Name Failed:");
            exit(1);
        }
        cout << "The Server A has successfully finished sending the reduction value to AWS server." << endl;

    }
    close(socket_fd);
    return 0;
}

/*************************************************************************
 > Function: reduction MAX, MIN, SUM,SOS.
 > Reference: 
 ************************************************************************/
int MAX(int *array, int num)
{
    int max = array[0];
    for(int i=0;i<num;i++)
    {
        if (max < array[i])
            max = array[i];
    }
    return max;
}

int MIN(int *array, int num)
{
    int min = array[0];
    for(int i=0;i<num;i++)
    {
        if (min > array[i])
            min = array[i];
    }
    return min;
}

int SUM(int *array, int num)
{
    int sum=0;
    for(int i=0;i<num;i++)
        sum = sum+ array[i];
    return sum;
}

int SOS(int *array, int num)
{
    int sos=0;
    for(int i=0;i<num;i++)
        sos = sos+ array[i]*array[i];
    return sos;
}
