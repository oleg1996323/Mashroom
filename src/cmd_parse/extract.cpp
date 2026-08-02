#include "cmd_parse/extract.h"
#include "proc/extract.h"
#include "cmd_parse/types_parse/datetimediff_parse.h"
#include "sys/application.h"

namespace parse{
    Extract::Extract(CLI::App* app):app_(app){
        auto jobs_=app_->add_option("-j,--jobs",jobs_val_,
        "Number of used threads. Number may be discarded to the maximal physical number threads")->
        capture_default_str()->
        default_val(std::thread::hardware_concurrency());
        auto output_dir_ = app_->
        add_option("--output-dir",output_dir_val_,
        "Output directory where the files with extracted data will be placed")->
        capture_default_str()->default_val((
                Application::app().config().user_config().current_settings().output_files_root_dir/
                "extract").string());
        auto input_paths_ = app_->add_option(
            "--input-paths",input_paths_val_,
            "Specify the input paths from where the data could be potentially extracted")->required();
        auto from_ = app_->add_option("--from",
            from_val_,"The date and time (in UTC) from which the data is extracted. Shall be not bigger than value of \"dtto\"")->
            capture_default_str()->default_str(boost::lexical_cast<std::string>(utc_tp_t<std::chrono::seconds>()));

        auto to_ =  app_->add_option("--to",to_val_,"The date and time (in UTC) up to which the data is extracted. Shall be not less than value of \"dtfrom\"")->
            capture_default_str()->default_str(boost::lexical_cast<std::string>(std::chrono::time_point_cast<std::chrono::seconds>(
                std::chrono::system_clock::now())));\
        auto coord_ = app_->add_option("--pos",
            pos_val_,"Coordinate position by latitude and longitude")->
            capture_default_str()
            ->default_val(Coord{.lat_=0,.lon_=0})->required();
        auto centers_ = app_->add_option("--center",centers_val_
            ,"Specify the center that released the data")->
            capture_default_str()->expected(1,-1);
        
        auto parameters_ = app_->add_option("--parameters",
            parameters_val_,"\tSpecify the expected parameters to process.\
            \tUse the '[...]' construction with escape words\
            \tseparation for matching the searched center by key words.")->
            capture_default_str()->expected(1,-1);
        auto collections_ = app_->add_option("--collection",
            collections_val_,"Specify by name of collection");
        auto time_forecast_=app_->add_option("--time_fcst",
            time_forecast_val_,"Specify the forecast time of the released data");
        auto level_=app_->add_option("--level",
            level_val_,"Specify the level (height bounds) of the released data")
            // ->required();
            ;
        auto grid_ = app_->add_option("--grid",grid_val_,"Specify the expected grid type.\
            Use the '[...]' construction with\
            escape words separation for matching\
            the searched center by key words.")->expected(1,-1);
        auto output_fmt_ = app_->add_option("--output-format",
            output_fmt_val_,
            "Sets the format of the output file(s)\
            containing the extracted data.\
            Possible formats: json,bin,txt.\
            Making \"zip+[choosen format]\"\
            or \"[choosen format]+zip\" will\
            archive the generated extraction\
            files.")->default_val(
                OutputDataFileFormats::BIN_F|
                OutputDataFileFormats::ARCHIVED);
        
        auto time_interval_ = app_->add_subcommand("time-interval",
            "Sets the time-period to be\
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
            this argument will be ignored for it.")->callback([this](){
                std::error_code err;
                return DateTimeDiff(err,std::chrono::years(years_),
                std::chrono::months(months_),
                std::chrono::days(days_),
                std::chrono::hours(hours_),
                std::chrono::minutes(minutes_),
                std::chrono::seconds(seconds_));});
        auto group = time_interval_->add_option_group("time-units");
        group->add_option("--years",years_)->
        default_val(0)->capture_default_str();
        group->add_option("--months",months_)->
        default_val(1)->capture_default_str();
        group->add_option("--days",days_)->
        default_val(0)->capture_default_str();
        group->add_option("--hours",hours_)->
        default_val(0)->capture_default_str();
        group->add_option("--minutes",minutes_)->
        default_val(0)->capture_default_str();
        group->add_option("--seconds",seconds_)->
        default_val(0)->capture_default_str();
        app_->callback([this](){
            this->execute();
        });
    }

    void Extract::execute(){
        ::Extract extract;
        if(auto jobs_ = app_->get_option("--job");
                jobs_->count())
            extract.set_using_processor_cores(jobs_val_);
        if(auto output_dir_ = app_->get_option("--output-dir");
                output_dir_->count())
            extract.set_out_path(output_dir_val_);
        if(auto from_ = app_->get_option("from");
                from_->count())
            extract.set_from_date(boost::lexical_cast<utc_tp_t<std::chrono::seconds>>(from_val_));
        if(auto to_ = app_->get_option("to");
                to_->count())
            extract.set_to_date(boost::lexical_cast<utc_tp_t<std::chrono::seconds>>(to_val_));
        if(auto coord_=app_->get_option("--pos");coord_->count())
            extract.set_position(pos_val_);
        if(auto centers_ = app_->get_option("--center");
                centers_->count()){
            auto center_res = center_notifier(centers_val_);
            if(center_res.has_value())
                extract.set_center(center_res.value());
            else throw std::runtime_error("invalid centers input");
        }
        if(auto parameters_ = app_->get_option("--parameters");
                parameters_->count()){
            osterlib::ContextedError ctx_err;
            for(std::string_view parameter:parameters_val_)
                extract.add_set_of_parameters(
                    parse::parameter_tv::param_by_tv_abbr(
                    extract.get_center().value(),
                    std::ranges::split_view(parameter,' ')|
                    std::ranges::to<std::vector<std::string>>(),
                    ctx_err));
        }
        if(auto collections_=app_->get_option("--collection");
                collections_->count()){
            std::cout<<"--collections option still not developped"<<std::endl;
        }
        if(auto time_forecast_ = app_->get_option("--time_fcst");
                time_forecast_->count()){
            //extract.set_time_fcst(time_forecast_val_);
            std::cout<<"--time_fcst option still not developped"<<std::endl;
        }
        if(auto level_ = app_->get_option("--level");
                level_->count()){
            std::cout<<"--level option still not developped"<<std::endl;
        }
        if(auto grid_ = app_->get_option("--grid");
                grid_->count()){
            if(auto res = grid_notifier(grid_val_);res.has_value())
                extract.set_grid_respresentation(res.value());
            else throw std::runtime_error("invalid grid input");
        }
        if(auto output_fmt_ = app_->get_option("--output-format")->count())
            extract.set_output_format(output_fmt_val_);
        if(app_->got_subcommand("time-interval")){
            auto time_interval_ = app_->get_subcommand("time-interval");
            std::error_code err;
            using namespace std::chrono;
            DateTimeDiff dtd(err,
                years(years_),
                months(months_),
                days(days_),
                hours(hours_),
                minutes(minutes_),
                std::chrono::seconds(seconds_));
            if(err)
                throw CLI::InvalidError("time-interval");
            extract.set_offset_time_interval(dtd);
        }
        extract.execute();
    }
}