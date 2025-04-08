# TFTP Client-Server Implementation

This project implements a TFTP (Trivial File Transfer Protocol) client and server in C. The client and server communicate using UDP sockets to transfer files between them.

## Project Structure

### Key Components

- **TFTP_Server**: Contains the server-side implementation.
  - `Server.c`: Main server logic to handle client requests.
  - `Send_file.c`: Handles sending files to the client.
  - `Receive.c`: Handles receiving files from the client.
  - `Server.h`: Header file for server-specific declarations.
  - `Tftp.h`: Shared TFTP packet structure and opcode definitions.
## Features

- **File Upload**: The client can upload files to the server using the `put` command.
- **File Download**: The client can download files from the server using the `get` command.
- **Modes**: Supports different transfer modes (`octet`, `netascii`, `normal`).
- **Error Handling**: Handles errors such as file not found, invalid requests, and connection issues.

## How to Build and Run

### Prerequisites

- GCC compiler
- Visual Studio Code (optional)
- Linux environment (recommended)

### Build Instructions

1. compile the server:
   ```sh
   gcc -o server Server.c Send_file.c Receive.c
   ./server
   
