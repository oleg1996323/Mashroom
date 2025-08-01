#include "cmd_parse/index_parse.h"
#include <thread>
#include <filesystem>
#include <iostream>
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "proc/index.h"
#include "cmd_parse/functions.h"
#include "proc/index.h"
#include "information_parse.h"
#include "cmd_parse/cmd_def.h"
#include "internal_format_parse.h"
#include "filesystem.h"

namespace fs = std::filesystem;

namespace parse{
    void Index::init() noexcept{
        add_options
        ("j",po::value<int>()->notifier([this](uint8_t threads){
            hIndex->set_using_processor_cores(threads);
        }),"Number of used threads. Number may be discarded to the maximal physical number threads")
        ("out-dirs",po::value<std::string>()->value_name("DIR")->default_value("")->notifier([this](const std::string& out_path){
            if(!out_path.empty())
                err_ = hIndex->set_dest_dir(out_path);
        }),"Output path. Must be defined if index-overwrite is set true. If the path does not exist and can be created, it will be created. Else the indexing process will not begin.")
        ("inp",po::value<std::vector<path::Storage<false>>>()->value_name("dir:DIR|file:FILE|host:HOST")->required()->notifier([this](const std::vector<path::Storage<false>>& paths){
            for(auto& path:paths){
                hIndex->add_in_path(path);
                if(err_!=ErrorCode::NONE)
                    return;
            }
        }),"Specify the input paths. The paths must exists otherwise index process will not begin.")
        ("index-dir-order",po::value<std::string>()->value_name("[year-Y, month-M, day-D, hour-H]")->notifier([this](const std::string& order)
        {
            if(hIndex)
                err_ = hIndex->set_output_order(order);
            else err_ = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "invalid index directories order",AT_ERROR_ACTION::CONTINUE,order);
        }),("Specify the separation of indexd data by paths. Is ignored if \"index-overwrite\" option is not specified or set \"false\"."s+
        " Example: YMDH - {outp-value}/{data-year}/{data-month}/{data-day}/{data-hour}.").c_str())
        ("index-format",po::value<InternalDateFileFormats>()->value_name("["+
            boost::lexical_cast<std::string>(InternalDateFileFormats::NATIVE)+"|"+
            boost::lexical_cast<std::string>(InternalDateFileFormats::TEXT)+"|"+
            boost::lexical_cast<std::string>(InternalDateFileFormats::BINARY)+"|"+
            boost::lexical_cast<std::string>(InternalDateFileFormats::GRIB)+"|"+
            boost::lexical_cast<std::string>(InternalDateFileFormats::HGT)+"|"+
            boost::lexical_cast<std::string>(InternalDateFileFormats::IEEE)+"|"+
            boost::lexical_cast<std::string>(InternalDateFileFormats::NETCDF)+"]"
            )->default_value(InternalDateFileFormats::NATIVE)->notifier([this](InternalDateFileFormats order){
            hIndex->set_output_format(order);
        }),("Specify the output overwriting format if the \"index-overwrite\" option is set. If overwriting with selected format cannot be implemented the \""s+
        boost::lexical_cast<std::string>(InternalDateFileFormats::NATIVE)+
        "\" format is set automatically. Ignored if \"index-overwrite\" is not set.").c_str())
        /*@todo*/("index-overwrite",po::bool_switch(),("Set overwrite mode at indexing. That mean all indexd data from read files will be structured in hierarchy depending of index-dir-order."s+
        "If index-dir-order is set to \"\" (empty) the indexd file will be simply copied to the defined by \"out-dirs\" option path.").c_str())
        ("index-ref","")
        ("index-part-memsize",po::value<info_quantity>()->default_value(static_cast<double>(std::numeric_limits<uint64_t>::max())*info_units{})->notifier([this](const info_quantity& memory_size){
            hIndex->set_max_index_size(memory_size);
        }),("Specify the maximal part size of shared data when indexing process a host-path. It is useful for limiting the data indexing in cases when the summary size of remote indexd data is unknown."s+\
        "If value is \"0\" it will be ignored. If \"index-ref\" is set \"true\" this option is ignored.").c_str());
        define_uniques();
    }

    ErrorCode Index::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        hIndex = std::make_unique<::Index>();
        if(vm.contains("index_ref"))
            hIndex->set_host_ref_only();
        err_ = try_notify(vm);
        if(err_==ErrorCode::NONE)
            hIndex->execute();
        return err_;
    }
}