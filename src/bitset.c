#ifndef BTST_H
#define BTST_H


//#include <sys/_types/_u_int64_t.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>

#define bitset_t u_int64_t
#define UINT64BIT 64
#define SETNUM_BITS UINT64BIT

#define bitmask(st) (1 << ((st) % SETNUM_BITS))
#define bitwhere(st) ((st) / SETNUMBITS)
#define bitset(in,whch) ((in)[bitwhere(whch)] |= bitmask(whch))
#define bitclear(in,whch) ((in)[bitwhere(whch)] &= ~bitmask(whch))
#define bit_is_set(in,whch) ((in)[bitwhere(whch)] & bitmask(whch))
#define bits_take_up(num) ((num + SETNUM_BITS  - 1) / SETNUM_BITS)
#define is_in_set(in,whch) (bit_is_set((in),(whch)))
#define remove_from_set(in,whch) (bitclear((in),(whch)))
#define add_to_set(in,whch) (bitset((in),(whch)))
#define init_set(in,num) (memset((in),0,bits_take_up((num))))

enum {
	NUL, SOH, STX, ETC, EOT, ENQ, ACK, BEL, BS, TAB, LF, VT, FF, CR,
	SO, SI, DLE, DC1, DC2, DC3, DC4, NAK, SYN, ETB, CAN, EM, SUB, ESC,
	FS, GS, RS, US, SPACE, XPOINT, DBLQ, HSH, DLLRS, PERCNT, AMPER,
	SNGLQ, LPAREN, RPAREN, STAR, PLUS, TCK, MINUS, PERIOD, FWDS, ZERO,
	ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, COLON, SCOLON,
	LESS, EQ, GRT, QUESTM, AT, UPA, UPB, UPC, UPD, UPE, UPF, UPG, UPH, UPI,
	UPJ, UPK, UPL, UPM, UPN, UPO, UPP, UPQ, UPR, UPS, UPT, UPU, UPV, UPW, UPX,
	UPY, UPZ, LBRK, BKS, RBRK, CAROT, UNDS, BTCK, LOWA, LOWB, LOWC, LOWD, LOWE,
	LOWF, LOWG, LOWH, LOWI, LOWJ, LOWK, LOWL, LOWM, LOWN, LOWO, LOWP, LOWQ, LOWR,
	LOWS, LOWT, LOWU, LOWV, LOWW, LOWX, LOWY, LOWZ, LCBCK, VBAR, RCBCK, TILD, DEL
};
const static char ascii_vals[] = { '\0', 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, '\t', '\n',
0xB, '\f', 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A,
0x1B, 0x1C, 0x1D, 0x1E, 0x1F, ' ', '!', '\"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', '\1', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<',
'=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_', '`', 'a', 'b',
'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', '\1', '\0'
};

#define BITS_TO_USE 128
#ifdef DEBUG
int main(int argc, const char* argv[]){
	
	u_int64_t bitarray[bits_take_up(BITS_TO_USE)];
	init_set(bitarray,BITS_TO_USE);
	printf("number of unsigned 64 bit integers needed %d\n",bits_take_up(BITS_TO_USE));
	printf("--ascii array--\n --\n%s\n--\n", &(ascii_vals[1]));
	
}
#endif

#endif