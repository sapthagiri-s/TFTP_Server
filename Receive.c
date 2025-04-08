#include <stdio.h> // Include standard input/output library
#include <sys/stat.h> // Include library for file status
#include <sys/wait.h> // Include library for process waiting
#include <fcntl.h> // Include library for file control options
#include <string.h> // Include library for string manipulation
#include <unistd.h> // Include library for POSIX API
#include <stdlib.h> // Include standard library for memory allocation, etc.
#include <sys/types.h> // Include library for data types
#include <sys/socket.h> // Include library for socket programming
#include <arpa/inet.h> // Include library for internet operations
#include "Server.h" // Include custom header file for server definitions

int receive_file(tftp_packet *packet, int sockfd, struct sockaddr_in server_addr, unsigned int server_addr_len)
{
    // Open the file in create and write-only mode, fail if it already exists
    int fd = open(packet->body.request.filename, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd < 0) // Check if file already exists
    {
        // Open the file in truncate and write-only mode if it exists
        fd = open(packet->body.request.filename, O_TRUNC | O_WRONLY, 0644);
        printf("File is already present. Clearing the previous content.\n"); // Notify user
    }
    else
    {
        close(fd); // Close the file descriptor
        // Reopen the file in create and write-only mode
        fd = open(packet->body.request.filename, O_CREAT | O_WRONLY, 0644);
        printf("Success : Client side file has been created.\n"); // Notify user
    }
    int set_bit = 1, block_number = 0; // Initialize control variables
    while (set_bit) // Loop until set_bit is cleared
    {
        // Receive data packet from the server
        recvfrom(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, &server_addr_len);
        if (packet->opcode == DATA) // Check if the packet is a data packet
        {
            // Write the received data to the file
            write(fd, packet->body.data_packet.data, packet->body.data_packet.size);
            // Update block number for the packet
            packet->body.data_packet.block_number = block_number++;
            printf("Success : Packet %d received\n", packet->body.data_packet.block_number); // Notify user
            // Send acknowledgment back to the server
            sendto(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, server_addr_len);
        }
        else if (packet->opcode == ERROR) // Check if the packet is an error packet
        {
            set_bit = packet->body.error_packet.error_code; // Update set_bit with error code
        }
    }
    close(fd); // Close the file descriptor
    // Send final acknowledgment to the server
    if (sendto(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, server_addr_len) < 0)
    {
        printf("Failure : Acknowledgement not sent\n"); // Notify user of failure
    }
    else
    {
        printf("Success : Acknowledgement sent\n"); // Notify user of success
        printf("Success : File received\n"); // Notify user of file reception
    }
}