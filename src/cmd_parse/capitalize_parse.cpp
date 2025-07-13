#include "cmd_parse/capitalize_parse.h"
#include <thread>
#include <filesystem>
#include <iostream>
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "capitalize.h"
#include "cmd_parse/functions.h"
#include "capitalize.h"
#include "information_parse.h"
#include "cmd_parse/cmd_def.h"

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
        ("cap-format",po::value<std::string>()->notifier([this](const std::string& order){
            err_ = hCapitalize->set_output_order(order);
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
    }
}