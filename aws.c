/*************************************************************************
 > File Name: aws.c
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
#define SERVER_B_PORT 22391
#define SERVER_C_PORT 23391 
#define AWS_UDP_PORT 24391
#define AWS_TCP_PORT 25391

#define MAXLINE 10000

using namespace std;

int MAX(int *array, int num);
int MIN(int *array, int num);
int SUM(int *array, int num);
int SOS(int *array, int num);

int main(int argc, char** argv)
{
    int    i, n, data_num, data_num_sever, listenfd, connfd;
    struct sockaddr_in     aws_addr;
    int    recvline[10000], buff[1024], buff_A[512], buff_B[512], buff_C[512],result_A[1],result_B[1],result_C[1], result[3],result_FINAL[1];
    char   recvargv[10];

/*************************************************************************
 > Function: TCP initialization, create socket, bind and listen that socket.
 > Reference: http://blog.csdn.net/gneveek/article/details/8699198
 ************************************************************************/
    /* create socket */
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    memset(&aws_addr, 0, sizeof(aws_addr));
    aws_addr.sin_family = AF_INET;
    aws_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    aws_addr.sin_port = htons(AWS_TCP_PORT);

    /* bind socket */
    if( bind(listenfd, (struct sockaddr*)&aws_addr, sizeof(aws_addr)) == -1)
    {
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    /* listen socket */
    if( listen(listenfd, 10) == -1){
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

/*************************************************************************
 > Function: UDP initialization, create socket.
 > Reference: http://www.1sohu.com/newsHtm/23/n_49423.shtml
 ************************************************************************/
    struct sockaddr_in serverA_addr,serverB_addr,serverC_addr;

    /* create socket */
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* server address */
    memset(&serverA_addr, 0, sizeof(serverA_addr));
    serverA_addr.sin_family = AF_INET;
    serverA_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverA_addr.sin_port = htons(SERVER_A_PORT);

    memset(&serverB_addr, 0, sizeof(serverB_addr));
    serverB_addr.sin_family = AF_INET;
    serverB_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverB_addr.sin_port = htons(SERVER_B_PORT);

    memset(&serverC_addr, 0, sizeof(serverC_addr));
    serverC_addr.sin_family = AF_INET;
    serverC_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverC_addr.sin_port = htons(SERVER_C_PORT);

    cout << "The AWS is up and running." << endl;

    while(1)
    {
        memset(recvargv, 0, 10);

/*************************************************************************
 > Function: receive data and reduction type by TCP, data processing.
 > Reference: 
 ************************************************************************/
        if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1)
        {
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }

        n = recv(connfd, recvargv, 10, 0);
//        cout << recvargv << endl;

        n = recv(connfd, recvline, MAXLINE, 0);
//        n = recv(connfd, recvargv, 10, 0);
//        cout << n << endl;
        data_num = recvline[0];
        data_num_sever = data_num/3;
//        cout << data_num << endl;
        cout << "The AWS has received " << data_num << " numbers from the client using TCP over port " << AWS_TCP_PORT << endl;

        for(i=0;i<data_num;i++)
        {
            buff[i] = recvline[i+1];
//            cout << buff[i] << endl;
        }
        
        buff_A[0] = data_num_sever;
        buff_B[0] = data_num_sever;
        buff_C[0] = data_num_sever;
        for(i=0;i<data_num_sever;i++)
        {
            buff_A[i+1] = buff[i];
            buff_B[i+1] = buff[data_num_sever+i];
            buff_C[i+1] = buff[data_num_sever*2+i];
        }

/*************************************************************************
 > Function: send data and reduction type by UDP.
 > Reference: http://www.1sohu.com/newsHtm/23/n_49423.shtml
 ************************************************************************/
        if(socket_fd < 0)
        {
            perror("Create Socket Failed:");
            exit(1);
        }

        if(sendto(socket_fd, buff_A, (data_num_sever+1)*4,0,(struct sockaddr*)&serverA_addr,sizeof(serverA_addr)) < 0)
        {
            perror("Send data to Backend-Server A Failed:");
            exit(1);
        }
        if(sendto(socket_fd, recvargv, 10,0,(struct sockaddr*)&serverA_addr,sizeof(serverA_addr)) < 0)
        {
            perror("Send argv to Backend-Server A Failed:");
            exit(1);
        }

        cout << "The AWS sent " << data_num_sever << " numbers to Backend-Server A." << endl;

        if(sendto(socket_fd, buff_B, (data_num_sever+1)*4,0,(struct sockaddr*)&serverB_addr,sizeof(serverB_addr)) < 0)
        {
            perror("Send data to Backend-Server B Failed:");
            exit(1);
        }
        if(sendto(socket_fd, recvargv, 10,0,(struct sockaddr*)&serverB_addr,sizeof(serverB_addr)) < 0)
        {
            perror("Send argv to Backend-Server B Failed:");
            exit(1);
        }

        cout << "The AWS sent " << data_num_sever << " numbers to Backend-Server B." << endl;

        if(sendto(socket_fd, buff_C, (data_num_sever+1)*4,0,(struct sockaddr*)&serverC_addr,sizeof(serverC_addr)) < 0)
        {
            perror("Send data to Backend-Server C Failed:");
            exit(1);
        }
        if(sendto(socket_fd, recvargv, 10,0,(struct sockaddr*)&serverC_addr,sizeof(serverC_addr)) < 0)
        {
            perror("Send argv to Backend-Server C Failed:");
            exit(1);
        }

        cout << "The AWS sent " << data_num_sever << " numbers to Backend-Server C." << endl;

/*************************************************************************
 > Function: receive results by UDP.
 > Reference: http://www.1sohu.com/newsHtm/23/n_49423.shtml
 ************************************************************************/
        socklen_t serverA_addr_length = sizeof(serverA_addr);

        if(recvfrom(socket_fd, result_A, 4,0,(struct sockaddr*)&serverA_addr, &serverA_addr_length) == -1)
        {
            perror("Receive Data from Backend-Server A Failed:");
            exit(1);
        }

        cout << "The AWS received reduction result of " << recvargv << " from Backend-Server A using UDP over port " << AWS_UDP_PORT << " and it is " << result_A[0] << endl;

        socklen_t serverB_addr_length = sizeof(serverB_addr);

        if(recvfrom(socket_fd, result_B, 4,0,(struct sockaddr*)&serverB_addr, &serverB_addr_length) == -1)
        {
            perror("Receive Data from Backend-Server B Failed:");
            exit(1);
        }

        cout << "The AWS received reduction result of " << recvargv << " from Backend-Server B using UDP over port " << AWS_UDP_PORT << " and it is " << result_B[0] << endl;

        socklen_t serverC_addr_length = sizeof(serverC_addr);

        if(recvfrom(socket_fd, result_C, 4,0,(struct sockaddr*)&serverC_addr, &serverC_addr_length) == -1)
        {
            perror("Receive Data from Backend-Server C Failed:");
            exit(1);
        }

        cout << "The AWS received reduction result of " << recvargv << " from Backend-Server C using UDP over port " << AWS_UDP_PORT << " and it is " << result_C[0] << endl;

//        cout << result[0] << endl;

/*************************************************************************
 > Function: result processing and send the result to client by TCP.
 > Reference: 
 ************************************************************************/
        result[0] = result_A[0];
        result[1] = result_B[0];
        result[2] = result_C[0];

        if (strcmp(recvargv, "max")==0)
        {
            result_FINAL[0] = MAX(result,3);
//            cout << result_FINAL[0] << endl;
        }

        else if(strcmp(recvargv, "min")==0)
        {
            result_FINAL[0] = MIN(result,3);
//            cout << result_FINAL[0] << endl;
        }
        else if(strcmp(recvargv, "sum")==0)
        {
            result_FINAL[0] = SUM(result,3);
//            cout << result_FINAL[0] << endl;
        }
        else if(strcmp(recvargv, "sos")==0)
        {
            result_FINAL[0] = SUM(result,3);
//            cout << result_FINAL[0] << endl;
        }

        cout << "The AWS has successfully finished the reduction " << recvargv << ": " << result_FINAL[0] << endl;

        if( send(connfd, result_FINAL, 4, 0) < 0)
        {
            printf("send data error: %s(errno: %d)\n", strerror(errno), errno);
            exit(0);
        }

        cout << "The AWS has successfully finished sending the reduction value to client." << endl;
            
        close(connfd);
    }
    close(socket_fd);
    close(listenfd);
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
/*
int SOS(int *array, int num)
{
    int sos;
    for(int i=0;i<num;i++)
        sos = sos+ array[i]*array[i];
    return sos;
}
*/
