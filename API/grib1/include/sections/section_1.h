#pragma once
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <stddef.h>
#include "sections/def.h"
#include "byte_read.h"
#include "code_tables/table_0.h"
#include "code_tables/table_1.h"
#include "code_tables/table_2.h"
#include "sections/product/levels.h"
#include "code_tables/table_4.h"
#include "code_tables/table_5.h"
#include "sections/product/def.h"
#include "generated/code_tables/eccodes_tables.h"
#include <chrono>
#include <span>
#include "types/time_interval.h"
#include "types/time_period.h"
#include "product/time_forecast.h"

/* #define LEN_HEADER_PDS (28+42+100) */
#define LEN_HEADER_PDS (28+8)
using namespace std::chrono;
using parameter_t = uint8_t;
using table_version_t = uint8_t;

struct ProductDefinitionSection
{
	unsigned char* buffer_;

	ProductDefinitionSection(unsigned char* buffer):buffer_(buffer){}
	unsigned section_length() const noexcept;
	unsigned char table_version() const noexcept;
	Organization center() const noexcept;
	unsigned char generatingProcessIdentifier() const noexcept;
	unsigned char grid_definition() const noexcept;
	Section2_3_flag section1Flags() const noexcept;
	parameter_t parameter_number() const noexcept;
	LevelsTags level() const noexcept;
	uint8_t level1_data() const noexcept;
	uint8_t level2_data() const noexcept;
	Level level_data() const noexcept;
	unsigned char subcenter() const noexcept;
	unsigned char month() const noexcept;
	unsigned char day() const noexcept;
	unsigned char hour() const noexcept;
	unsigned char minute() const noexcept;
	std::chrono::system_clock::time_point reference_time() const noexcept{
		return sys_days(year_month_day(std::chrono::year(year())/month()/day())) + hh_mm_ss(hours(hour())+minutes(minute())).to_duration();
	}
	TimeFrame unit_time_range() const noexcept;
	TimeForecast time_forecast() const noexcept;
	unsigned char year_of_century() const noexcept;
	unsigned char century() const noexcept;
	unsigned short year() const noexcept;
	unsigned short decimal_scale_factor() const noexcept;
	unsigned char period1() const noexcept;
	unsigned char period2() const noexcept;
	TimeRangeIndicator time_range_indicator() const noexcept;
	unsigned short numberMissingFromAveragesOrAccumulations() const noexcept;
	unsigned char numberIncludedInAverage() const noexcept;
	std::string_view parameter_name() const noexcept;
	std::string_view param_comment() const noexcept;
};

