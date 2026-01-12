#pragma once
#include "sys/error_code.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include "filesystem.h"

void make_dir(const std::filesystem::path& filepath);

void make_and_open_file(std::ofstream& file,const std::filesystem::path& out_f_name);