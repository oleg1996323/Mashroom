#pragma once
#include <unordered_map>
#include <string>
#include <set>
#include <unordered_set>
#ifdef __cplusplus
    extern "C"{
        #include "PDSdate.h"
        #include "func.h"
    }
#endif  
#include "base.h"
#include "tags.h"

struct DateInterval{
    Date from_;
    Date to_;
    Rect rect_;
    bool operator<(const DateInterval other) const{
        return ::date_less(&from_,&other.from_)?true:(::date_less(&to_,&other.to_))?true:false;
    }

    bool operator==(const DateInterval& other) const{
        return from_==other.from_ && to_==other.to_ && rect_==other.rect_;
    }

    bool operator!=(const DateInterval& other) const{
        return !(*this==other);
    }
};

class DataInfo{
    public:
    void add_data_info(Collection coll_name,Tag tag,Rect rect,Date from,Date to){
        if(coll_name!=Collection::NONE && tag!=Tag::None && correct_rect(&rect)){
            if(data_[coll_name].contains(tag)){
                DateInterval& element = data_[coll_name][tag];
                if(element.from_>from)
                    element.from_=from;
                if(element.to_<to)
                    element.to_=to;
                Rect el_rect = merge_rect(&element.rect_,&rect);
                if(is_correct_rect(&el_rect))
                    element.rect_ = el_rect;
            }
            else{
                DateInterval& element = data_[coll_name][tag];
                element.from_=from;
                element.to_=to;
                element.rect_=rect;
            }
        }
        return;
    }
    bool is_in_collections(Collection coll,Coord pos,Date from, Date to,Tag tag){
        const DateInterval& interval = get_date_interval(coll,tag);
        if(interval!=empty_interval && 
                from>=interval.from_ && 
                to<=interval.to_ &&
                point_in_rect(&interval.rect_,pos))
            return true;
        else return false;
    }
    
    const DateInterval& get_date_interval(Collection coll,Tag tag){
        if(data_.contains(coll) && data_.at(coll).contains(tag)){
            return data_.at(coll).at(tag);
        }
        else return empty_interval;
    }

    private:
    std::unordered_map<Collection,std::unordered_map<Tag,DateInterval>> data_;
    static DateInterval empty_interval;
};