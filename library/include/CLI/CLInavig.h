#pragma once
#include <string>
#include <iostream>
#include <filesystem>
#include <ranges>
#include <vector>
#include "concepts.h"
#include <cstring>
#include <memory>
#include <cassert>

class CLIHandler final{
    std::filesystem::path hist_path_;
    inline static std::unique_ptr<CLIHandler> hCli_= std::unique_ptr<CLIHandler>();
    friend constexpr std::unique_ptr<CLIHandler> std::make_unique<CLIHandler>(const std::filesystem::path&,std::string_view&);
    char* line = nullptr;
    CLIHandler(const std::filesystem::path& dir,std::string_view filename)
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
    void __add_to_history__(std::string_view str) noexcept;
    std::string_view __input__(std::string_view input) noexcept;
    public:
    ~CLIHandler(){
        if(line)
            free(line);
    }
    bool save_history() const noexcept;
    void set_history_length(uint16_t length) noexcept;
    void add_to_history(const String auto& input) noexcept{
        __add_to_history__(std::string_view(input));
    }
    void add_to_history(const RangeOfStrings auto& input) noexcept
    {
        __add_to_history__(std::string_view(std::views::join_with(input,' ')).data());
    }
    bool load_history() noexcept;
    std::string_view input(const String auto& prompt) noexcept{
        return __input__(std::string_view(prompt));
    }
    static CLIHandler& make_instance(const std::filesystem::path& dir,std::string_view filename) noexcept{
        hCli_=std::make_unique<CLIHandler>(dir,filename);
        return *hCli_;
    }
    static CLIHandler& instance() noexcept{
        assert(hCli_);
        return *hCli_;
    }
};