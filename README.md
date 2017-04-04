# Socket

a. First, boot up all four server programs(AWS,Back-Sever A, B, & C), then listen on the appropriate port for incoming packets/connections. Second, read the numbers from the csv file and send them to the AWS server over a TCP connection, then the AWS server divide the data into 3 non-overlapping components and send that to the 3 back-server, then once a back-server receives the actual numbers and the reduction type, it computes the function value. Third, 3 back-server send the function value back to AWS by UDP, then AWS calculates the final answer to client by TCP.

b. My code files include client.c, aws.c, serverA.c, serverB.c, serverC.c.
client.c: Read the numbers from the csv file, then send them and reduction type to the AWS server over TCP, and receive the final result from AWS server.
aws.c: Receive the data and reduction type from client by TCP, then send the data and reduction type to 3 back-server by UDP, then receive the result from 3 back-server by UDP, then compute the final result, then send the final result to client by TCP.
serverA.c: Receive the data and reduction type from AWS by UDP, then compute the reduction value, then send the result to AWS by UDP.
serverB.c: Receive the data and reduction type from AWS by UDP, then compute the reduction value, then send the result to AWS by UDP.
serverC.c: Receive the data and reduction type from AWS by UDP, then compute the reduction value, then send the result to AWS by UDP.

c. You need to run all these servers aws, serverA, serverB, serverC. Then run client.

d. Message from Client to AWS consists of 2 packets, first contains the function name and the next containing an int array of the number of data and the data.
   Message from AWS to serverA/B/C consists of 2 packets, first contains the function name and the next containing an int array of the number of data and the data.
   Message from serverA/B/C to AWS consists of 1 packet, It contains an int array of the result.
   Message from AWS to Client consists of 1 packet, It contains an int array of the result.	

e. I define many error types under different conditions such as create socket error, connect error, etc. If any error types happen that on-screen messages are not the same as expected, please make all and run again.

f. I read many materials about file read, TCP socket, UDP socket, etc. I read the code from others but write them by myself. I list the references below, and I mention all of them in my code file.
	Read csv file: http://blog.csdn.net/u013232740/article/details/50828062
	TCP: http://blog.csdn.net/gneveek/article/details/8699198
	UDP: http://www.1sohu.com/newsHtm/23/n_49423.shtml
