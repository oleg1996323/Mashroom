#include "cmd_parse/mashroom_parse.h"

namespace parse{
    ErrorCode extract_notifier(const std::vector<std::string>& input) noexcept{
        return Extract::instance().parse(input);
    }
    ErrorCode index_notifier(const std::vector<std::string>& input) noexcept{
        return Index::instance().parse(input);
    }
    ErrorCode integrity_notifier(const std::vector<std::string>& input) noexcept{
        return Integrity::instance().parse(input);
    }
    ErrorCode contains_notifier(const std::vector<std::string>& input) noexcept{
        return Contains::instance().parse(input);
    }
    ErrorCode server_notifier(const std::vector<std::string>& input) noexcept{
        return Network::instance().parse(input);
    }
    ErrorCode config_notifier(const std::vector<std::string>& input) noexcept{
        return ProgramConfig::instance().parse(input);
    }
    ErrorCode serverconfig_notifier(const std::vector<std::string>& input) noexcept{
        return ServerConfig::instance().parse(input);
    }
    ErrorCode save_notifier() noexcept{
        ::Mashroom::instance().save();
        return ErrorCode::NONE;
    }
    ErrorCode exit_notifier(const std::string& input) noexcept{
        if(input=="save")
            ::Mashroom::instance().save();
        exit(0);
    }

    void Mashroom::init() noexcept{
        auto& app = Application::command_line();
        {
        auto* extract = app.add_subcommand("extract","Extract specified data.");
            extract->require_subcommand(1)->
            alias("-E");
            int jobs = std::thread::hardware_concurrency();
            extract->add_option("-j",jobs,
            "Number of used threads. Number may be discarded to the maximal physical number threads")->
            capture_default_str()->
            default_val(16);
            std::string output_dir="";
            extract->
            add_option("--output-dir",output_dir,
            "Output directory where the files with extracted data will be placed")->
            sys
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
            procedures::Extract extract_procedure;
            extract_procedure.
        }
        //////////////////
        app.add_subcommand("index",
            "\tRead specified files and\
            \tregister the contained data properties and data positions.\
            \tOrganize the data by defined hierarchy from unique massive archive.");
        app.add_subcommand("integrity",
            "\tCheck the integrity (dimensional and temporal)\
            \tof indexd data and detect the corrupted files of\
            \tdifferent format.");
        app.add_subcommand("contains",
            "\tCheck if indexed data contains the data\
            \tspecified by properties");
        app.add_subcommand("config","Configurations");
        app.add_subcommand("network","Network activities");
        app.add_subcommand("save","Save the current instance (data, configurations)");
        app.add_subcommand("help","Show help");
        app.add_subcommand("exit","Exit from program");
        add_options_instances
        ("extract",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = extract_notifier(items);
        }),"Extract specified data.",Extract::instance())
        ("index",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = index_notifier(items);
        }),"Read specified files and register the contained data properties and data positions. Organize the data by defined hierarchy from unique massive archive.",Index::instance())
        ("integrity",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = integrity_notifier(items);
        }),"Check the integrity (dimensional and temporal) of indexd data and detect the corrupted files of different format.",Integrity::instance())
        ("contains",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = contains_notifier(items);
        }),"Check if indexed data contains the data specified by properties",Contains::instance())
        ("config",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = config_notifier(items);
        }),"Configurations",ProgramConfig::instance())
        ("network",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            
        }),"Network activities",Network::instance())
        ("save",po::value<void>()->notifier([this](){
            
        }),"Save the current instance (data, configurations)")
        ("help,H","Show help")
        ("exit",po::value<void>()->notifier([](){
            exit(0);
        }),"Exit from program");
        assert(!descriptor_.find_nothrow("add",false));
        define_uniques();
    }
}