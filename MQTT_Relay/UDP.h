#ifndef UDP_H
#define UDP_H

using namespace std;

#define UDP_MAXLINE 1024

class DNS_Name {
	public:
        char dns_name[100];
	    int port;
};

class IP_Address {
    public:
	    int ip1;
	    int ip2;
	    int ip3;
	    int ip4;
	    int port;
};

class UDP
{
    public:
        void SendUDPMessage_DNS(char *buffer, DNS_Name *send_to_dns);
        void SendUDPMessage_IP(char *buffer, IP_Address *send_to_ip);
        
    private:
        void GetUDP_prefix(char *buffer);
};

#endif // UDP_H
