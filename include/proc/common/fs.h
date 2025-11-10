#pragma once
#include "sys/error_code.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include "filesystem.h"

ErrorCode make_dir(const std::filesystem::path& filepath) noexcept;

ErrorCode make_file(std::ofstream& file,const std::filesystem::path& out_f_name) noexcept;