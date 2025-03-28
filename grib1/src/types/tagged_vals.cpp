#include "types/tagged_vals.h"

TaggedVal* find_tagged_val_by_tag(TaggedValues* tagged_vals, const char* tag){
    for(int i=0;i<tagged_vals->sz;++i){
        if(strcmp(tagged_vals->vals[i].tag,tag)==0)
            return &tagged_vals->vals[i];
        else continue;
    }
    return NULL;
}

#include <assert.h>

StampVal* push_back_empty_val_to_tag(TaggedVal* tag_val){
    assert(tag_val);
    assert(tag_val->tag);
    if(tag_val->values==NULL){
        if((tag_val->values=(StampVal*)malloc(sizeof(StampVal)))==NULL){
            fprintf(stderr,"Not enough memory. Abort.\n");
            exit(1);    
        }
        tag_val->capacity = 1;
        tag_val->values[0]=StampVal();
    }
    else if(tag_val->sz==tag_val->capacity){
        StampVal* tmp_ptr = NULL;
        size_t new_capacity = tag_val->capacity * 2;
        tmp_ptr = realloc(tag_val->values, new_capacity * sizeof(StampVal));
        //tmp_ptr = (StampVal*)realloc(tag_val->values,tag_val->capacity*2*sizeof(StampVal));
        if(tmp_ptr==NULL){
            if((tmp_ptr = (StampVal*)malloc(tag_val->capacity*2*sizeof(StampVal)))==NULL){
                fprintf(stderr,"Not enough memory. Abort.\n");
                exit(1);
            }
            memcpy(tmp_ptr,tag_val->values,sizeof(StampVal)*tag_val->sz);
            free(tag_val->values);
            tag_val->values = tmp_ptr;
        }
        tag_val->values = tmp_ptr;
        tag_val->capacity = new_capacity;
    }
    tag_val->values[tag_val->sz] = StampVal();
    return &tag_val->values[tag_val->sz++];
}

TaggedVal* push_back_empty_tagged_val(TaggedValues* values){
    assert(values);
    if(values->vals==NULL){
        if((values->vals=(TaggedVal*)malloc(sizeof(TaggedVal)))==NULL){
            fprintf(stderr,"Not enough memory. Abort.\n");
            exit(1);    
        }
        values->capacity = 1;
        values->vals[0]=TaggedVal();
    }
    else if(values->sz==values->capacity){
        size_t new_capacity = values->capacity*2;
        TaggedVal* tmp_ptr = (TaggedVal*)realloc(values->vals,sizeof(TaggedVal)*new_capacity);
        if(tmp_ptr==NULL){
            if((tmp_ptr = (TaggedVal*)malloc(sizeof(TaggedVal)*values->capacity*2))==NULL){
                fprintf(stderr,"Not enough memory. Abort.\n");
                exit(1);
            }
            memcpy(tmp_ptr,values->vals,values->sz*sizeof(TaggedVal));
            free(values->vals);
        }
        values->vals = tmp_ptr;
        values->capacity = new_capacity;
    }
    values->vals[values->sz] = TaggedVal();
    return &values->vals[values->sz++];
}

void delete_tagged_val(TaggedVal* tag_val){
    if(tag_val && tag_val->values)
        free(tag_val->values);
}

void delete_values(TaggedValues tag_vals){
    for(int i=0;i<tag_vals.sz;++i){
        delete_tagged_val(&tag_vals.vals[i]);
    }
    free(tag_vals.vals);
}

void delete_values_by_ptr(TaggedValues* tag_vals){
    assert(tag_vals);
    for(int i=0;i<tag_vals->sz;++i){
        delete_tagged_val(&tag_vals->vals[i]);
    }
    free(tag_vals->vals);
    tag_vals->sz=0;
    tag_vals->capacity=0;
}

StampVal* append_stamped_val(TaggedValues* values, const char* tag, float val, Date* date, Coord* coord){
    assert(values);
    TaggedVal* tag_vals = find_tagged_val_by_tag(values,tag);
    StampVal* stamp_val = NULL;
    if(!tag_vals){
        tag_vals = push_back_empty_tagged_val(values);
        tag_vals->tag = tag;
    }
    stamp_val = push_back_empty_val_to_tag(tag_vals);
    if(!stamp_val){
        fprintf(stdout,"Error at adding stamped value. Abort.");
        exit(1);
    }
    stamp_val->val = val;
    if(date)
        stamp_val->time = *date;
    if(coord)
        stamp_val->coord = *coord;
    return &tag_vals->values[tag_vals->sz-1];
}