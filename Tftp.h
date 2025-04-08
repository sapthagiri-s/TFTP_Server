// TFTP OpCodes
typedef enum
{
    RRQ = 1,  // Read Request
    WRQ = 2,  // Write Request
    DATA = 3, // Data Packet
    ACK = 4,  // Acknowledgment
    ERROR = 5 // Error Packet
} tftp_opcode;

// TFTP Packet Structure
typedef struct
{
    uint16_t opcode; // Operation code (RRQ/WRQ/DATA/ACK/ERROR)
    union
    {
        struct
        {
            char filename[256]; // File name for the request
            char mode[8];       // Transfer mode, typically "octet"
        } request;              // Structure for RRQ and WRQ packets
        struct
        {
            uint16_t block_number; // Block number of the data packet
            int size;              // Size of the data in bytes
            char data[512];        // Actual data being transferred
        } data_packet;            // Structure for DATA packets
        struct
        {
            uint16_t block_number1; // Block number being acknowledged
        } ack_packet;              // Structure for ACK packets
        struct
        {
            uint16_t error_code;    // Error code indicating the type of error
            char error_msg[512];    // Error message describing the error
        } error_packet;             // Structure for ERROR packets
    } body;                        // Union containing different packet types
} tftp_packet;                     // Structure representing a TFTP packet
