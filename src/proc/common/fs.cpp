#include "proc/common/fs.h"
#include "sys/log_err.h"
#include "sys/application.h"

namespace fs = std::filesystem;

ErrorCode make_dir(const fs::path& filepath) noexcept{
    if(!fs::exists(filepath.parent_path()))
        if(!fs::create_directories(filepath.parent_path())){
            log().record_log(ErrorCodeLog::CANNOT_ACCESS_PATH_X1,"",filepath.relative_path().c_str());
            return ErrorCode::INTERNAL_ERROR;
        }
    return ErrorCode::NONE;
}