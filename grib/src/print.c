#include <stdio.h>
#include <stdlib.h>
#include "print.h"
#include "PDS.h"
#include "GDS.h"
#include "def.h"

void print_pds(unsigned char *pds, int print_PDS, int print_PDS10, int verbose) {
    int i, j;

    j = PDS_LEN(pds);
    if (verbose < 2) {
        if (print_PDS && verbose < 2) {
            printf(":PDS=");
            for (i = 0; i < j; i++) {
                printf("%2.2x", (int) pds[i]);
            }
        }
        if (print_PDS10 && verbose < 2) {
            printf(":PDS10=");
            for (i = 0; i < j; i++) {
                printf(" %d", (int) pds[i]);
            }
        }
    }
    else {
        if (print_PDS) {
            printf("  PDS(1..%d)=",j);
            for (i = 0; i < j; i++) {
                if (i % 20 == 0) printf("\n    %4d:",i+1);
                printf(" %3.2x", (int) pds[i]);
            }
            printf("\n");
        }
        if (print_PDS10) {
            printf("  PDS10(1..%d)=",j);
            for (i = 0; i < j; i++) {
                if (i % 20 == 0) printf("\n    %4d:",i+1);
                printf(" %3d", (int) pds[i]);
            }
            printf("\n");
        }
    }
}

void print_gds(unsigned char *gds, int print_GDS, int print_GDS10, int verbose) {
    int i, j;

    j = GDS_LEN(gds);
    if (verbose < 2) {
        if (print_GDS && verbose < 2) {
            printf(":GDS=");
            for (i = 0; i < j; i++) {
                printf("%2.2x", (int) gds[i]);
            }
        }
        if (print_GDS10 && verbose < 2) {
            printf(":GDS10=");
            for (i = 0; i < j; i++) {
                printf(" %d", (int) gds[i]);
            }
        }
    }
    else {
        if (print_GDS) {
            printf("  GDS(1..%d)=",j);
            for (i = 0; i < j; i++) {
                if (i % 20 == 0) printf("\n    %4d:",i+1);
                printf(" %3.2x", (int) gds[i]);
            }
            printf("\n");
        }
        if (print_GDS10) {
            printf("  GDS10(1..%d)=",j);
            for (i = 0; i < j; i++) {
                if (i % 20 == 0) printf("\n    %4d:",i+1);
                printf(" %3d", (int) gds[i]);
            }
            printf("\n");
        }
    }
}