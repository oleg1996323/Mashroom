#include "cmd_parse/index.h"
#include "sys/error_print.h"
#include "sys/application.h"
#include "sys/config.h"
#include "sys/outputdatafileformats.h"

namespace parse{
    Index::Index(CLI::App* cli):app_(cli){
        ErrorCode err_;
        jobs_ = index_->add_option("-j",
        "Number of used threads.\
        Number may be discarded to\
        the maximal physical number\
        threads")->
        capture_default_str()->
        default_val(16);
        ///
        output_dir_ = index_->
        add_option_function<std::string>("--output-dir",
            [this](const std::string& opath){
                hIndex->set_dest_dir(opath);
            },
        "Output directory where the files with extracted data will be placed")->
        capture_default_str()->default_str([](){
            auto tmp_dir = boost::uuids::random_generator()();
            return (app().config().user_config().current_settings().output_files_root_dir/
                "index"/
                std::string(tmp_dir.begin(),tmp_dir.end())).string();
        }());
        input_paths_ = index_->add_option_function<std::vector<path::Storage<false>>>(
            "--input-paths",[this](const std::vector<path::Storage<false>>& paths)
                {
                    int count = paths.size();
                    for(auto& path:paths){
                        if(auto err = hIndex->add_in_path(path);
                            err!=ErrorCode::NONE)
                            ErrorPrint::print_error(err,"",AT_ERROR_ACTION::CONTINUE,path.path_);
                    }
                    if(count==paths.size())
                        throw CLI::ValidationError("--input-paths","all paths are invalid");
                },
            "Specify the input paths from where the data could be potentially extracted")->required();
        ///
        format_ = index_->add_option_function<IndexOutputFileFormat::token>("--format",
            [this](const IndexOutputFileFormat::token& fmt)
            {
                hIndex->set_output_format(boost::lexical_cast<IndexOutputFileFormat::token>(fmt));
            },
            "Specify the output overwriting format if the \"index-overwrite\" option is set.\
            If overwriting with selected format cannot be implemented the \""s+
            boost::lexical_cast<std::string>(IndexOutputFileFormat::token::BINARY)+
            "\" format is set automatically. Ignored if \"index-overwrite\" is not set.")->
            default_str(boost::lexical_cast<std::string>(
                IndexOutputFileFormat::token::BINARY
            ))->capture_default_str();
        overwrite_ = index_->add_flag("--overwrite",
            "The date and time (in UTC) up to which\
            the data is extracted.\
            Shall be not less than value of \"dtfrom\"")->
            default_val(false)->
            capture_default_str();
        ref_ = index_->add_flag("--reference-only",
            "The date and time (in UTC) up to which\
            the data is extracted.\
            Shall be not less than value of \"dtfrom\"")->
            default_val(true)->
            capture_default_str();
        web_file_max_size_ = index_->add_option_function<info_quantity>(
            "--web-file-max-size",[&](info_quantity mem_sz)
        {
            hIndex->set_max_index_size(mem_sz);
        },
        "Specify the maximal part size of shared data when indexing process a host-path. "+
        "It is useful for limiting the data indexing in cases when the summary size of remote indexd data is unknown."s+\
        "If value is \"0\" it will be ignored. If \"index-ref\" is set \"true\" this option is ignored.")
        ->ignore_case(true)
        ->default_val(static_cast<double>(std::numeric_limits<uint64_t>::max())*info_units{});
    }

    void Index::execute(){
        hIndex=std::make_unique<::Index>();
        if(jobs_->count())
            hIndex->set_using_processor_cores(jobs_->as<int>());
        if(output_dir_->count())
            hIndex->set_dest_dir(output_dir_->as<std::string>());
        if(input_paths_->count())
            for(path::Storage<false>& path:input_paths_->as<std::vector<path::Storage<false>>>())
                hIndex->add_in_path(path);
        if(overwrite_->count())
            assert(false);
        if(ref_->count() && ref_->as<bool>())
            hIndex->set_host_ref_only();
        if(web_file_max_size_->count())
            hIndex->set_max_index_size(web_file_max_size_->as<info_quantity>());
    }
}