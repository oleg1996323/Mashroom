#include "cmd_parse/index.h"
#include "sys/error_print.h"
#include "sys/application.h"
#include "sys/config.h"
#include "sys/outputdatafileformats.h"
#include "types_parse/information_parse.h"
#include "proc/index.h"

namespace parse{
    Index::Index(CLI::App* cli):app_(cli){
        jobs_ = app_->add_option("-j",
        "Number of used threads.\
        Number may be discarded to\
        the maximal physical number\
        threads")->
        capture_default_str()->
        default_val(16);
        ///
        output_dir_ = app_->
        add_option("--output-dir",output_dir_val_,
        "Output directory where the files with extracted data will be placed")->
        capture_default_str()->default_str([](){
            auto tmp_dir = boost::uuids::random_generator()();
            return (::app().config().user_config().current_settings().output_files_root_dir/
                "index"/
                std::string(tmp_dir.begin(),tmp_dir.end())).string();
        }());
        input_paths_ = app_->add_option(
            "--input-paths",input_paths_val_,
            "Specify the input paths from where the data could be potentially extracted")->required();
        ///
        format_ = app_->add_option("--format"s,fmt_val_,
            "Specify the output overwriting format if the \"index-overwrite\" option is set.\
            If overwriting with selected format cannot be implemented the \""s+
            boost::lexical_cast<std::string>(IndexOutputFileFormat::token::BINARY)+
            "\" format is set automatically. Ignored if \"index-overwrite\" is not set.")->
            default_str(boost::lexical_cast<std::string>(
                IndexOutputFileFormat::token::BINARY
            ))->capture_default_str();
        ref_ = app_->add_flag("--reference-only",
            "The date and time (in UTC) up to which\
            the data is extracted.\
            Shall be not less than value of \"dtfrom\"")->
            default_val(true)->
            capture_default_str();
        web_file_max_size_ = app_->add_option("--web-file-max-size",info_val_,
        "Specify the maximal part size of shared data when indexing process a host-path.\
        It is useful for limiting the data indexing in cases when the summary size of remote indexd data is unknown.\
        If value is \"0\" it will be ignored. If \"index-ref\" is set \"true\" this option is ignored.")
        ->default_str([](){
            return boost::lexical_cast<std::string>(static_cast<double>(std::numeric_limits<uint64_t>::max())*info_units{});
        }());
    }

    void Index::execute(){
        ::Index index;
        if(jobs_->count())
            index.set_using_processor_cores(jobs_->as<uint32_t>());
        if(output_dir_->count())
            index.set_dest_dir(output_dir_->as<std::string>());
        if(input_paths_->count()){
            int count = input_paths_val_.size();
            for(auto& path:input_paths_val_){
                if(auto err = index.add_in_path(path);
                    err!=ErrorCode::NONE)
                    ErrorPrint::print_error(err,"",AT_ERROR_ACTION::CONTINUE,path.path_);
                else --count;
            }
            if(count==input_paths_val_.size())
                throw CLI::ValidationError("--input-paths","all paths are invalid");
        }
        if(format_->count())
            index.set_output_format(format_->as<IndexOutputFileFormat::token>());
        if(ref_->count());
            index.set_host_ref_only();
        if(web_file_max_size_->count() && index.host_ref_only())
            index.set_max_index_size(boost::lexical_cast<info_quantity>(info_val_));
        index.execute();
    }
}