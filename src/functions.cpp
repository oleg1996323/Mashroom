#include "functions.h"
#ifdef __cplusplus
extern "C"{
    #include <decode.h>
    #include <capitalize.h>
}
#endif

using namespace std::string_literals;

// void extract_by_date(std::filesystem::path filename, const Date& date,std::filesystem::path output){
//     if(output.c_str()=="")
//         throw std::invalid_argument("Not defined output file");
//     std::filesystem::perms perms = std::filesystem::directory_entry(output).status().permissions();
//     {
//         using namespace std::filesystem;
//         if((perms & (perms::group_write | perms::others_write | perms::owner_write))==perms::none)
//             throw std::runtime_error("Permission denied");
//     }

//     const char* outfn = output.c_str();
//     FILE* output_file_stream = stdout;
//     if(output_file_stream){
//         //-s -text -4yr jan.grib
//         //-V -text -4yr
        
//         const char* fn = filename.c_str();
        
        
//         std::array<const char*,5> args = {"-bin","-4yr","-cap","-f=cyM",fn};
        
//         std::cout<<filename.c_str()<<args[0]<<args[1]<<std::endl;
//         capitalize(fn, std::string(std::string(getenv("HOME"))+"/data"s).data(), "CYM", BINARY);
//         //decode(args.size(), args.data());
//         GridSize size;
//         bool x = false;
//         bool y = false;
//         std::string buf;
//         int CRCLcount = 0;
//         char ch;
//         while((ch = (char)getc(stdout))>='0' && ch<='9'){
//             buf.push_back(ch);
//             if(buf.size()>10)
//                 throw std::overflow_error("Incorrect reading file. Prompt: bad data");
//         }
//         size.x = std::atoi(buf.data());
//         x = true;
//         buf.clear();
//         while((ch = (char)getc(stdout))>='0' && ch<='9'){
//             buf.push_back(ch);
//             if(buf.size()>10)
//                 throw std::overflow_error("Incorrect reading file. Prompt: bad data");
//         }
//         size.y = std::atoi(buf.data());
//         y = true;
//         buf.clear();

//         size_t lat;
//         size_t lon;
//         size_t line_count = 0;
//         size_t column_count = 0;
//         while((ch = (char)getc(stdout))!= EOF){
//             if(ch!='\n' && ch!='\r')
//                 buf.push_back(ch);
//             else{
//                 if(column_count<size.x){
//                     ++column_count;
//                     fwrite(buf.data(),sizeof(char),buf.size(),output_file_stream);
//                 }
//                 else{
//                     column_count==0;
//                     if(line_count<size.y)
//                         ++line_count;
//                     else{
//                         fclose(output_file_stream);
//                         return;
//                     }
//                 }
//             }
//         }
//     }
// }
