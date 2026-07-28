#pragma once
#include "common/AbstractProjection.h"

namespace projection{
    class Cylindric final:public AbstractProjection{
        
        public:
        Cylindric(std::string_view name):
            AbstractProjection(std::move(name)){}

        std::unique_ptr<AbstractProjection> clone() const noexcept override{
            using type =std::decay_t<decltype(*this)>;
            return std::make_unique<type>(*this);
        }
    };
}