#include "cmd_parse/capitalize_parse.h"
#include <thread>
#include <filesystem>
#include <iostream>
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "proc/capitalize.h"
#include "cmd_parse/functions.h"
#include "proc/capitalize.h"
#include "information_parse.h"
#include "cmd_parse/cmd_def.h"
#include "internal_format_parse.h"

namespace fs = std::filesystem;

namespace parse{
    void Capitalize::init() noexcept{
        descriptor_.add_options()
        ("j",po::value<int>()->notifier([this](uint8_t threads){
            hCapitalize->set_using_processor_cores(threads);
        }),"Number of used threads. Number may be discarded to the maximal physical number threads")
        ("out-dir",po::value<std::string>()->default_value("")->notifier([this](const std::string& out_path){
            if(!out_path.empty())
                err_ = hCapitalize->set_dest_dir(out_path);
        }),"Output path. May be directory or file path")
        ("inp",po::value<std::vector<path::Storage<true>>>()->required()->notifier([this](const std::vector<path::Storage<true>>& paths){
            for(auto& path:paths){
                err_ = hCapitalize->add_in_path(path);
                if(err_!=ErrorCode::NONE)
                    return;
            }
        }),"Specify the input path")
        ("cap-dir-order",po::value<std::string>()->notifier([this](const std::string& order)
        {
            if(hCapitalize)
                err_ = hCapitalize->set_output_order(order);
            else err_ = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "invalid capitalize directories order",AT_ERROR_ACTION::CONTINUE,order);
        }),"")
        ("cap-format",po::value<InternalDateFileFormats>()->value_name("["+
            boost::lexical_cast<std::string>(InternalDateFileFormats::NATIVE)+"|"+
            boost::lexical_cast<std::string>(InternalDateFileFormats::TEXT)+"|"+
            boost::lexical_cast<std::string>(InternalDateFileFormats::BINARY)+"|"+
            boost::lexical_cast<std::string>(InternalDateFileFormats::GRIB)+"|"+
            boost::lexical_cast<std::string>(InternalDateFileFormats::HGT)+"|"+
            boost::lexical_cast<std::string>(InternalDateFileFormats::IEEE)+"|"+
            boost::lexical_cast<std::string>(InternalDateFileFormats::NETCDF)+"]"
            )->default_value(InternalDateFileFormats::NATIVE)->notifier([this](InternalDateFileFormats order){
            hCapitalize->set_output_format(order);
        }),"")
        ("cap-ref","")
        ("cap-part-memsize",po::value<info_quantity>()->notifier([this](const info_quantity& memory_size){
            hCapitalize->set_max_cap_size(memory_size);
        }),"");
        define_uniques();
    }

    ErrorCode Capitalize::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        hCapitalize = std::make_unique<::Capitalize>();
        if(vm.contains("cap_ref"))
            hCapitalize->set_host_ref_only();
        err_ = try_notify(vm);
        return err_;
    }
}