#include "Tftp.h" // Include the header file for TFTP-related definitions and functions

// Function to send a file using TFTP protocol
int send_file(tftp_packet *packet, int sockfd, struct sockaddr_in server_addr, unsigned int server_addr_len, int read_by_mode);

// Function to receive a file using TFTP protocol
int receive_file(tftp_packet *packet, int sockfd, struct sockaddr_in server_addr, unsigned int server_addr_len);