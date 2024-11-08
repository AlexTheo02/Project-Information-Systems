#pragma once

#include <iostream>
#include <iterator>
#include <cstdlib>
#include <map>
#include <vector>
#include <set>
#include <cmath>
#include <functional>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <list>
#include <unordered_map>
#include <unordered_set>


struct Id {
    int value;

    Id() : value(-1) {}
    Id(int newValue) : value(newValue) {}

    operator int() const { return value; }

    Id& operator=(int newValue) {
        value = newValue;
        return *this;
    }

    bool operator==(const Id& other) const {
        return value == other.value;
    }

    bool operator==(const int other) const {
        return value == other;
    }
};

// Specialize std::hash for the Id struct
namespace std {
    template <>
    struct hash<Id> {
        std::size_t operator()(const Id& id) const noexcept {
            return std::hash<int>()(id.value);  // Hash based on the integer value
        }
    };
}