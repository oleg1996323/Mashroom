#pragma once
#include <vector>
#include <string>

template<typename ARRAY_T>
struct Array{
    std::vector<ARRAY_T> values_;

    static Array parse(const std::vector<std::string>& input){
        std::vector<std::string> processed_tokens;
            bool closed = true;
            std::vector<std::string> current_string;
            for(const std::string& inp_str:input){
                if(inp_str.starts_with('[') && inp_str.ends_with(']')){
                    if(closed==true){
                        if(inp_str.size()>2){
                            processed_tokens.push_back(inp_str.substr(1,inp_str.size()-2));
                        }
                        else continue;
                    }
                    else{
                        err_=ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"quoted values in composed text must be escaped",AT_ERROR_ACTION::CONTINUE,inp_str);
                        return;
                    }
                }
                else if(closed==true){
                    if(inp_str.starts_with('[')){
                        closed = false;
                        if(inp_str=="[")
                            continue;
                        else current_string.push_back(inp_str.substr(1));
                    }
                    else if(inp_str.ends_with(']')){
                        err_=ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,inp_str);
                        return;
                    }
                    else{
                        processed_tokens.push_back(inp_str);
                        continue;
                    }
                }
                else{
                    if(inp_str.ends_with(']')){
                        closed = true;
                        if(inp_str=="]"){
                            if(!current_string.empty()){
                                processed_tokens.push_back(std::ranges::join_with_view(current_string,' ')|std::ranges::to<std::string>());
                                current_string.clear();
                            }
                            else continue;
                        }
                        else{
                            current_string.push_back(inp_str.substr(0,inp_str.size()-1));
                            processed_tokens.push_back(std::ranges::join_with_view(current_string,' ')|std::ranges::to<std::string>());
                            current_string.clear();
                        }
                    }
                    else if(inp_str.starts_with(']')){
                        err_ = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,inp_str);
                        return;
                    }
                    else current_string.push_back(inp_str);
                }
            }
    }
};