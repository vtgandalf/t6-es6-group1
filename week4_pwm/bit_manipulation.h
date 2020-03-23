#ifndef _BIT_MANIPULATION_H_
#define _BIT_MANIPULATION_H_

#define NR_BITS_IN_A_BYTE               (8)
#define GET_NTH_BIT(v,n)                ((v & (1 << n)) >> n)
#define SET_NTH_BIT(v,n)                (v | (1 << n))
#define CLEAR_NTH_BIT(v,n)              (v & ~(1<<n))

#endif 