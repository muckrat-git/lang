#pragma once

#include <cstdint>
#include <math.h>

namespace Muck {
    // Fixed point decimal number
    template <typename type>
    class FixedPoint {
        public:
        int point = 100;
        type value;

        // - Default Point Constructors -
        FixedPoint(float val) {
            value = (type)roundf(val * (float)point);
        }
        FixedPoint(double val) {
            value = (type)roundf(val * (double)point);
        }

        // - Set Point Constructors -
        FixedPoint(float val, int places) {
            point = pow(10, point);
            value = (type)roundf(val * (float)point);
        }
        FixedPoint(double val, int places) {
            point = pow(10, point);
            value = (type)round(val * (double)point);
        }

        // - Operator Overloads -

        // - Conversion Operators -
        operator float() const {
            // Calculate decimal
            float dec = (float)(value % point) / (float)point;
            // Calculate whole number
            float whole = (float)(value / point);
            return whole + dec;
        }
        operator double() const {
            // Calculate decimal
            double dec = (double)(value % point) / (double)point;
            // Calculate whole number
            double whole = (double)(value / point);
            return whole + dec;
        }
        operator auto() const {
            return value / point;
        }

        // - Arithmatic Operators -
        const FixedPoint operator+(const FixedPoint & a) {
            return (FixedPoint){
                .point = point,
                .value = a.value + value
            };
        }
    };

    // Simplified fixed point types
    typedef FixedPoint<int8_t>  fix8_t;
    typedef FixedPoint<int16_t> fix16_t;
    typedef FixedPoint<int32_t> fix32_t;
    typedef FixedPoint<int64_t> fix64_t;

    // Default fixed point type
    typedef fix32_t fix;
};