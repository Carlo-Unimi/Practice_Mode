#include <stdio.h>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "../oscpkt.hh"

#define XR18_IP "192.168.1.xxx"
#define XR18_PORT 10024

//* PROGRAM HEAD
int main(int argc, char **argv[])
{
}