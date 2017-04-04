/*************************************************************************
 > File Name: client.c
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
#include <arpa/inet.h>

#define AWS_TCP_PORT 25391

using namespace std;

int main(int argc, char** argv)
{
/*************************************************************************
 > Function: TCP initialization, create socket and connect.
 > Reference: http://blog.csdn.net/gneveek/article/details/8699198
 ************************************************************************/
    int    sock_fd;
    struct sockaddr_in    aws_addr;

    cout << "The client is up and running." << endl;

    if( argc != 2)
    {
        printf("Missing parameter\n");
        exit(0);
    }

    if(strcmp(argv[1], "max")&&strcmp(argv[1], "min")&&strcmp(argv[1], "sum")&&strcmp(argv[1], "sos"))
    {
        printf("Incorrect reduction type\n");
        exit(0);
    }

    /* create socket */
    if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)       
    {
        printf("Create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    }

    /* create TCP socket address */
    memset(&aws_addr, 0, sizeof(aws_addr));
    aws_addr.sin_family = AF_INET;
    aws_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    aws_addr.sin_port = htons(AWS_TCP_PORT);

    /* connect with socket address */
    if( connect(sock_fd, (struct sockaddr*)&aws_addr, sizeof(aws_addr)) < 0)
    {
        printf("Connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

 /*************************************************************************
 > Function: read the data from csv file.
 > Reference: http://blog.csdn.net/u013232740/article/details/50828062
 ************************************************************************/   
    ifstream fin("nums.csv");                                             // open the csv file
    string file_data;
    int data[1024], sendline[1024], result[1];
    int i,num=0; 
    while (getline(fin, file_data))                                       // read data of each line until eof
    {
        data[num] = atoi(file_data.data());                               // transfer string to int
 //       cout << data[num] << endl;
        num++;
    }
//    cout << num << endl;
    sendline[0] = num;
    for(i=0;i<num;i++)
        sendline[i+1] = data[i];

  /*************************************************************************
 > Function: send data and reduction type, receive result.
 > Reference: http://blog.csdn.net/gneveek/article/details/8699198
 ************************************************************************/ 
    /* send data */   
    if( send(sock_fd, argv[1], strlen(argv[1]), 0) < 0)
    {
        printf("send argv error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    cout << "The client has sent the reduction type " << argv[1] << " to AWS." << endl;

    /* send reduction type */
    if( send(sock_fd, sendline, (num+1)*4, 0) < 0)
    {
        printf("send data error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    cout << "The client has sent " << num << " numbers to AWS." << endl;

    /* receive result */
    recv(sock_fd, result, 4, 0);

    cout << "The client has received reduction type " << argv[1] << ": " << result[0] << endl;
//    cout << result[0] << endl;

//    cout << "send successfully\n" << endl;

    close(sock_fd);
    exit(0);
}
