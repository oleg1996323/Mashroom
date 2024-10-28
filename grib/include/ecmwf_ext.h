#pragma once

/*
 * EC_ext	v1.0 wesley ebisuzaki
 *
 * prints something readable from the EC stream parameter
 *
 * prefix and suffix are only printed if EC_ext has text
 */

void EC_ext(unsigned char *pds, char *prefix, char *suffix, int verbose);