#include "sections/binary/def.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>

void BDS_unpack(float *flt, unsigned char *bds, unsigned char *bitmap,
	int n_bits, int n, double ref, double scale) {

    unsigned char *bits;

    int i, mask_idx, t_bits, c_bits, j_bits;
    unsigned int j, map_mask, tbits, jmask, bbits;
    double jj;
    if (BDS_ComplexPacking(bds)) {
	fprintf(stderr,"*** Cannot decode complex packed fields n=%d***\n", n);
	exit(8);
	memset(flt,-UNDEFINED,n);
	return;
    }

    if (BDS_Harmonic(bds)) {
        bits = bds + 15;
        /* fill in global mean */
		memset(flt,BDS_Harmonic_RefValue(bds),n);
        n -= 1; 
    }
    else {
        bits = bds + 11;  
    }

    tbits = bbits = 0;

    /* assume integer has 32+ bits */
    if (n_bits <= 25) {
        jmask = (1 << n_bits) - 1;
        t_bits = 0;

        if (bitmap) {
	    for (i = 0; i < n; i++) {
		/* check bitmap */
		mask_idx = i & 7;
		if (mask_idx == 0) bbits = *bitmap++;
	        if ((bbits & map_masks[mask_idx]) == 0) {
		    *flt++ = UNDEFINED;
		    continue;
	        }

	        while (t_bits < n_bits) {
	            tbits = (tbits * 256) + *bits++;
	            t_bits += 8;
	        }
	        t_bits -= n_bits;
	        j = (tbits >> t_bits) & jmask;
	        *flt++ = ref + scale*j;
            }
        }
        else {
	    for (i = 0; i < n; i++) {
		if (n_bits - t_bits > 8) {
                    tbits = (tbits << 16) | (bits[0] << 8) | (bits[1]);
		    bits += 2;
                    t_bits += 16;
		}
                while (t_bits < n_bits) {
                    tbits = (tbits * 256) + *bits++;
                    t_bits += 8;
                }
                t_bits -= n_bits;
                flt[i] = (tbits >> t_bits) & jmask;
            }
	    /* at least this vectorizes :) */
	    for (i = 0; i < n; i++) {
		flt[i] = ref + scale*flt[i];
	    }
        }
    }
    else {
	/* older unoptimized code, not often used */
        c_bits = 8;
        map_mask = 128;
        while (n-- > 0) {
	    if (bitmap) {
	        j = (*bitmap & map_mask);
	        if ((map_mask >>= 1) == 0) {
		    map_mask = 128;
		    bitmap++;
	        }
	        if (j == 0) {
		    *flt++ = UNDEFINED;
		    continue;
	        }
	    }

	    jj = 0.0;
	    j_bits = n_bits;
	    while (c_bits <= j_bits) {
	        if (c_bits == 8) {
		    jj = jj * 256.0  + (double) (*bits++);
		    j_bits -= 8;
	        }
	        else {
		    jj = (jj * shift[c_bits]) + (double) (*bits & mask[c_bits]);
		    bits++;
		    j_bits -= c_bits;
		    c_bits = 8;
	        }
	    }
	    if (j_bits) {
	        c_bits -= j_bits;
	        jj = (jj * shift[j_bits]) + (double) ((*bits >> c_bits) & mask[j_bits]);
	    }
	    *flt++ = ref + scale*jj;
        }
    }
    return;
}

float BDS_unpack_val(unsigned char *bds, unsigned char *bitmap,
	int n_bits, int n, double ref, double scale) {

    unsigned char *bits;

    int i, mask_idx, t_bits, c_bits, j_bits;
    unsigned int j, map_mask, tbits, jmask, bbits;
    double jj;
	float flt = UNDEFINED;

    if (BDS_ComplexPacking(bds)) {
		fprintf(stderr,"*** Cannot decode complex packed fields n=%d***\n", n);
		exit(8);
		return flt;
	}

	if (BDS_Harmonic(bds)) {
		bits = bds + 15;
		/* fill in global mean */
		flt = BDS_Harmonic_RefValue(bds);
		n -= 1; 
	}
	else {
		bits = bds + 11;  
	}

	tbits = bbits = 0;

	/* assume integer has 32+ bits */
	if (n_bits <= 25) {
		jmask = (1 << n_bits) - 1;
		t_bits = 0;

		if (bitmap) {

		/* check bitmap */
		bitmap+=n-1;
	/* check bitmap */
			mask_idx = n & 7;
			if (mask_idx == 0) bbits = *bitmap;
			if ((bbits & map_masks[mask_idx]) == 0) {
				return UNDEFINED;
			}

			while (t_bits < n_bits) {
				tbits = (tbits * 256) + *bits++;
				t_bits += 8;
			}
			t_bits -= n_bits;
			j = (tbits >> t_bits) & jmask;
			return ref + scale*j;
			
		}
		else {
			bits += 2*(n-1);
			if (n_bits - t_bits > 8) {
				tbits = (tbits << 16) | (bits[0] << 8) | (bits[1]);
				bits += 2;
				t_bits += 16;
			}
			while (t_bits < n_bits) {
				tbits = (tbits * 256) + *bits++;
				t_bits += 8;
			}
			t_bits -= n_bits;
			flt = (tbits >> t_bits) & jmask;
			return ref + scale*flt;
		/* at least this vectorizes :) */
		}
		return flt;
	}
    // else {
	// /* older unoptimized code, not often used */
    //     c_bits = 8;
    //     map_mask = 128;
    //     while (n-- > 0) {
	//     if (bitmap) {
	//         j = (*bitmap & map_mask);
	//         if ((map_mask >>= 1) == 0) {
	// 	    map_mask = 128;
	// 	    bitmap++;
	//         }
	//         if (j == 0) {
	// 	    *flt++ = UNDEFINED;
	// 	    continue;
	//         }
	//     }

	//     jj = 0.0;
	//     j_bits = n_bits;
	//     while (c_bits <= j_bits) {
	//         if (c_bits == 8) {
	// 	    jj = jj * 256.0  + (double) (*bits++);
	// 	    j_bits -= 8;
	//         }
	//         else {
	// 	    jj = (jj * shift[c_bits]) + (double) (*bits & mask[c_bits]);
	// 	    bits++;
	// 	    j_bits -= c_bits;
	// 	    c_bits = 8;
	//         }
	//     }
	//     if (j_bits) {
	//         c_bits -= j_bits;
	//         jj = (jj * shift[j_bits]) + (double) ((*bits >> c_bits) & mask[j_bits]);
	//     }
	//     *flt++ = ref + scale*jj;
    //     }
    // }
    return UNDEFINED;
}

int BDS_NValues(unsigned char *bds) {

    /* returns number of grid points as determined from the BDS */

    int i = 0;

    if (BDS_SimplePacking(bds) && BDS_Grid(bds)) {
		i = ((BDS_LEN(bds) - BDS_DataStart(bds))*8 -
			BDS_UnusedBits(bds)) / (BDS_NumBits(bds));
    }
    else if (BDS_ComplexPacking(bds) && BDS_Grid(bds)) {
		i = BDS_P2(bds);
    }
    return i;
}