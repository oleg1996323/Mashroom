#pragma once
#include <array>

constexpr std::array<const char*,11> err_msg_log = {
    "Directory \"{0}\" creation denied",
    "Missing \"format.bin\" file in destination directory \"{0}\"",
    "File \"{0}\" access denied by permissions",
    "Trying to open a not-file \"{0}\"",
    "Cannot access path: \"{0}\"",
    "File \"{0}\" don't exists",
    "Cannot open file \"{0}\"",
    "Invalid path of directories \"{0}\"",
    "\"{0}\" is not a directory",
    "\"{0}\" is not a file",
    "Format file {0} is corrupted"
};

constexpr std::array<const char*,17> err_msg={
    "Process successed",
    "Internal error",
    "Incorrect argument: date interval",
    "Incorrect argument: coordinate position",
    "Incorrect argument: rectangle",
    "Error at command input {0}",
    "Incorrect argument \"{0}\". Already choosen mode: \"{1}\"",
    "Directory \"{0}\" creation denied",
    "File \"{0}\" access denied by permissions",
    "Trying to open a not-file \"{0}\"",
    "Cannot access path: \"{0}\"",
    "File \"{0}\" don't exists",
    "Cannot open file \"{0}\"",
    "Invalid path of directories \"{0}\"",
    "\"{0}\" is not a directory",
    "\"{0}\" is not a file",
    "Ignoring argument \"{0}\""
};