#include "functions.h"
#include "sys/base.h"
#ifdef __cplusplus
extern "C"{
    #include "PDSdate.h"
}
#endif

using namespace std::string_literals;

namespace auxiliairy{
    bool is_correct_date_interval(const Date& from, const Date& to){
        if(!is_correct_date(&from) || !is_correct_date(&to))
            return false;
        else{
            if(from>to)
                return false;
            else return true;
        }
    }
    bool is_in_accessible_positions(const Coord& pos){
        //TODO
    }
    Rect get_accessible_rect(){
        //TODO
    }
}