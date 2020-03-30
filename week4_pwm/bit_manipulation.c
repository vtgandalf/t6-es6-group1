#include "bit_manipulation.h"

#include <linux/kernel.h>				// kernel work
#include <linux/module.h>				// required for module

#include "es6_generic.h"				// for generic defines

/************************************************************
*	Public functions
************************************************************/

int write_n_bits_from_src_to_dest (unsigned long src, unsigned long* dest, int n, int offset)
{
	if (dest == NULL)
		return -EINVAL;
    
  int bit_pos = 0;

  for (bit_pos = 0; bit_pos < n; bit_pos++)
  {
    int dest_pos = bit_pos + offset;
    
    uint8_t src_value = GET_NTH_BIT(src, bit_pos);
    
    *dest = (src_value == 0) ?
            CLEAR_NTH_BIT(*dest, dest_pos) :
            SET_NTH_BIT(*dest, dest_pos);
  }

	return SUCCESS;
}