#include "decode_aux.h"



bool check(char ch, struct VALID_CAP_FMT* valid){
	switch (ch)
	{
	case 'C':
		if(!valid->COORD){
			valid->COORD=true;
			return true;
		}
		break;
	case 'D':
		if(!valid->DAY){
			valid->DAY=true;
			return true;
		}
		break;
	case 'M':
		if(!valid->MONTH){
			valid->MONTH=true;
			return true;
		}
		break;
	case 'H':
		if(!valid->HOUR){
			valid->MONTH=true;
			return true;
		}
		break;
	case 'Y':
		if(!valid->YEAR){
			valid->YEAR=true;
			return true;
		}
		else
		break;
	default:
		return false;
		break;
	}
}