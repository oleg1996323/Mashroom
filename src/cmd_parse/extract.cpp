#include "cmd_parse/extract.h"
#include "proc/extract.h"
#include "cmd_parse/types_parse/datetimediff_parse.h"

namespace parse{
    Extract::Extract(CLI::App* app):app_(app){
        jobs_=app_->add_option("-j",jobs_val_,
        "Number of used threads. Number may be discarded to the maximal physical number threads")->
        capture_default_str()->
        default_val(16);
        output_dir_ = app_->
        add_option("--output-dir",output_dir_val_,
        "Output directory where the files with extracted data will be placed")->
        capture_default_str();
        input_paths_ = app_->add_option(
            "--input-paths",input_paths_val_,
            "Specify the input paths from where the data could be potentially extracted");
        from_ = app_->add_option("--dtfrom",
            from_val_,"The date and time (in UTC) from which the data is extracted. Shall be not bigger than value of \"dtto\"")->
            capture_default_str()->default_str(boost::lexical_cast<std::string>(utc_tp_t<std::chrono::seconds>()));

        to_ =  app_->add_option("--dtto",to_val_,"The date and time (in UTC) up to which the data is extracted. Shall be not less than value of \"dtfrom\"")->
            capture_default_str()->default_str(boost::lexical_cast<std::string>(std::chrono::time_point_cast<std::chrono::seconds>(
                std::chrono::system_clock::now())));\
        coord_ = app_->add_option("--pos",
            pos_val_,"Coordinate position by latitude and longitude")->
            capture_default_str()
            ->default_val(Coord{.lat_=0,.lon_=0});
        centers_ = app_->add_option("--center",centers_val_
            ,"Specify the center that released the data")->
            capture_default_str()->expected(1,-1);
        
        parameters_ = app_->add_option("--parameters",
            parameters_val_,"\tSpecify the expected parameters to process.\
            \tUse the '[...]' construction with escape words\
            \tseparation for matching the searched center by key words.")->
            capture_default_str()->expected(1,-1);
        collections_ = app_->add_option("--collection",
            collections_val_,"Specify by name of collection");
        time_forecast_=app_->add_option("--time_fcst",
            time_forecast_val_,"Specify the forecast time of the released data");
        level_=app_->add_option("--level",
            level_val_,"Specify the level (height bounds) of the released data");
        grid_ = app_->add_option("--grid",grid_val_,"Specify the expected grid type.\
            Use the '[...]' construction with\
            escape words separation for matching\
            the searched center by key words.")->expected(1,-1);
        output_fmt_ = app_->add_option("--output-format",
            output_fmt_val_,
            "Sets the format of the output file(s)\
            containing the extracted data.\
            Possible formats: json,bin,txt.\
            Making \"zip+[choosen format]\"\
            or \"[choosen format]+zip\" will\
            archive the generated extraction\
            files.");
        
        time_interval_ = app_->add_subcommand("time-interval",
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
                return DateTimeDiff(err,std::chrono::years(0),
                std::chrono::months(1),
                std::chrono::days(0),
                std::chrono::hours(0),
                std::chrono::minutes(0),
                std::chrono::seconds(0));});
        auto group = time_interval_->add_option_group("time-units");
        group->add_option("--years",years)->
        default_val(0)->capture_default_str();
        group->add_option("--months",months)->
        default_val(1)->capture_default_str();
        group->add_option("--days",days)->
        default_val(0)->capture_default_str();
        group->add_option("--hours",hours)->
        default_val(0)->capture_default_str();
        group->add_option("--minutes",minutes)->
        default_val(0)->capture_default_str();
        group->add_option("--seconds",seconds)->
        default_val(0)->capture_default_str();
        app_->callback([this](){
            this->execute();
        });
    }

    void Extract::execute(){
        ::Extract extract;
        if(jobs_->count())
            extract.set_using_processor_cores(jobs_->as<int32_t>());
        if(output_dir_->count())
            extract.set_out_path(output_dir_val_);
        if(from_->count())
            extract.set_from_date(boost::lexical_cast<utc_tp_t<std::chrono::seconds>>(from_val_));
        if(to_->count())
            extract.set_to_date(boost::lexical_cast<utc_tp_t<std::chrono::seconds>>(to_val_));
        if(coord_->count())
            extract.set_position(pos_val_);
        if(centers_->count()){
            auto center_res = center_notifier(centers_val_);
            if(center_res.has_value())
                extract.set_center(center_res.value());
            else throw std::runtime_error("invalid centers input");
        }
        if(parameters_->count()){
            for(std::string_view parameter:parameters_val_)
                extract.add_set_of_parameters(
                    parse::parameter_tv::param_by_tv_abbr(
                    extract.get_center().value(),
                    std::ranges::split_view(parameter,' ')|
                    std::ranges::to<std::vector<std::string>>()));
        }
        if(collections_->count()){
            std::cout<<"--collections option still not developped"<<std::endl;
        }
        if(time_forecast_->count()){
            //extract.set_time_fcst(time_forecast_val_);
            std::cout<<"--time_fcst option still not developped"<<std::endl;
        }
        if(level_->count()){
            std::cout<<"--level option still not developped"<<std::endl;
        }
        if(grid_->count()){
            if(auto res = grid_notifier(grid_val_);res.has_value())
                extract.set_grid_respresentation(res.value());
            else throw std::runtime_error("invalid grid input");
        }
        if(output_fmt_->count())
            extract.set_output_format(output_fmt_val_);
        // if(app_->got_subcommand("time-interval")){
        //     if(time_interval_->count("--years"))

        //     extract.set_offset_time_interval(time_interval_val_);
        // }
    }
}