#pragma once
#include "common/AbstractProjection.h"

namespace projection{
    class Conical final:public AbstractProjection{
        public:
        Conical(std::string_view name):
            AbstractProjection(name){}
        Conical(const Conical& other) = default;
        Conical(Conical&& other) = default;
        Conical& operator=(const Conical& other) = default;
        Conical& operator=(Conical&& other) noexcept = default;

        std::unique_ptr<AbstractProjection> clone() const noexcept override{
            using type =std::decay_t<decltype(*this)>;
            return std::make_unique<type>(*this);
        }
    };
}