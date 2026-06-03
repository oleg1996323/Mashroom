#include "cmd_parse/mashroom_parse.h"
#include "cmd_parse/types_parse/grid_type_parse.h"
#include "cmd_parse/types_parse/center_parse.h"
#include "cmd_parse/types_parse/information_parse.h"
#include "proc/index.h"
#include "proc/extract.h"
#include "proc/integrity.h"
#include "proc/contains.h"

namespace parse{

    void init_config(CLI::App* cli) noexcept{
        ErrorCode err_;
        ::Integrity hCheck;
        auto* cmd = cli->add_subcommand("configure",
            "Set configurations");
            
    }

    void init_network(CLI::App* cli) noexcept{
        ErrorCode err_;
        ::Integrity hCheck;
        auto* cmd = cli->add_subcommand("configure",
            "Network activities");
            
    }
    
    void init_integrity(CLI::App* cli) noexcept{
        ErrorCode err_;
        ::Integrity hCheck;
        auto* cmd = cli->add_subcommand("check",
            "Check the integrity (dimensional and temporal)\
            of indexd data and detect the corrupted files\
            of different format.");
            cmd->require_subcommand(1);
            int jobs = std::thread::hardware_concurrency();
            cmd->add_option("-j",jobs,
            "Number of used threads.\
            Number may be discarded to\
            the maximal physical number\
            threads")->
            capture_default_str()->
            default_val(16);
    }
    
    void init_contains(CLI::App* cli) noexcept{
        ErrorCode err_;
        ::Contains hContains;
        auto* cmd = cli->add_subcommand("contains",
            "\tCheck if indexed data contains the data\
            \tspecified by properties");
            cmd->require_subcommand(1);
            int jobs = std::thread::hardware_concurrency();
            cmd->add_option("-j",jobs,
            "Number of used threads.\
            Number may be discarded to\
            the maximal physical number\
            threads")->
            capture_default_str()->
            default_val(16);
    }

    void init_index(CLI::App* cli) noexcept{
        ErrorCode err_;
        ::Index hIndex;
        auto* cmd = cli->add_subcommand("index",
            "Read specified files and register\
            the contained data properties and\
            data positions. Organize the data\
            by defined hierarchy from unique\
            massive archive.");
            cmd->require_subcommand(1)->
            alias("-I");
            int jobs = std::thread::hardware_concurrency();
            cmd->add_option("-j",jobs,
            "Number of used threads.\
            Number may be discarded to\
            the maximal physical number\
            threads")->
            capture_default_str()->
            default_val(16);
            std::string output_dir;
            {
                auto tmp_dir = boost::uuids::random_generator_mt19937()();
                std::string output_dir=app().config().user_config().current_settings().output_files_root_dir/
                    "index"/
                    std::string(tmp_dir.begin(),tmp_dir.end());
            }
            ///
            cmd->
            add_option("--output-dir",output_dir,
            "Output directory where the files with extracted data will be placed")->
            capture_default_str()->each([&](const std::string& opath){
                hIndex.set_dest_dir(opath);
            })->capture_default_str();
            cmd->add_option_function<std::vector<path::Storage<false>>>(
                "--input-paths",[&](const std::vector<path::Storage<false>>& paths)
                    {
                        int count = paths.size();
                        for(auto& path:paths){
                            if(auto err = hIndex.add_in_path(path);
                                err!=ErrorCode::NONE)
                                ErrorPrint::print_error(err,"",AT_ERROR_ACTION::CONTINUE,path.path_);
                        }
                        if(count==paths.size())
                            throw CLI::ValidationError("--input-paths","all paths are invalid");
                    },
                "Specify the input paths from where the data could be potentially extracted");
            ///
            IndexOutputFileFormat::token fmt_ext = IndexOutputFileFormat::token::BINARY;
            cmd->add_option_function<IndexOutputFileFormat::token>("--format",
                [&](const IndexOutputFileFormat::token& fmt)
                {
                    hIndex.set_output_format(boost::lexical_cast<IndexOutputFileFormat::token>(fmt));
                },"Specify the output overwriting format if the \"index-overwrite\" option is set.\
                If overwriting with selected format cannot be implemented the \""s+
                boost::lexical_cast<std::string>(IndexOutputFileFormat::token::BINARY)+
                "\" format is set automatically. Ignored if \"index-overwrite\" is not set.")->
                capture_default_str();
            bool overwrite = false;
            cmd->add_option("--overwrite",overwrite,
                "The date and time (in UTC) up to which\
                the data is extracted.\
                Shall be not less than value of \"dtfrom\"")->
                capture_default_str();
            bool ref = true;
            cmd->add_option("--reference-only",overwrite,
                "The date and time (in UTC) up to which\
                the data is extracted.\
                Shall be not less than value of \"dtfrom\"")->
                capture_default_str();
            info_quantity mem_sz = static_cast<double>(std::numeric_limits<uint64_t>::max())*info_units{};
            cmd->add_option_function<info_quantity>(
                "--web-file-max-size",[&](info_quantity mem_sz)
            {
                hIndex.set_max_index_size(mem_sz);
            },
            "Specify the maximal part size of shared data when indexing process a host-path. "+
            "It is useful for limiting the data indexing in cases when the summary size of remote indexd data is unknown."s+\
            "If value is \"0\" it will be ignored. If \"index-ref\" is set \"true\" this option is ignored.")->ignore_case(ref);
    }

