#ifndef _PEEK_AND_POKE_H_
#define _PEEK_AND_POKE_H_

/** Generic **/
#define SUCCESS                 0       // for return values
#define ERROR                   -1

#define REGISTER_SIZE           4       // 1 register size (in bytes)
#define HEXADECIMAL_BASE	    16      // hexadecimal base 
#define TRUE                    1
#define FALSE                   0

/** Protocol-related **/
#define PROTOCOL_INDEX_OP       0
#define PROTOCOL_INDEX_ADDR     1
#define PROTOCOL_INDEX_DATA     2
#define PROTOCOL_DELIM	        " "     
#define PROTOCOL_MAX_INDEX      3  
#define PROTOCOL_READ_OP        'r'
#define PROTOCOL_WRITE_OP       'w'

/** Sysfs-related **/
#define SYSFS_DIR               "lpc_comm"
#define SYSFS_CMD_FILE          "lpc_cmd"
#define SYSFS_MAX_BUF_LEN       1024
#define SYSFS_MAX_INPUT_LEN     100         // should be enough for a 32 bit address string

/** @brief Struct to store input parameters */
typedef struct input_param_t {
    /** @brief The operation (r/w) */
	char 			op; 
    /** @brief The operation (r/w) */
	unsigned long 	addr;
    /** @brief Data string */
	char           data[SYSFS_MAX_INPUT_LEN];
} input_param;



#endif 