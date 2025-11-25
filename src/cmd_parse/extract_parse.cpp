#include "extract_parse.h"
#include <thread>
#include "cmd_def.h"
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "functions.h"
#include "data/info.h"
#include "proc/extract.h"
#include "code_tables/table_0.h"
#include "out_format_parse.h"
#include "types_parse/array_parse.h"

namespace fs = std::filesystem;

#include <format>
#include <numeric>
#include <time_separation_parse.h>

namespace parse{
    void Extract::init() noexcept{
        add_options("j",po::value<int>()->notifier([this](int input){
            hExtract->set_using_processor_cores(input);
        }),"Number of used threads. Number may be discarded to the maximal physical number threads")
        ("outp",po::value<std::string>()->required()->notifier([this](const std::string& input){
            if(err_==ErrorCode::NONE)
                hExtract->set_out_path(input);
        }),"Output directory where the files with extracted data will be placed")
        ("inp",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& input){
            if(err_==ErrorCode::NONE)
                for(auto& path:input){
                    err_ = hExtract->add_in_path(path);
                    if(err_!=ErrorCode::NONE)
                        return;
                }
        }),"Specify the input paths from where the data could be potentially extracted")
        ("dtfrom",po::value<utc_tp>()->default_value(utc_tp())->notifier([this](const utc_tp& input){
            if(err_==ErrorCode::NONE)
                hExtract->set_from_date(input);
        }),"The date and time (in UTC) from which the data is extracted. Shall be not bigger than value of \"dtto\"")
        ("dtto",po::value<utc_tp>()->default_value(std::chrono::system_clock::now())->notifier([this](const utc_tp& input){
            if(err_==ErrorCode::NONE)
                hExtract->set_to_date(input);
        }),"The date and time (in UTC) up to which the data is extracted. Shall be not less than value of \"dtfrom\"")
        ("pos",po::value<Coord>()->value_name("example: "+boost::lexical_cast<std::string>(Coord{.lat_=0,.lon_=0}))->required()->notifier([this](const Coord& input){
            if(err_==ErrorCode::NONE)
                hExtract->set_position(input);
        }),"Coordinate position by latitude and longitude")
        ("center",po::value<std::vector<std::string>>()->required(),"Specify the center that released the data")
        ("parameters",po::value<std::vector<std::string>>()->multitoken()->required()->notifier([this](const std::vector<std::string>& input){
            if(err_==ErrorCode::NONE){
                for(std::string_view input_part:input)
                    hExtract->add_set_of_parameters(parse::parameter_tv::param_by_tv_abbr(hExtract->get_center().value(),std::ranges::split_view(input_part,' ')|std::ranges::to<std::vector<std::string>>()));
            }
            if(hExtract->get_parameters().empty())
                err_ = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR, "parameter match tokens",AT_ERROR_ACTION::CONTINUE,std::ranges::join_with_view(input,' ')|std::ranges::to<std::string>());
        }),"Specify the expected parameters to process. Use the '[...]' construction with escape words separation for matching the searched center by key words.")
        ("collection",po::value<std::vector<std::string>>()/** @todo*/,"Specify by name of collection")
        ("time_fcst",po::value<std::string>(),"Specify the forecast time of the released data")
        ("grid",po::value<std::vector<std::string>>()->multitoken()->required()->notifier([this](const std::vector<std::string>& input){
            if(err_==ErrorCode::NONE)
                if(auto res = grid_notifier(input);res.has_value())
                    hExtract->set_grid_respresentation(res.value());
                else err_ = res.error();
        }),"Specify the expected grid type. Use the '[...]' construction with escape words separation for matching the searched center by key words.")
        ("ext-out-format",po::value<::OutputDataFileFormats>()->notifier([this](const ::OutputDataFileFormats& input){
            if(err_==ErrorCode::NONE)
                hExtract->set_output_format(input);
        }),"Sets the format of the output file(s) containing the extracted data. \nPossible formats: json,bin,txt. Making \"zip+[choosen format]\" or \"[choosen format]+zip\" will archive the generated extraction files.")
        ("ext-time-period",po::value<TimePeriod>()->default_value(TimePeriod(years(0),
                                                                    months(1),
                                                                    days(0),
                                                                    hours(0),
                                                                    minutes(0),
                                                                    std::chrono::seconds(0)))->notifier([this](const TimePeriod& input){
                                                                        if(err_==ErrorCode::NONE)
                                                                            err_=hExtract->set_offset_time_interval(input);
                                                                    }),
        "Sets the time-period to be extracted in different files. E.g. you choose 1 day time-period separation for period from 1990/01/01 20:25 to 1991/01/03 12:00, \
        all extracted data will be separated by files in the period from 1990/01/01 00:00 to 1990/01/04 00:00, e.g. flooring the indicated time-period separation. If data is not represented as a time series \
        this argument will be ignored for it.");
        define_uniques();
    }

    ErrorCode Extract::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        hExtract = std::make_unique<::Extract>();
        if(!vm.contains("center")){
            err_ = ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"missing --center parameter",AT_ERROR_ACTION::CONTINUE);
            return err_;
        }
        auto center_res = center_notifier(vm.at("center").as<std::vector<std::string>>());
        if(center_res.has_value())
            hExtract->set_center(center_res.value());
        else{
            err_ = center_res.error();
            return err_;
        }
        err_ = try_notify(vm);
        if(err_==ErrorCode::NONE){
            if(err_=hExtract->properties_integrity();err_==ErrorCode::NONE)
                err_ = hExtract->execute();
        }
        if(err_!=ErrorCode::NONE)
            hExtract.reset();
        return err_;
    }
}