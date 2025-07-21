#include "boost_functional/json.h"

std::expected<boost::json::value,std::error_code> parse_json(const fs::path& path) noexcept{
    using namespace boost;
    if(fs::exists(path)){
        std::ifstream file(path,std::ifstream::in);
        if(!file.is_open())
            return std::unexpected(std::make_error_code(std::errc::file_exists));
        json::stream_parser parser;
        json::error_code err_code;
        std::vector<char> buffer;
        std::error_code err;
        size_t buf_sz = fs::file_size(path,err);
        if(err.value()!=0)
            return std::unexpected(err);
        buffer.resize(buf_sz);
        while(file.good()){
            file.read(buffer.data(),buffer.size());
            parser.write(buffer.data(),file.gcount(),err_code);
            if(err_code)
                return std::unexpected(std::make_error_code(std::errc::bad_message));
        }
        if(!parser.done())
            return std::unexpected(std::make_error_code(std::errc::bad_message));
        else
            parser.finish();
        return parser.release();
    }
    else return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
}