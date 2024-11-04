#pragma once

template<typename C_T>
concept Color = requires(C_T v){
    {
        v.operator<()
    }->bool;
};