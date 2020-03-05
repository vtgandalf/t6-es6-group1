#ifndef _PP_IOMEM_H_ 
#define _PP_IOMEM_H_

/**
 * @brief Read 4 bytes from physical address
 * 
 * @param address   Physical address 
 * @param output    Pointer to where output will be stored 
 * @return int      Success or error
 */
int pp_iomem_read_4_bytes (unsigned long address, unsigned long* output);

/**
 * @brief Write 4 bytes to physical address
 * 
 * @param address   Physical address 
 * @param value     New value
 * @return int      Success or error
 */
void pp_iomem_write_4_bytes (unsigned long address, unsigned long value);

#endif 