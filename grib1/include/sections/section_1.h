#pragma once
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <stddef.h>
#include "sections/def.h"
#include "aux_code/byte_read.h"
#include "code_tables/table_0.h"
#include "code_tables/table_1.h"
#include "code_tables/table_2.h"
#include "code_tables/table_3.h"
#include "code_tables/table_4.h"
#include "code_tables/table_5.h"
#include "sections/product/def.h"
#include "generated/code_tables/include/eccodes_tables.h"

#ifdef __cplusplus
#include <span>
#endif


/* #define LEN_HEADER_PDS (28+42+100) */
#define LEN_HEADER_PDS (28+8)

STRUCT_BEG(ProductDefinitionSection)
{
	unsigned char* buffer_;

	#ifdef __cplusplus
	ProductDefinitionSection(unsigned char* buffer):buffer_(buffer){}
	unsigned section_length() const noexcept{
		return read_bytes<3>(buffer_[0],buffer_[1],buffer_[2]);
	}
	unsigned char table_version() const noexcept{
		return PDS_Vsn(buffer_);
	}
	Organization center() const noexcept{
		return (Organization)PDS_Center(buffer_);
	}
	unsigned char generatingProcessIdentifier() const noexcept{
		return PDS_Model(buffer_);
	}
	unsigned char grid_definition() const noexcept{
		return PDS_Grid(buffer_);
	}
	Section2_3_flag section1Flags() const noexcept{
		return {PDS_HAS_GDS(buffer_),PDS_HAS_BMS(buffer_)};
	}
	unsigned char IndicatorOfParameter() const noexcept{
		return PDS_PARAM(buffer_);
	}
	LevelsTags level() const noexcept{
		return (LevelsTags)PDS_L_TYPE(buffer_);
	}
	unsigned char level1_data() const noexcept{
		return PDS_LEVEL1(buffer_);
	}
	unsigned char level2_data() const noexcept{
		return PDS_LEVEL2(buffer_);
	}
	unsigned char subcenter() const noexcept{
		return PDS_Subcenter(buffer_);
	}
	unsigned char month() const noexcept{
		return PDS_Month(buffer_);
	}
	unsigned char day() const noexcept{
		return PDS_Day(buffer_);
	}
	unsigned char hour() const noexcept{
		return PDS_Hour(buffer_);
	}
	unsigned char minute() const noexcept{
		return PDS_Minute(buffer_);
	}
	TimeFrame unit_time_range() const noexcept{
		return (TimeFrame)PDS_ForecastTimeUnit(buffer_);
	}
	unsigned char year_of_century() const noexcept{
		return PDS_Year(buffer_);
	}
	unsigned char century() const noexcept{
		return PDS_Century(buffer_);
	}
	unsigned short year() const noexcept{
		return PDS_Year4(buffer_);
	}
	unsigned short decimal_scale_factor() const noexcept{
		return PDS_DecimalScale(buffer_);
	}
	unsigned numberMissingFromAveragesOrAccumulations() const noexcept{
		return PDS_NumMissing(buffer_);
	}
	unsigned char numberIncludedInAverage() const noexcept{
		return PDS_NumAve(buffer_);
	}
	const char* parameter() const noexcept{
		return parameter_table(center(),subcenter(),IndicatorOfParameter())->name;
	}
	const char* param_comment() const noexcept{
		return parameter_table(center(),subcenter(),IndicatorOfParameter())->comment;
	}
	#endif
}
STRUCT_END(ProductDefinitionSection)