#ifndef COLOUR_H
#define COLOUR_H

class Colour
{
public:
    Colour(float r, float g, float b, float a = 1.0f)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
    {}

    Colour(glm::vec3 input)
    : r(input.r)
    , g(input.g)
    , b(input.b)
    , a(1.0f)
    {}

    operator uint32_t()
    {
        uint32_t out = 0;
        out |= (uint8_t)(r * 255) << 24;
        out |= (uint8_t)(g * 255) << 16;
        out |= (uint8_t)(b * 255) <<  8;
        out |= (uint8_t)(a * 255) <<  0;
        return out;
    }

    float r;
    float g;
    float b;
    float a;
};

#endif
