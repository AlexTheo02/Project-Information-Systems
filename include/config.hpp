#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <cstdlib>
#include <string>
#include <cstring>
#include <random>
#include <chrono>
#include <utility>
#include <type_traits>
#include <list>



#define N_THREADS 8 // Some functions are accelerated by leveraging parallelism using N_THREADS threads (eg. DirectedGraph::medoid).
// global constant on whether to omit output on tests. Set to non-zero to omit outputs, 0 to allow them.
#define SHOULD_OMIT 1

// Checks with the SHOULD_OMIT flag on whether to omit or allow output (sets state).
// https://stackoverflow.com/questions/30184998/how-to-disable-cout-output-in-the-runtime
#define OMIT_OUTPUT if (SHOULD_OMIT) cout.setstate(ios_base::failbit)


using namespace std;


// User Requirement: implement the std::hash<T> for the specific T to be used.
// In this scenario, Content Type of graph T is a vector<T2>, where T2 can be anything already implemented (or expanded) in the std::hash
// T2 is the inner type (that inside of the vector: int, float, ..., any other type that std::hash<T> has been expanded for)
// namespace std {
//     // https://en.cppreference.com/w/cpp/container/map - unordered map hash defaults to std::hash<TYPE> => specialize for given type TYPE.
//     // https://stackoverflow.com/questions/10405030/c-unordered-map-fail-when-used-with-a-vector-as-key - Hash Function for vectors.
//     template <typename T2>
//         class hash<vector<T2>>{
//         public:
//             size_t operator()(const vector<T2>& t) const{

//                 size_t ret = t.size();
//                 for (const auto& v : t) {
//                     ret ^=  hash<T2>()(v) + 0x9e3779b9 + (ret << 6) + (ret >> 2);
//                 }
//                 return ret;
//             }
//     };
// };
// from previous implementation. Commented out instead of deleted because we might need it again in future updates.

// User Requirement: implement << and >> for I/O operations for the specific data type
// https://stackoverflow.com/questions/476272/how-can-i-properly-overload-the-operator-for-an-ostream
// https://stackoverflow.com/questions/69803296/overloading-istream-operator



// Enable Operator Overloading only for specific types, but using a generic template
// Allowed Types are:
// list
// vector
// set
// ADD MORE IF NEEDED

// https://medium.com/@sidbhasin82/c-templates-what-is-std-enable-if-and-how-to-use-it-fd76d3abbabe

// Primary template for unsupported types
template <typename Container, typename Enable = void>
struct is_supported_container : false_type {};

// Specialization for vector
template <typename T>
struct is_supported_container<vector<T>> : true_type {};

// Specialization for list
template <typename T>
struct is_supported_container<list<T>> : true_type {};

// Specialization for set
template <typename T>
struct is_supported_container<set<T>> : true_type {};


// template overload for printing any iterable container, containing any printable type 
template<typename Container>
enable_if_t<is_supported_container<Container>::value, ostream&>
operator<<(ostream& stream, const Container& container) {

    stream << "<";

    bool first = true;  // flag to omit delimiter in first iteration
    for (auto it = container.begin(); it != container.end(); it++){

        if (first){
            stream << *it;  // dereference the iterator and print value in stream
            first = false;  // from now on print delimiters
        }
        else{
            stream << ", " << *it;  // print delimiter and value
        }
    }
    stream << ">";
    return stream;  // return stream to allow chaining
}


// template overload for reading any iterable container, containing any printable type.
template<typename Container>
enable_if_t<is_supported_container<Container>::value, istream&>
operator>>(istream& stream, Container& container) {  // container is updated by reference
    
    container.clear();  // ensure container will be replaced if it was non empty

    stream.ignore(1);   // reads and consumes "<"

    typename Container::value_type value;   // value is of type Container::value_type which is the type of the elements in the container
    // maybe auto could work, but we assume this is safer when reading from a stream

    while (stream >> value){    // loop condition: formatted read from stream and attempt to store at Container::value_type type variable
        // stream >> value returns true if it can successfully read a Container::value_type variable from the stream
        // stream >> value returns false if the stream input cannot be converted into a Container::value_type variable format

        // if Container is vector use push_back(), else use insert(). Vector insert requires position.
        if constexpr (is_same<Container, vector<typename Container::value_type>>::value)  //https://en.cppreference.com/w/cpp/types/is_same
            container.push_back(value);
        else
            container.insert(value);   // correct format for value is inserted into the container

        if (stream.peek() == '>'){ // check if we have reached the end of the container (in string representation)
            stream.ignore(1);   // consumes the '>'
            break; 
        }
        // consume delimiter: ", " (= consume 2 characters)
        stream.ignore(2);
    }

    return stream;
}

// Operator Overloading for Map (adjacency). Works for any printable type T.

// template overload for printing a map of T,set<T>
template <typename T>
ostream& operator<<(ostream& stream, const map<T, set<T>>& m){

    stream << "{";

    bool first = true;  // flag to omit delimiter in first iteration

    for (const auto& pair : m){

        if (first){
            stream << "(" << pair.first << ", " << pair.second << ")";  // dereference the iterator->first for key and seconds for value and print value in stream
            first = false;  // from now on print delimiters
        }
        else{
            stream << ", " << "(" << pair.first << ", " << pair.second << ")";
        }
    }
    stream << "}";
    return stream;
}

// template overload for reading a map of T,set<T>
template <typename T>
istream& operator>>(istream& stream, map<T, set<T>>& m){

    m.clear();

    stream.ignore(1);   // reads and consumes "{"

    // if map is empty
    if (stream.peek() == '}'){
        stream.ignore(1);   // consume the '}'
        return stream;
    }
        
    T key;
    set<T> value;

    // pair ::= ε
    // pair ::= (T, set<T>)
    // pair_list ::= ε
    // pair_list ::= pair, pair_list
    // map ::= {pair_list}

    // if we reach here, it is guaranteed that the map is not empty and a pair exists
    
    stream.ignore(1);   // consume the first parenthesis of the pair

    while (stream >> key){  // loop condition: formatted read from stream and attempt to store T type variable (key)
        // stream >> value returns true if it can successfully read a Container::value_type variable from the stream
        // stream >> value returns false if the stream input cannot be converted into a Container::value_type variable format
        
        stream.ignore(2);   // consume in-pair delimiter ", "

        stream >> value;    // read the value

        m[key] = value;     // create the pair and put it in the map

        stream.ignore(1);   // consume pair closing parenthesis ')'
        
        if (stream.peek() == '}'){  // check if we have reached the end of the map (in string representation)
            stream.ignore(1);       // consume the '}'
            return stream;          
        }

        // consume delimiter ", "
        stream.ignore(2);
        // consume "(" for the next pair
        stream.ignore(1);
    }

    return stream;
}