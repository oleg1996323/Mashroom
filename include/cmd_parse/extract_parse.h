#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "sys/error_code.h"
#include "cmd_parse/cmd_def.h"
#include "proc/extract.h"
#include "time_separation_parse.h"
#include "out_format_parse.h"
#include "cmd_parse/types_parse/center_parse.h"
#include "cmd_parse/types_parse/grid_type_parse.h"
#include "sys/application.h"
#include "proc/extract.h"

namespace parse{
    class Extract:public AbstractCLIParser<parse::Extract>{
        friend AbstractCLIParser;
        Extract(CLI::App* cli):AbstractCLIParser("Extract options"){
            ::Extract hExtract;
            auto* extract = cli->add_subcommand("extract","Extract specified data.");
                extract->require_subcommand(1)->
                alias("-E");
                int jobs = std::thread::hardware_concurrency();
                extract->add_option("-j",jobs,
                "Number of used threads. Number may be discarded to the maximal physical number threads")->
                capture_default_str()->
                default_val(16);
                std::string output_dir=app().config().user_config().current_settings().output_files_root_dir.c_str();
                ///
                extract->
                add_option("--output-dir",output_dir,
                "Output directory where the files with extracted data will be placed")->
                capture_default_str()->each([&hExtract](const std::string& opath){
                    hExtract.set_out_path(opath);
                    
                });
                ///
                extract->add_option_function<std::vector<std::string>>(
                    "--input-paths",[&hExtract](const std::vector<std::string>& paths)
                        {
                            int count = paths.size();
                            for(auto& path:paths){
                                if(auto err = hExtract.add_in_path(path);
                                    err!=ErrorCode::NONE)
                                    ErrorPrint::print_error(err,"",AT_ERROR_ACTION::CONTINUE,path.c_str());
                            }
                            if(count==paths.size())
                                throw CLI::ValidationError("--input-paths","all paths are invalid");
                        },
                    "Specify the input paths from where the data could be potentially extracted");
                ///
                extract->add_option("--dtfrom",);
                ("dtfrom",po::value<std::string>()->default_value(std::format("\"{:%y/%m/%d %H:%M:%S}\"",utc_tp_t<std::chrono::seconds>()))->notifier([this](const std::string& input){
                    if(err_==ErrorCode::NONE)
                        hExtract->set_from_date(boost::lexical_cast<utc_tp_t<std::chrono::seconds>>(input));
                }),"The date and time (in UTC) from which the data is extracted. Shall be not bigger than value of \"dtto\"")
                ("dtto",po::value<std::string>()->default_value(std::format("\"{:%y/%m/%d %H:%M:%S}\"",std::chrono::system_clock::now()))->notifier([this](const std::string& input){
                    if(err_==ErrorCode::NONE)
                        hExtract->set_to_date(boost::lexical_cast<utc_tp_t<std::chrono::seconds>>(input));
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
                ("ext-time-period",po::value<DateTimeDiff>()->default_value([](){
                                                                            std::error_code err;
                                                                            return DateTimeDiff(err,years(0),
                                                                            months(1),
                                                                            days(0),
                                                                            hours(0),
                                                                            minutes(0),
                                                                            std::chrono::seconds(0));}())->notifier([this](const DateTimeDiff& input){
                                                                                if(err_==ErrorCode::NONE)
                                                                                    err_=hExtract->set_offset_time_interval(input);
                                                                            }),
                "Sets the time-period to be extracted in different files. E.g. you choose 1 day time-period separation for period from 1990/01/01 20:25 to 1991/01/03 12:00, \
                all extracted data will be separated by files in the period from 1990/01/01 00:00 to 1990/01/04 00:00, e.g. flooring the indicated time-period separation. If data is not represented as a time series \
                this argument will be ignored for it.");
                
                extract_procedure.
        }

        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
        public:
    };
}