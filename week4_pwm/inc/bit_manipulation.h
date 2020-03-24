#ifndef _BIT_MANIPULATION_H_
#define _BIT_MANIPULATION_H_

#define NR_BITS_IN_A_BYTE               (8)
#define GET_NTH_BIT(v,n)                ((v & (1 << n)) >> n)
#define SET_NTH_BIT(v,n)                (v | (1 << n))
#define CLEAR_NTH_BIT(v,n)              (v & ~(1<<n))

int write_n_bits_from_src_to_dest (unsigned long src, unsigned long* dest, int n, int offset);

#endif 