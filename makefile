all: client.c aws.c serverA.c serverB.c serverC.c
	g++ -o client client.c -lsocket -lnsl -lresolv 
	g++ -o aws_o aws.c -lsocket -lnsl -lresolv
	g++ -o serverA_o serverA.c -lsocket -lnsl -lresolv
	g++ -o serverB_o serverB.c -lsocket -lnsl -lresolv
	g++ -o serverC_o serverC.c -lsocket -lnsl -lresolv

serverA:
	./serverA_o

serverB:
	./serverB_o

serverC:
	./serverC_o

aws:
	./aws_o
