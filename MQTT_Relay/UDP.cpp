#include "UDP.h"
#include <sys/socket.h>
#include  <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include  <sys/types.h>
#include <cstring>
#include <time.h>       /* time_t, struct tm, time, gmtime */
#include <stdio.h>
#include <unistd.h>     // close
#include <iostream>
#include <string>


// UDP Messages
void UDP::GetUDP_prefix(char *buffer)
{
	char date_time[32];
    time_t t;
	
    /* Retrieve the current time */
    t = time(NULL);
	strftime(date_time, 32, "%c", gmtime(&t));
	sprintf(buffer, "vMaxLoRa-0.10: %s - ", date_time);
}

void UDP::SendUDPMessage_IP(char *buffer, IP_Address *send_to_ip)
{
	int sockfd;
	struct sockaddr_in	 servaddr;
    ssize_t bytes_sent;
	char dest_ip[16];
	
	sprintf(dest_ip, "%d.%d.%d.%d", send_to_ip->ip1, send_to_ip->ip2, send_to_ip->ip3, send_to_ip->ip4);

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) {
		// internet not connected - return
//		perror("socket creation failed");
		return;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(send_to_ip->port);
	if (inet_aton(dest_ip, &servaddr.sin_addr) == 0) {
		// internet not connected - return
		perror("inet_aton failed\n");
		close(sockfd);
		return;
	}

//	cout << "Heartbeat message sent to " << dest_ip << " port " << send_to_ip->port << endl;
		
	bytes_sent = sendto(sockfd, (const char *)buffer, strlen(buffer),
		MSG_CONFIRM, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
	if (bytes_sent == -1)
		perror("Error sending message\n");
		
	close(sockfd);
}

void UDP::SendUDPMessage_DNS(char *buffer, DNS_Name *send_to_dns)
{
	int sockfd;
	struct sockaddr_in	 servaddr;
    ssize_t bytes_sent;
    struct hostent *he;
    
    if ((he = gethostbyname(send_to_dns->dns_name)) == NULL) {
		// internet not connected - return
		// perror("get host by name failed");
		return;
	}
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) {
		perror("socket creation failed");
		return;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(send_to_dns->port);
	memcpy(&servaddr.sin_addr, he->h_addr_list[0], he->h_length);
		
	bytes_sent = sendto(sockfd, (const char *)buffer, strlen(buffer),
		MSG_CONFIRM, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
	if (bytes_sent == -1)
		perror("Error sending message\n");
		
	close(sockfd);
}
