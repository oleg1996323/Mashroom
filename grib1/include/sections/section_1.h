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
#include "generated/code_tables/eccodes_tables.h"
#include "types/date.h"
#include <chrono>
#include <span>


/* #define LEN_HEADER_PDS (28+42+100) */
#define LEN_HEADER_PDS (28+8)
using namespace std::chrono;
struct ProductDefinitionSection
{
	unsigned char* buffer_;

	#ifdef __cplusplus
	ProductDefinitionSection(unsigned char* buffer):buffer_(buffer){}
	unsigned section_length() const noexcept;
	unsigned char table_version() const noexcept;
	Organization center() const noexcept;
	unsigned char generatingProcessIdentifier() const noexcept;
	unsigned char grid_definition() const noexcept;
	Section2_3_flag section1Flags() const noexcept;
	unsigned char IndicatorOfParameter() const noexcept;
	LevelsTags level() const noexcept;
	unsigned char level1_data() const noexcept;
	unsigned char level2_data() const noexcept;
	unsigned char subcenter() const noexcept;
	unsigned char month() const noexcept;
	unsigned char day() const noexcept;
	unsigned char hour() const noexcept;
	unsigned char minute() const noexcept;
	std::chrono::system_clock::time_point date() const noexcept{
		return sys_days(year_month_day(std::chrono::year(year())/month()/day())) + hh_mm_ss(hours(hour())).to_duration();
	}
	TimeFrame unit_time_range() const noexcept;
	unsigned char year_of_century() const noexcept;
	unsigned char century() const noexcept;
	unsigned short year() const noexcept;
	unsigned short decimal_scale_factor() const noexcept;
	unsigned numberMissingFromAveragesOrAccumulations() const noexcept;
	unsigned char numberIncludedInAverage() const noexcept;
	std::string_view parameter_name() const noexcept;
	std::string_view param_comment() const noexcept;
	#endif
};