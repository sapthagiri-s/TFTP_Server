#include <stdio.h> // Standard I/O library
#include <sys/stat.h> // File status library
#include <sys/wait.h> // Wait for process termination
#include <fcntl.h> // File control options
#include <string.h> // String manipulation functions
#include <unistd.h> // POSIX API
#include <stdlib.h> // Standard library for memory allocation, process control, etc.
#include <sys/types.h> // Data types used in system calls
#include <sys/socket.h> // Socket programming library
#include <arpa/inet.h> // Internet operations library
#include "Server.h" // Custom header file for server-specific definitions

int main()
{
    tftp_packet *packet; // Pointer to TFTP packet structure
    packet = (tftp_packet *)malloc(sizeof(tftp_packet)); // Allocate memory for TFTP packet
    int sockfd, Connected_flag = 0, read_by_mode = 512; // Socket descriptor, connection flag, and read mode
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Create a UDP socket
    if (sockfd < 0) // Check if socket creation failed
    {
        perror("Socket creation failed"); // Print error message
        exit(1); // Exit program with error
    }
    struct sockaddr_in server_addr; // Structure to hold server address
    server_addr.sin_family = AF_INET; // Set address family to IPv4
    server_addr.sin_port = htons(9091); // Set port number to 9091
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
    printf("INFO: Created relay socket\n"); // Log socket creation
    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)); // Bind socket to address and port
    unsigned int server_addr_len = sizeof(server_addr); // Length of server address structure
    while (1) // Infinite loop to handle client requests
    {
        if (recvfrom(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, &server_addr_len) < 0) // Receive data from client
        {
            perror("Filename receive failed"); // Print error message
            exit(1); // Exit program with error
        }
        else
        {
            if (!Connected_flag) // Check if client is not already connected
            {
                printf("INFO: Client connected\n"); // Log client connection
                Connected_flag = 1; // Set connection flag
            }
            sleep(2); // Delay for 2 seconds
            if (!strcmp(packet->body.request.mode, "octet")) // Check if mode is "octet"
            {
                read_by_mode = 1; // Set read mode to 1
            }
            else if (!strcmp(packet->body.request.mode, "normal")) // Check if mode is "normal"
            {
                read_by_mode = 512; // Set read mode to 512
            }
            else if (!strcmp(packet->body.request.mode, "netascii")) // Check if mode is "netascii"
            {
                read_by_mode = 1; // Set read mode to 1
            }
            printf("INFO: Filename received\n"); // Log filename reception
        }
        printf("Error code: %d\n", packet->body.error_packet.error_code); // Print error code from packet
        if (packet->body.error_packet.error_code == 2) // Check if error code indicates disconnection
        {
            printf("ERROR: Disconnected from client\n"); // Log disconnection
            close(sockfd); // Close socket
            break; // Exit loop
        }
        printf("INFO: File requested: %s\n", packet->body.request.filename); // Log requested filename
        if (packet->opcode == RRQ) // Check if request is a read request
        {
            printf("INFO: Read request received\n"); // Log read request
            send_file(packet, sockfd, server_addr, server_addr_len, read_by_mode); // Call function to send file
        }
        else if (packet->opcode == WRQ) // Check if request is a write request
        {
            printf("INFO: Write request received\n"); // Log write request
            receive_file(packet, sockfd, server_addr, server_addr_len); // Call function to receive file
        }
        else
        {
            printf("ERROR: Invalid request type\n"); // Log invalid request type
        }
    }
    close(sockfd); // Close socket
}