#include <path_process.h>

path::Storage<false>::Storage(const std::string& path,TYPE type):path_(path),type_(type){}
path::Storage<false>::Storage(std::string&& path,TYPE type):path_(path),type_(type){}
path::Storage<false>::Storage(std::string_view path,TYPE type):path_(path),type_(type){}
path::Storage<false>::Storage(const path::Storage<false>& other):
path_(other.path_),type_(other.type_){}
path::Storage<false>::Storage(const path::Storage<true>& other):
path_(other.path_),type_(other.type_){}

path::Storage<true>::Storage(const std::string& path,TYPE type){}
path::Storage<true>::Storage(std::string&& path,TYPE type){}
path::Storage<true>::Storage(std::string_view path,TYPE type){}
path::Storage<true>::Storage(const path::Storage<false>& other):
path_(other.path_),type_(other.type_){}
path::Storage<true>::Storage(const path::Storage<true>& other):
path_(other.path_),type_(other.type_){}