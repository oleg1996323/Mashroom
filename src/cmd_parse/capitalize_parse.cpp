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
#include "filesystem.h"

namespace fs = std::filesystem;

namespace parse{
    void Capitalize::init() noexcept{
        add_options
        ("j",po::value<int>()->notifier([this](uint8_t threads){
            hCapitalize->set_using_processor_cores(threads);
        }),"Number of used threads. Number may be discarded to the maximal physical number threads")
        ("out-dirs",po::value<std::string>()->value_name("DIR")->default_value("")->notifier([this](const std::string& out_path){
            if(!out_path.empty())
                err_ = hCapitalize->set_dest_dir(out_path);
        }),"Output path. Must be defined if cap-overwrite is set true. If the path does not exist and can be created, it will be created. Else the capitalization process will not begin.")
        ("inp",po::value<std::vector<path::Storage<false>>>()->value_name("dir:DIR|file:FILE|host:HOST")->required()->notifier([this](const std::vector<path::Storage<false>>& paths){
            for(auto& path:paths){
                hCapitalize->add_in_path(path);
                if(err_!=ErrorCode::NONE)
                    return;
            }
        }),"Specify the input paths. The paths must exists otherwise capitalize process will not begin.")
        ("cap-dir-order",po::value<std::string>()->value_name("[year-Y, month-M, day-D, hour-H]")->notifier([this](const std::string& order)
        {
            if(hCapitalize)
                err_ = hCapitalize->set_output_order(order);
            else err_ = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "invalid capitalize directories order",AT_ERROR_ACTION::CONTINUE,order);
        }),("Specify the separation of capitalized data by paths. Is ignored if \"cap-overwrite\" option is not specified or set \"false\"."s+
        " Example: YMDH - {outp-value}/{data-year}/{data-month}/{data-day}/{data-hour}.").c_str())
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
        }),("Specify the output overwriting format if the \"cap-overwrite\" option is set. If overwriting with selected format cannot be implemented the \""s+
        boost::lexical_cast<std::string>(InternalDateFileFormats::NATIVE)+
        "\" format is set automatically. Ignored if \"cap-overwrite\" is not set.").c_str())
        /*@todo*/("cap-overwrite",po::bool_switch(),("Set overwrite mode at capitalization. That mean all capitalized data from read files will be structured in hierarchy depending of cap-dir-order."s+
        "If cap-dir-order is set to \"\" (empty) the capitalized file will be simply copied to the defined by \"out-dirs\" option path.").c_str())
        ("cap-ref","")
        ("cap-part-memsize",po::value<info_quantity>()->default_value(static_cast<double>(std::numeric_limits<uint64_t>::max())*info_units{})->notifier([this](const info_quantity& memory_size){
            hCapitalize->set_max_cap_size(memory_size);
        }),("Specify the maximal part size of shared data when capitalization process a host-path. It is useful for limiting the data capitalization in cases when the summary size of remote capitalized data is unknown."s+\
        "If value is \"0\" it will be ignored. If \"cap-ref\" is set \"true\" this option is ignored.").c_str());
        define_uniques();
    }

    ErrorCode Capitalize::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        hCapitalize = std::make_unique<::Capitalize>();
        if(vm.contains("cap_ref"))
            hCapitalize->set_host_ref_only();
        err_ = try_notify(vm);
        if(err_==ErrorCode::NONE)
            hCapitalize->execute();
        return err_;
    }
}