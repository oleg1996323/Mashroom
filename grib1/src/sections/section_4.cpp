#include "sections/section_4.h"
#include "types/coord.h"


float BDS_unpack_val(unsigned char *bds, unsigned char *bitmap,
        int n_bits, int n, double ref, double scale) {

    unsigned char *bits;

    int i, mask_idx, t_bits, c_bits, j_bits;
    unsigned int j, map_mask, tbits, jmask, bbits;
    double jj;
	float flt = UNDEFINED;

    if (BDS_ComplexPacking(bds)) {
        //https://codes.ecmwf.int/grib/format/grib1/packing/3/
        //https://codes.ecmwf.int/grib/format/grib1/packing/4/
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