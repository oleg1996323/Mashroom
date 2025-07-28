#pragma once
#include <string>
#include <iostream>
#include <filesystem>
#include <ranges>
#include <vector>
#include "linenoise.h"
#include "concepts.h"
#include <cstring>
#include <memory>


class CLIHandler final{
    std::filesystem::path hist_path_;
    inline static std::unique_ptr<CLIHandler> hCli_= std::unique_ptr<CLIHandler>();
    friend constexpr std::unique_ptr<CLIHandler> std::make_unique<CLIHandler>(const std::filesystem::path&,const std::string&);
    char* line = nullptr;
    CLIHandler(const std::filesystem::path& dir,const std::string& filename)
    {
        assert(!filename.empty());
        if(!std::filesystem::exists(dir)){
            if(!std::filesystem::create_directories(dir))
                hist_path_ = std::filesystem::current_path()/filename;
            else hist_path_ = dir/filename;
        }
        else hist_path_ = dir/filename;
        load_history();
    }
    public:
    ~CLIHandler(){
        if(line)
            free(line);
    }
    bool save_history() const noexcept{
        return linenoiseHistorySave(hist_path_.c_str())==0?true:false;
    }
    void set_history_length(uint16_t length) noexcept{
        linenoiseHistorySetMaxLen(length);
    }
    void add_to_history(const String auto& input) noexcept{
        linenoiseHistoryAdd(std::string_view(input).data());
    }
    void add_to_history(const RangeOfStrings auto& input) noexcept
    {
        linenoiseHistoryAdd(std::string_view(std::views::join_with(input,' ')).data());
    }
    bool load_history() noexcept{
        return linenoiseHistoryLoad(hist_path_.c_str())==0?true:false;
    }
    std::string_view input(const String auto& prompt) noexcept{
        if(line)
            free(line);
        line = linenoise(std::string_view(prompt.begin(),prompt.end()).data());
        if(line==nullptr)
            return std::string_view();
        else {
            add_to_history(std::string_view(line));
            save_history();
        }
        return std::string_view(line,line+std::strlen(line));
    }
    static CLIHandler& make_instance(const std::filesystem::path& dir,const std::string& filename) noexcept{
        hCli_=std::make_unique<CLIHandler>(dir,filename);
        return *hCli_;
    }
    static CLIHandler& instance() noexcept{
        assert(hCli_);
        return *hCli_;
    }
};