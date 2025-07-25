#include "extract_parse.h"
#include <thread>
#include "cmd_def.h"
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "functions.h"
#include "data/info.h"
#include "proc/extract.h"
#include "code_tables/table_0.h"
#include "search_process_parse.h"
#include "out_format_parse.h"

namespace fs = std::filesystem;

#include <format>
#include <numeric>
#include <time_separation_parse.h>

namespace parse{
    void Extract::init() noexcept{
        add_options("j",po::value<int>()->notifier([this](int input){
            hExtract->set_using_processor_cores(input);
        }),"Number of used threads. Number may be discarded to the maximal physical number threads")
        ("outp",po::value<std::vector<std::string>>()->required()->notifier([this](const std::vector<std::string>& paths){
            assert(hExtract);
            for(auto& path:paths)
                if(err_ = hExtract->add_in_path(path);err_!=ErrorCode::NONE)
                    return;
        }),"Output directory where the files with extracted data will be placed")
        ("inp",po::value<std::vector<std::string>>()->required()->notifier([this](const std::vector<std::string>& paths) noexcept{
            assert(hExtract);
            for(auto& path:paths){
                err_ = hExtract->add_in_path(path);
                if(err_!=ErrorCode::NONE)
                    return;
            }
        }),"Specify the input paths from where the data could be potentially extracted")
        ("dtfrom",po::value<utc_tp>()->default_value(utc_tp())->notifier([this](utc_tp input){
            hExtract->set_from_date(input);
        }),"The date and time (in UTC) from which the data is extracted. Shall be not bigger than value of \"dtto\"")
        ("dtto",po::value<utc_tp>()->default_value(std::chrono::system_clock::now())->notifier([this](utc_tp input){
            hExtract->set_to_date(input);
        }),"The date and time (in UTC) up to which the data is extracted. Shall be not less than value of \"dtfrom\"")
        ("pos",po::value<Coord>()->required()->notifier([this](const Coord& input){
            hExtract->set_position(input);
        }),"Coordinate position (latitude-longitude)")
        ("center",po::value<std::string>()->required(),"Specify the center that released the data")
        ("tabv-param",po::value<std::vector<SearchParamTableVersion>>()->notifier(
            [this](const std::vector<SearchParamTableVersion>& input){
                for(auto& param_tv:input)
                    hExtract->add_parameter(param_tv);
        }),"Specify the expected parameters to process")
        ("tabv-pname",po::value<std::vector<std::string>>()->notifier(
            [this](const std::vector<std::string>& input){
                if(!hExtract->get_center().has_value())
                    err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,
                    "undefined center (must be previously defined)",AT_ERROR_ACTION::ABORT);
                for(auto& param_tv:input){
                    auto param = parse::parameter_tv::param_by_tv_abbr(hExtract->get_center().value(),param_tv);
                    if(!param.has_value()){
                        err_=param.error();
                        return;
                    }
                    else hExtract->add_parameter(param.value());
                }
        }),"Specify the expected parameters to process by table version and parameter name")
        ("collection",po::value<std::vector<std::string>>()/** @todo*/,"Specify by name of collection")
        ("time_fcst",po::value<std::string>(),"Specify the forecast time of the released data")
        ("grid",po::value<RepresentationType>()->required()->notifier([this](RepresentationType input){
            hExtract->set_grid_respresentation(input);
        }),"Specify the expected grid type")
        ("ext-out-format",po::value<::OutputDataFileFormats>()->
                            notifier([this](::OutputDataFileFormats input){
                            hExtract->set_output_format(input);
        }),"Sets the format of the output file(s) containing the extracted data")
        ("ext-time-interval",po::value<TimeOffset>()->default_value(TimeOffset(years(0),
                                                                    months(1),
                                                                    days(0),
                                                                    hours(0),
                                                                    minutes(0),
                                                                    std::chrono::seconds(0)))->notifier([this](const auto& input){
            hExtract->set_offset_time_interval(input);
        }),"Sets the time interval to be extracted in different files. E.g. you choose 1 day time-interval, all extracted data will be separated by files");
        define_uniques();
    }
}