    void init_extract(CLI::App* cli) noexcept{
            ErrorCode err_;
            ::Extract hExtract;
            auto* cmd = cli->add_subcommand("extract","Extract specified data.");
            cmd->require_subcommand(1)->
            alias("-E");
            int jobs = std::thread::hardware_concurrency();
            cmd->add_option("-j",jobs,
            "Number of used threads. Number may be discarded to the maximal physical number threads")->
            capture_default_str()->
            default_val(16);
            std::string output_dir=app().config().user_config().current_settings().output_files_root_dir.c_str();
            ///
            cmd->
            add_option("--output-dir",output_dir,
            "Output directory where the files with extracted data will be placed")->
            capture_default_str()->each([&](const std::string& opath){
                hExtract.set_out_path(opath);
                
            });
            cmd->add_option_function<std::vector<std::string>>(
                "--input-paths",[&](const std::vector<std::string>& paths)
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
            utc_tp_t<std::chrono::seconds> from = utc_tp_t<std::chrono::seconds>();
            cmd->add_option_function<utc_tp_t<std::chrono::seconds>>("--dtfrom",
                [&](const utc_tp_t<std::chrono::seconds>& from)
                {
                    hExtract.set_from_date(boost::lexical_cast<utc_tp_t<std::chrono::seconds>>(from));
                },"The date and time (in UTC) from which the data is extracted. Shall be not bigger than value of \"dtto\"")->
                capture_default_str();
            utc_tp_t<std::chrono::seconds> to = 
                std::chrono::time_point_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now());
            cmd->add_option_function<utc_tp_t<std::chrono::seconds>>("--dtto",
                [&](const utc_tp_t<std::chrono::seconds>& from)
                {
                    hExtract.set_from_date(boost::lexical_cast<utc_tp_t<std::chrono::seconds>>(from));
                },"The date and time (in UTC) up to which the data is extracted. Shall be not less than value of \"dtfrom\"")->
                capture_default_str();
            Coord pos ={.lat_=0,.lon_=0};
            cmd->add_option_function<Coord>("--pos",
                [&](const Coord& pos)
                {
                    hExtract.set_position(boost::lexical_cast<Coord>(pos));
                },"Coordinate position by latitude and longitude")->
                capture_default_str()->required();
            std::vector<std::string> center;
            cmd->add_option_function<std::vector<std::string>>("--center",
                [&](const std::vector<std::string>& centers)
                {
                    ErrorCode err_;
                    auto center_res = center_notifier(centers);
                    if(center_res.has_value())
                        hExtract.set_center(center_res.value());
                    else err_ = center_res.error();
                },"Specify the center that released the data")->
                capture_default_str()->required()->expected(1,-1);
            cmd->add_option_function<std::vector<std::string>>("--parameters",
                [&](const std::vector<std::string>& parameters)
                {
                    for(std::string_view parameter:parameters)
                        hExtract.add_set_of_parameters(
                            parse::parameter_tv::param_by_tv_abbr(
                            hExtract.get_center().value(),
                            std::ranges::split_view(parameter,' ')|
                            std::ranges::to<std::vector<std::string>>()));
                },"\tSpecify the expected parameters to process.\
                \tUse the '[...]' construction with escape words\
                \tseparation for matching the searched center by key words.")->
                capture_default_str()->required()->expected(1,-1);
            cmd->add_option_function<std::vector<std::string>>("--collection",
                [&](const std::vector<std::string>& collection){
                    assert(false);
                },"Specify by name of collection");
            cmd->add_option_function<std::string>("--time_fcst",[&](const std::string& fcst){
                assert(false);
            },"Specify the forecast time of the released data");
            cmd->add_option_function<std::vector<std::string>>("--grid",
                [&](const std::vector<std::string>& grids){
                    if(err_==ErrorCode::NONE){
                        if(auto res = grid_notifier(grids);res.has_value())
                            hExtract.set_grid_respresentation(res.value());
                        else err_ = res.error();
                    }
                },"Specify the expected grid type.\
                Use the '[...]' construction with\
                escape words separation for matching\
                the searched center by key words.")->required()->expected(1,-1);
            cmd->add_option_function<::OutputDataFileFormats>("--output-format",
                [&](const ::OutputDataFileFormats& fmt){
                    if(err_==ErrorCode::NONE)
                        hExtract.set_output_format(fmt);
                },"Sets the format of the output file(s)\
                containing the extracted data.\
                Possible formats: json,bin,txt.\
                Making \"zip+[choosen format]\"\
                or \"[choosen format]+zip\" will\
                archive the generated extraction\
                files.");
            
            cmd->add_option_function<DateTimeDiff>("--time-interval",
                [&](const DateTimeDiff& fmt){
                    if(err_==ErrorCode::NONE)
                        err_=hExtract.set_offset_time_interval(fmt);
                },"Sets the time-period to be\
                extracted in different files.\
                E.g. you choose 1 day time-period\
                separation for period from\
                1990/01/01 20:25 to 1991/01/03 12:00, \
                all extracted data will be separated\
                by files in the period from\
                1990/01/01 00:00 to 1990/01/04 00:00,\
                e.g. flooring the indicated\
                time-period separation. If data\
                is not represented as a time series\
                this argument will be ignored for it.")->required()->default_val([](){
                    std::error_code err;
                    return DateTimeDiff(err,years(0),
                    months(1),
                    days(0),
                    hours(0),
                    minutes(0),
                    std::chrono::seconds(0));}());
    }

    void Mashroom::init() noexcept{
        auto& app = Application::command_line();
        init_extract(&app);
        init_index(&app);
        init_integrity(&app);
        init_contains(&app);
        init_config(&app);
        init_network(&app);
        init_save(&app);
        init_help(&app);
        init_exit(&app);

        auto save_cmd = app.add_subcommand("save",
            "Save the current instance (data, configurations)");
        if(*save_cmd)
            ::Mashroom::instance().save();
        auto help_cmd = app.add_subcommand("help","Show help");
        if(*help_cmd)
            CLI::CallForAllHelp(app);
        bool save = false;
        auto exit_cmd = app.add_subcommand("exit","Exit from program")->
        add_option("save",save)->capture_default_str();
        if(exit_cmd)
            exit(0);
    }
}