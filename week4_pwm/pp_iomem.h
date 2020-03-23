#ifndef _PP_IOMEM_H_
#define _PP_IOMEM_H_

int pp_iomem_read_4_bytes (unsigned long address, unsigned long* output);
int pp_iomem_write_4_bytes (unsigned long address, unsigned long value);

#endif