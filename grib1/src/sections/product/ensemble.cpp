// #include <stdio.h>
// #include <stdlib.h>
// #include "sections/section_1.h"
// #include "def.h"


// void ensemble(unsigned char *pds, int mode) {

//     int pdslen;
//     unsigned char ctmp;
//     char char_end;

//     pdslen = PDS_LEN(pds);
//     char_end = mode == 2 ? ' ' : ':';

//     if ((PDS_Center(pds) == NCEP || ncep_ens) && pdslen >= 45 && pds[40] == 1) {

// 	/* control run */

// 	if (pds[41] == 1) {
// 	    if (mode != 2) {
// 		printf("ens%c0:%c", pds[42] == 1 ? '+' : '-', char_end);
// 	    }
// 	    else {
// 		printf("%s-res_ens_control ", pds[42] == 1 ? "hi" : "low");
// 	    }
// 	}

// 	/* perturbation run */

// 	else if (pds[41] == 2 || pds[41] == 3) {
// 	    if (mode != 2) {
// 	        printf("ens%c%d%c", pds[41] == 3 ? '+' : '-', pds[42],char_end);
// 	    }
// 	    else {
// 		printf("ens_perturbation=%c%d ",pds[41] == 3 ? '+' : '-', 
// 		    pds[42]);
// 	    }
// 	}

// 	/* cluster mean */

// 	else if (pds[41] == 4) {
// 	    if (mode != 2) printf("cluster%c", char_end);
// 	    else printf("cluster(%d members) ",pds[60]);
// 	}


// 	/* ensemble mean */

// 	else if (pds[41] == 5) {
// 	    if (mode != 2) printf("ensemble%c", char_end);
// 	    else printf("ensemble(%d members) ",pds[60]);
// 	}

// 	/* other case .. debug code */

// 	else {
// 		printf("ens %d/%d/%d/%d%c", pds[41],pds[42],pds[43],pds[44],char_end);
// 	}


// 	if (pdslen >= 44) {
// 	    if (pds[43] == 1 && pds[41] >= 4) printf("mean%c", char_end);
// 	    else if (pds[43] == 2) printf("weighted mean%c",char_end);
// 	    else if (pds[43] == 3) printf("no bias%c",char_end);
// 	    else if (pds[43] == 4) printf("weighted mean no bias%c",char_end);
// 	    else if (pds[43] == 5) printf("weight%c",char_end);
// 	    else if (pds[43] == 6) printf("climate percentile%c",char_end);
// 	    else if (pds[43] == 7) printf("daily climate mean%c",char_end);
// 	    else if (pds[43] == 8) printf("daily climate std dev%c",char_end);
// 	    else if (pds[43] == 11) printf("std dev%c",char_end);
// 	    else if (pds[43] == 12) printf("norm std dev%c",char_end);
// 	    else if (pds[43] == 21) printf("max val%c",char_end);
// 	    else if (pds[43] == 22) printf("min val%c",char_end);
// 	}

// 	/* NCEP probability limits */
//     }
// }