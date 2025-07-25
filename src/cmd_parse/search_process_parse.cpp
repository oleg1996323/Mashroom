#include "cmd_parse/search_process_parse.h"
#include <ranges>

namespace parse{
    std::expected<Organization,ErrorCode> center_notifier(const std::string& input){
        split<std::string>(input,"; ");
        auto centers = multitoken_approx_match_center(split<std::string>(input,"; "));
        //if abbreviation
        if(centers.empty())
            return std::unexpected(ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                    "invalid center value",AT_ERROR_ACTION::CONTINUE,input));
        else if(centers.size()==1)
            return centers.front();
        else{
            std::cout<<"Matched more than 1 center:"<<std::endl;
            auto txt_centers = std::views::transform(centers,[](Organization org)
            {
                return std::string_view(center_to_abbr(org));
            });
            for(auto org:std::views::join_with(txt_centers,"; "))
                std::cout<<org<<std::endl;
            return std::unexpected(ErrorCode::INTERNAL_ERROR);
        }
    }

    void SearchProcess::init() noexcept{
        add_options("j",po::value<int>()->notifier([this](int input){
            search_proc_->set_using_processor_cores(input);
        }),"Number of used threads. Number may be discarded to the maximal physical number threads")
        ("outp",po::value<std::vector<std::string>>()->required()->notifier([this](const std::vector<std::string>& paths){
            assert(search_proc_);
            for(auto& path:paths)
                if(err_ = search_proc_->add_in_path(path);err_!=ErrorCode::NONE)
                    return;
        }),"Output path. May be directory or file path")
        ("inp",po::value<std::vector<std::string>>()->required()->notifier([this](const std::vector<std::string>& paths) noexcept{
            assert(search_proc_);
            for(auto& path:paths){
                err_ = search_proc_->add_in_path(path);
                if(err_!=ErrorCode::NONE)
                    return;
            }
        }),"Specify the input paths")
        ("datefrom",po::value<utc_tp>()->notifier([this](utc_tp input){
            search_proc_->set_from_date(input);
        }),"")
        ("dateto",po::value<utc_tp>()->notifier([this](utc_tp input){
            search_proc_->set_to_date(input);
        }),"")
        ("pos",po::value<Coord>()->notifier([this](const Coord& input){
            search_proc_->set_position(input);
        }),"")
        ("lattop",po::value<Lat>(),"Top latitude of rectangle")
        ("latbot",po::value<Lat>(),"Latitude bottom of rectangle")
        ("lonleft",po::value<Lon>(),"Left-side longitude of rectangle")
        ("lonrig",po::value<Lon>(),"Right-side longitude of rectangle")
        ("center",po::value<std::string>(),"Specify the center that released the data")
        ("tabv-param",po::value<std::vector<SearchParamTableVersion>>()->notifier(
            [this](const std::vector<SearchParamTableVersion>& input){
                for(auto& param_tv:input)
                    search_proc_->add_parameter(param_tv);
        }),"Specify the expected parameters to process")
        ("tabv-pname",po::value<std::vector<std::string>>()->notifier(
            [this](const std::vector<std::string>& input){
                if(!search_proc_->get_center().has_value())
                    err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,
                    "undefined center (must be previously defined)",AT_ERROR_ACTION::ABORT);
                for(auto& param_tv:input){
                    auto param = parse::parameter_tv::param_by_tv_abbr(search_proc_->get_center().value(),param_tv);
                    if(!param.has_value()){
                        err_=param.error();
                        return;
                    }
                    else search_proc_->add_parameter(param.value());
                }
        }),"Specify the expected parameters to process by table version and parameter name")
        ("collection",po::value<std::vector<std::string>>()/** @todo*/,"Specify by name of collection")
        ("time_fcst",po::value<std::string>(),"Specify the forecast time of the released data")
        ("grid",po::value<RepresentationType>()->notifier([this](RepresentationType input){
            search_proc_->set_grid_respresentation(input);
        }),"Specify the expected grid type");
        define_uniques();
    }

    ErrorCode SearchProcess::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        if(vm.contains("center")){
            auto center_res = center_notifier(vm.at("center").as<std::string>());
            if(center_res.has_value())
                search_proc_->set_center(center_res.value());
            else{
                err_ = center_res.error();
                return err_;
            }
        }
        err_=try_notify(vm);
        return err_;
    }

    ErrorCode SearchProcess::parse(::AbstractSearchProcess* ptr,const std::vector<std::string>& args){
        if(ptr==nullptr)
            return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,
                "zero pointer passed. Abort.",AT_ERROR_ACTION::ABORT);
        else{
            search_proc_ = ptr;
            return AbstractCLIParser::parse(args);
        }
    }
}