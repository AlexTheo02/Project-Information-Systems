#pragma once

#include "types.hpp"


Id& Id::operator=(int newValue) { this->value = newValue; return *this; }

bool Id::operator==(const Id& other) const { return this->value == other.value; }

bool Id::operator==(const int other) const { return this->value == other; }

bool Id::operator<(const Id& other) const { return this->value < other.value; }

bool Id::operator<(const int other) const { return this->value < other; }

bool Id::operator>=(const int other) const { return this->value >= other; }

Id& Id::operator++() { this->value++; return *this; }


// Overload the >> operator for Id
istream& operator>>(istream& stream, Id& id) {
    stream >> id.value;  // Read an integer from the stream and assign it to id.value
    return stream;
}

// Overload the << operator for Id
ostream& operator<<(ostream& stream, const Id& id) {
    stream << id.value;  // push the integer value to the stream
    return stream;
}

