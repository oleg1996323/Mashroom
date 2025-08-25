#include "paramtablev.h"

const ParmTable* parameter_table(Organization center, unsigned char table_version, unsigned char param_num){
	return parameter_table((unsigned char)center,table_version,param_num);
}