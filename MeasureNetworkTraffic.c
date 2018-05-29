/** This function is designed to measure network traffic across a port. 
    The idea is to use this to measure how much data is flowing, for the debugging
	of the ESPA system.

	This function works by having a global variable that simply counts how much
	traffic is coming through a port. Then, there is a signal that gets thrown
	every second that reads the current data value and then resets it accordingly.

	The program is called as follows:

		MeasureNetworkTraffic IP port

	So, for example, if would be called as follows:

		./MeasureNetworkTraffic 192.168.137.1 51001
	
	David Brandman, April 2018
*/

#include "Supersocket.h"
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <ncurses.h>
// This is the total number of bytes that have been read 
int bytesRead = 0;
int numPackets = 0;

Supersocket s = {0};
	char *ip;
	int port;

static void sigHandler(int sig)
{
	clear();
	printw("Listening on: %s, port: %d\n", ip, port);	
	printw("MegaBytes  : %.03f\n", bytesRead / 1000000.0);
	printw("Num Packets: %d\n", numPackets);
	
	int packetLength = bytesRead == 0 ? 0 : bytesRead / numPackets;
		printw("Packet Length: %d\n", packetLength);

	refresh();
	
	bytesRead = 0;	
	numPackets = 0;
}

int main(int argc, char *argv[])
{
	InitializeDisplay(argc, argv);
	SetVerbose(FALSE);
	

	// We first convert the inline arguments to the appropriate values.

	if (argc > 1){
		 ip = argv[1];
	} 
	else
	{
		printf("Please provide an IP address. Exiting!\n");
		exit(1);
	}
 	
	if (argc > 2){
		port = atoi(argv[2]);
	}
	else
	{
		printf("Please provide a port. Exiting!\n");
		exit(1);
	}

	// At this point we invoke Supersocket to get some port listening done

	AddSocket(&s, "DataMeasure", ip, port, AF_INET, SOCK_DGRAM, BIND);

	int bufferSize = 2000;
    char buffer[bufferSize];
	
	// Initialize the Signal business
	if(signal(SIGALRM, sigHandler) == SIG_ERR)
		DisplayError("Could not initialize Signal: %s", strerror(errno));

	// To make this work properly, we want to ignore the ALARM if it happens during the
	// readv() function call that happens as part of Supersocket. Otherwise funky stuff
	// happens if there is a signal thrown during a system call read.

	sigset_t blockSet, prevMask;
	sigemptyset(&blockSet); // Initialize this to be empty
	sigaddset(&blockSet, SIGALRM); // Now set the bit-mask flag to be active for SIGALRM

	// Now define the timer we are going to use here:
	struct itimerval t;
	t.it_interval.tv_sec = 1;
	t.it_interval.tv_usec = 0;
	t.it_value.tv_sec = 1;
	t.it_value.tv_usec = 0;

	if(setitimer(ITIMER_REAL, &t, 0) == -1) {
		DisplayError("Could not initialize timer: %s", strerror(errno));
		exit(1);
	}


	initscr();

	while(1){

		if(sigprocmask(SIG_BLOCK, &blockSet, &prevMask) == -1){
			DisplayError("Could not set signal mask: %s", strerror(errno));
			exit(1);
		}
		if(PollSockets(&s, 0) > 0){
			bytesRead += ReceiveData(&s, &buffer, bufferSize, NULL);
			numPackets++;
		}
		if(sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1){
			DisplayError("Could not reset signal mask: %s", strerror(errno));
			exit(1);
		}
		usleep(10);
	}



	
	CloseSupersocket(&s);
	return 0;
}
