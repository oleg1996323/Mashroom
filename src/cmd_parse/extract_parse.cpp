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
        ("datefrom",po::value<utc_tp>()->default_value(utc_tp())->notifier([this](utc_tp input){
            hExtract->set_from_date(input);
        }),"")
        ("dateto",po::value<utc_tp>()->default_value(std::chrono::system_clock::now())->notifier([this](utc_tp input){
            hExtract->set_to_date(input);
        }),"")
        ("pos",po::value<Coord>()->required()->notifier([this](const Coord& input){
            hExtract->set_position(input);
        }),"")
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

// ErrorCode extract_parse(const std::vector<std::string_view>& input){
//     Extract hExtract;
//     std::vector<std::pair<uint8_t,std::string_view>> aliases_parameters;
//     for(size_t i=0;i<input.size();++i){
//         switch (translate_from_txt<translate::token::Command>(input[i++]))
//         {
//             case translate::token::Command::EXTRACT_FORMAT:{
//                 auto tmp = get_extract_format(input[i]);
//                 if(!tmp.has_value())
//                     return ErrorCode::COMMAND_INPUT_X1_ERROR;
//                 hExtract.set_output_format(tmp.value());
//                 break;
//             }
//             case translate::token::Command::EXTRACTION_TIME_INTERVAL:{
//                 auto err = time_separation_parse(input[i],hExtract);
//                 if(err!=ErrorCode::NONE)
//                     return err;
//                 break;
//             }
//             default:{
//                 auto err = search_process_parse(input[i-1],input[i],hExtract,aliases_parameters);
//                 ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid argument: argv["+std::string(input[i])+"]",AT_ERROR_ACTION::CONTINUE,input[i]);
//                 return ErrorCode::COMMAND_INPUT_X1_ERROR;
//                 break;
//             }
//         }
//     }
//     if(!hExtract.get_center().has_value()){
//         if(!aliases_parameters.empty()){
//             std::set<int> error_pos;
//             auto result = post_parsing_parameters_aliases_def(hExtract.get_center().value(),aliases_parameters,error_pos);
//             if(!error_pos.empty()){
//                 std::string error_aliases;
//                 for(int pos:error_pos)
//                     error_aliases+=""s+aliases_parameters.at(pos).second.data()+",";
//                 error_aliases.pop_back();
//                 ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Parameter alias definition error",AT_ERROR_ACTION::CONTINUE,error_aliases);
//                 return ErrorCode::COMMAND_INPUT_X1_ERROR;
//             }
//             else{
//                 for(auto& param:result){
//                     hExtract.add_parameter(std::move(param));
//                 }
//             }
//         }
//     }
//     else{
//         ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Center",AT_ERROR_ACTION::CONTINUE);
//         return ErrorCode::UNDEFINED_VALUE;
//     }
//     ErrorCode err = hExtract.properties_integrity();
//     if(err!=ErrorCode::NONE)
//         return err;
//     return hExtract.execute();
// }

// std::vector<std::string_view> commands_from_extract_parse(const std::vector<std::string_view>& input,ErrorCode& err){
//     std::vector<std::string_view> commands;
//     std::vector<std::pair<uint8_t,std::string_view>> aliases_parameters;
//     Extract hExtract;
//     commands.push_back(translate_from_token(translate::token::ModeArgs::EXTRACT));
//     for(size_t i=0;i<input.size();++i){
//         switch (translate_from_txt<translate::token::Command>(commands.emplace_back(input[i++])))
//         {
//             case translate::token::Command::EXTRACT_FORMAT:{
//                 if(i>=input.size()){
//                     ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
//                     err = ErrorCode::TO_FEW_ARGUMENTS;
//                     return {};
//                 } 
//                 auto out_fmt = get_extract_format(input[i]);
//                 if(!out_fmt.has_value()){
//                     err = ErrorCode::COMMAND_INPUT_X1_ERROR;
//                     return {};
//                 }
//                 hExtract.set_output_format(out_fmt.value());
//                 commands.push_back(input.at(i));
//                 break;
//             }
//             case translate::token::Command::EXTRACTION_TIME_INTERVAL:{
//                 auto res = commands_from_time_separation(input[i],err);
//                 if(err!=ErrorCode::NONE || res.empty())
//                     return {};
//                 else commands.push_back(res);
//                 break;
//             }
//             default:{
//                 commands_from_search_process(input[i-1],input[i],hExtract,aliases_parameters,err);
//                 if(err!=ErrorCode::NONE)
//                     return {};
//                 else{
//                     commands.push_back(input[i]);
//                     break;
//                 }
//                 err = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid argument: argv["+std::string(input[i])+"]",AT_ERROR_ACTION::CONTINUE,input[i]);
//                 return {};
//                 break;
//             }
//         }
//     }
    
//     if(!hExtract.get_center().has_value()){
//         if(!aliases_parameters.empty()){
//             std::set<int> error_pos;
//             auto result = post_parsing_parameters_aliases_def(hExtract.get_center().value(),aliases_parameters,error_pos);
//             if(!error_pos.empty()){
//                 std::string error_aliases;
//                 for(int pos:error_pos)
//                     error_aliases+=""s+aliases_parameters.at(pos).second.data()+",";
//                 error_aliases.pop_back();
//                 err = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Parameter alias definition error",AT_ERROR_ACTION::CONTINUE,error_aliases);
//                 return {};
//             }
//             else{
//                 for(auto& param:result){
//                     hExtract.add_parameter(std::move(param));
//                 }
//             }
//         }
//     }
//     else{
//         err = ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Center",AT_ERROR_ACTION::CONTINUE);
//         return {};
//     }
//     if(err!=ErrorCode::NONE || hExtract.properties_integrity()!=ErrorCode::NONE)
//         return {};
//     return commands;
// }