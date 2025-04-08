#include <stdio.h> // Include standard input/output library
#include <sys/stat.h> // Include library for file status
#include <sys/wait.h> // Include library for process waiting
#include <fcntl.h> // Include library for file control options
#include <string.h> // Include library for string manipulation
#include <unistd.h> // Include library for POSIX API
#include <stdlib.h> // Include library for standard utilities
#include <sys/types.h> // Include library for data types
#include <sys/socket.h> // Include library for socket programming
#include <arpa/inet.h> // Include library for internet operations
#include "Server.h" // Include custom server header file

int send_file(tftp_packet *packet, int sockfd, struct sockaddr_in server_addr, unsigned int server_addr_len, int read_by_mode)
{
    int fd; // File descriptor
    fd = open(packet->body.request.filename, O_EXCL | O_CREAT | O_RDONLY, 0666); // Try to open file exclusively
    if (fd < 0) // Check if file already exists
    {
        printf("INFO: File already exists\n"); // Log file existence
        close(fd); // Close file descriptor
        fd = open(packet->body.request.filename, O_RDONLY); // Open file in read-only mode
        if (fd < 0) // Check if file open failed
        {
            perror("File open failed"); // Print error message
            exit(1); // Exit program
        }
        printf("INFO: File opened\n"); // Log file opened
        packet->body.error_packet.error_code = 6; // Set error code for file existence
        strcpy(packet->body.error_packet.error_msg, "File already exists"); // Set error message
        packet->opcode = ACK; // Set opcode to ACK
        sendto(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, server_addr_len); // Send acknowledgment
        if (recvfrom(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, &server_addr_len) < 0) // Receive acknowledgment
        {
            perror("Acknowledgement receive failed"); // Print error message
            exit(1); // Exit program
        }
        else
        {
            printf("INFO: Acknowledgement received\nINFO: File transfer started\n"); // Log acknowledgment and start of transfer
            int bytes_read; // Variable to store bytes read
            int pos = 0, flag = 0; // Position and flag variables
            char *buffer = malloc(read_by_mode); // Allocate memory for buffer
            while (bytes_read = read(fd, buffer, read_by_mode)) // Read file in chunks
            {
                packet->opcode = DATA; // Set opcode to DATA
                packet->body.data_packet.size = bytes_read; // Set data packet size
                if (!strcmp(packet->body.request.mode, "netascii")) // Check if mode is netascii
                {
                    flag = 1; // Set flag for netascii mode
                    if (pos == 512) // Check if position is 512
                    {
                        packet->body.data_packet.size = 512; // Set data packet size to 512
                        pos = 0; // Reset position
                        for (int i = 0; i < packet->body.data_packet.size; i++) // Iterate through buffer
                        {
                            if (buffer[i] == '\n') // Check for newline character
                            {
                                buffer[i] = '\r'; // Replace with carriage return
                                buffer = realloc(buffer, bytes_read); // Reallocate buffer
                                buffer[i] = '\n'; // Add newline character
                            }
                        }
                        memcpy(packet->body.data_packet.data, buffer, bytes_read); // Copy buffer to data packet
                        sendto(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, server_addr_len); // Send data packet
                        recvfrom(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, &server_addr_len); // Receive acknowledgment
                        printf("INFO: Packet %d sent\n", packet->body.data_packet.block_number); // Log packet sent
                    }
                }
                if (!flag) // If not netascii mode
                {
                    memcpy(packet->body.data_packet.data, buffer, bytes_read); // Copy buffer to data packet
                    sendto(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, server_addr_len); // Send data packet
                    recvfrom(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, &server_addr_len); // Receive acknowledgment
                    printf("INFO: Packet %d sent\n", packet->body.data_packet.block_number); // Log packet sent
                }
                pos++; // Increment position
            }
            if (flag) // If netascii mode
            {
                memcpy(packet->body.data_packet.data, buffer, pos); // Copy remaining buffer to data packet
                packet->body.data_packet.size = pos; // Set data packet size
                sendto(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, server_addr_len); // Send data packet
                recvfrom(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, &server_addr_len); // Receive acknowledgment
                printf("INFO: Packet %d sent\n", packet->body.data_packet.block_number); // Log packet sent
            }
            free(buffer); // Free allocated buffer
            close(fd); // Close file descriptor
            packet->opcode = ERROR; // Set opcode to ERROR
            packet->body.error_packet.error_code = 0; // Set error code to 0
            sendto(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, server_addr_len); // Send error packet
            printf("INFO: File transfer completed\n"); // Log file transfer completion
        }
        if (recvfrom(sockfd, packet, sizeof(tftp_packet), 0, (struct sockaddr *)&server_addr, &server_addr_len) < 0) // Receive acknowledgment
        {
            printf("INFO: Acknowledgement received\n"); // Log acknowledgment received
        }
    }
    else
        sendto(sockfd, "failure", sizeof("failure"), 0, (struct sockaddr *)&server_addr, server_addr_len); // Send failure message
}
