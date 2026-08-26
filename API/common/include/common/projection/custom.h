#pragma once
#include <vector>
#include <OsterLib/byte_read.h>
#include <OsterLib/byte_order.h>
#include "AbstractProjection.h"

namespace projection{
    namespace detail{
        class BitField{
            //@todo
            uint8_t bits_to_read_;
        };
    }

    class CustomProjection:public AbstractProjectionMaker<CustomProjection>{
        std::vector<detail::BitField> fields_;
        CustomProjection()=default;
        public:
        CustomProjection(const CustomProjection& other):
        AbstractProjectionMaker(other),
        fields_(other.fields_){}
        CustomProjection(CustomProjection&& other):
        AbstractProjectionMaker(other),
        fields_(std::move(other.fields_)){}
        CustomProjection& operator=(const CustomProjection& other){
            if(this!=&other){
                AbstractProjectionMaker::operator=(other);
                fields_=other.fields_;
            }
            return *this;
        }
        CustomProjection& operator=(CustomProjection&& other){
            if(this!=&other){
                AbstractProjectionMaker::operator=(std::move(other));
                fields_=std::move(other.fields_);
            }
            return *this;
        }
        virtual double left_bound(Lat latitude) const noexcept override{
            //@todo
        }
        virtual double right_bound(Lat latitude) const noexcept override{
            //@todo
        }
        virtual double top_bound(Lon longitude) const noexcept override{
            //@todo
        }
        virtual double bottom_bound(Lon longitude) const noexcept override{
            //@todo
        }
        virtual bool position_in(Coord position) const noexcept override{
            //@todo
        }

        // Клонирование для полиморфного копирования
        virtual std::unique_ptr<AbstractProjection> clone() const noexcept override{
            using type =std::decay_t<decltype(*this)>;
            return std::make_unique<type>(*this);
        }
    };
}