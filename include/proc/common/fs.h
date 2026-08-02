#pragma once
#include "sys/error.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include "OsterLib/filesystem.h"

void make_and_open_file(std::ofstream& file,const std::filesystem::path& out_f_name);