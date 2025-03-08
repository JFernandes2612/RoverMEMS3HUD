#pragma once
#include <cstdint>
#include "HUD.hpp"

template <class T>
class HUDT : public HUD
{
protected:
    T* display;
};
