#include "stdlib.h"
#include "stdio.h"
#include "library/include/floattoieee.h"

int wrtieee(float *array, int n, int header, FILE *output) {

	unsigned long int l;
	int i, nbuf;
	unsigned char buff[BUFSIZ];
	unsigned char h4[4];

	nbuf = 0;
	h4[0] = h4[1] =  h4[2] = h4[3] = 0;
	if (header) {
		l = n * 4;
		for (i = 0; i < 4; i++) {
			h4[i] = l & 255;
			l >>= 8;
		}
		buff[nbuf++] = h4[3];
		buff[nbuf++] = h4[2];
		buff[nbuf++] = h4[1];
		buff[nbuf++] = h4[0];
	}
	for (i = 0; i < n; i++) {
		if (nbuf >= BUFSIZ) {
		    fwrite(buff, 1, BUFSIZ, output);
		    nbuf = 0;
		}
		flt2ieee(array[i], buff + nbuf);
		nbuf += 4;
	}
	if (header) {
		if (nbuf == BUFSIZ) {
		    fwrite(buff, 1, BUFSIZ, output);
		    nbuf = 0;
		}
		buff[nbuf++] = h4[3];
		buff[nbuf++] = h4[2];
		buff[nbuf++] = h4[1];
		buff[nbuf++] = h4[0];
	}
	if (nbuf) fwrite(buff, 1, nbuf, output);
	return 0;
}

/* write a big-endian 4 byte integer .. f77 IEEE  header */

int wrtieee_header(unsigned int n, FILE *output) {
	unsigned h4[4];

	h4[0] = n & 255;
	h4[1] = (n >> 8) & 255;
	h4[2] = (n >> 16) & 255;
	h4[3] = (n >> 24) & 255;

	putc(h4[3],output);
	putc(h4[2],output);
	putc(h4[1],output);
	putc(h4[0],output);

	return 0;
}