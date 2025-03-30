#pragma once

/*
 * returns pointer to the parameter table
 */

// /static const struct ParmTable *Parm_Table(unsigned char *pds);

/*
 * return name field of PDS_PARAM(pds)
 */

char *k5toa(unsigned char *pds);

/*
 * return comment field of the PDS_PARAM(pds)
 */

char *k5_comments(unsigned char *pds);

typedef enum{
    WMO,
    ETC
}Center;

#include "sections/section_1.h"

const ParmTable *Parm_Table(const ProductDefinitionSection& data);