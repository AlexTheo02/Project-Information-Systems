#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <cstdlib>
#include <string>
#include <cstring>
#include <random>
#include <chrono>
#include <utility>
#include <type_traits>
#include <list>

using namespace std;

// This is the configuration file for this Project.
// Here you will find:
//
//      1. Global Constants
//      2. Global Definitions
//      3. Operator overloads
//      (4. Specialized Hash Functions for specific Types) - was used in a previous implementation, commented out
//
// For your specified datatype T, you should have the ostream << and istream >> operators overloaded. 

// Default values
#define DEFAULT_L 100
#define DEFAULT_R 14
#define DEFAULT_a 1.0f
#define DEFAULT_k 100
#define DEFAULT_N_THREADS 1
#define DEFAULT_t 0.5f
#define DEFAULT_SHOULD_OMIT true
#define DEFAULT_FILTERED false

// Some functions are accelerated by leveraging parallelism using N_THREADS threads (eg. DirectedGraph::medoid).
static int N_THREADS = DEFAULT_N_THREADS;

// global flag on whether to omit output on tests. Set to non-zero to omit outputs, 0 to allow them.
static bool SHOULD_OMIT = DEFAULT_SHOULD_OMIT;

static bool FILTERED = DEFAULT_FILTERED;

// Custom Cout-like Object that respects the SHOULD_OMIT flag on whether to print or not.
// Cannot be used with endl. Please use << '\n'; instead of << endl;
// Can be chained with simple printable types: c_log << a << b << ... ;
class ConsoleLog{

    public:

    ConsoleLog(){};   // empty constructor

    // overloading the << operator
    template <typename T>
    ConsoleLog& operator<<(const T& t){
        if (!SHOULD_OMIT)
            cout << t;
        return *this;
    }
};
// instantiation of static object
static ConsoleLog c_log;    // each source file that includes config.hpp will have its own instance of c_log





// If you plan to use DirectedGraph::load and DirectedGraph::store functions:

// You should implement << and >> for I/O operations for the specific data type
// 
//      Helpful Links (Tutorials):
//      https://stackoverflow.com/questions/476272/how-can-i-properly-overload-the-operator-for-an-ostream
//      https://stackoverflow.com/questions/69803296/overloading-istream-operator
//      https://learn.microsoft.com/en-us/cpp/standard-library/overloading-the-output-operator-for-your-own-classes?view=msvc-170
//
// Such overloads have already been implemented for some specific standard template types that contain any printable type.
// Your overloads should either precede them or be contained within your class.


// Enable Operator Overloading only for specific types, but using a generic template
// Allowed Types are:
// list
// vector
// set
// .        add more as needed. See code below on how to do it:

// https://medium.com/@sidbhasin82/c-templates-what-is-std-enable-if-and-how-to-use-it-fd76d3abbabe

// Primary generic template for unsupported types           All types begin as unsupported
template <typename Container, typename Enable = void>
struct is_supported_container : false_type {};

// Specialization for vector                                vector is enabled
template <typename T>
struct is_supported_container<vector<T>> : true_type {};

// Specialization for list                                  list is enabled
template <typename T>
struct is_supported_container<list<T>> : true_type {};

// Specialization for set                                   set is enabled
template <typename T>
struct is_supported_container<set<T>> : true_type {};

// Specialization for unordered_set                         unordered_set is enabled
template <typename T>
struct is_supported_container<unordered_set<T>> : true_type {};


// template overload for printing any iterable container (of the above specialized), containing any printable type 
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

// Operator Overloading for Map<Key, Value>. Works for any printable types Key and Value.

// template overload for printing a unordered_map of Key,Value
template <typename Key, typename Value>
ostream& operator<<(ostream& stream, const unordered_map<Key, Value>& m){

    stream << "{";

    bool first = true;  // flag to omit delimiter in first iteration

    for (const auto& pair : m){ // auto = pair<Key, Value>

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

// template overload for reading a map of Key, Value
template <typename Key, typename Value>
istream& operator>>(istream& stream, unordered_map<Key, Value>& m){

    m.clear();

    stream.ignore(1);   // reads and consumes "{"

    // if map is empty
    if (stream.peek() == '}'){
        stream.ignore(1);   // consume the '}'
        return stream;
    }
        
    Key key;
    Value value;

    // how the map string representation is formatted (BNF syntax)
    // pair ::= ε
    // pair ::= (T, set<T>)
    // pair_list ::= ε
    // pair_list ::= pair, pair_list
    // map ::= {pair_list}

    // if we reach here, it is guaranteed that the map is not empty and at least one pair exists in the map's pair_list
    
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


// If implementation makes use of containers that use hashes, this should be uncommented. (unordered_set, unordered_map etc)

// User Requirement: implement the std::hash<T> for the specific T to be used.
// In this scenario, Content Type of graph T is a vector<T2>, where T2 can be any hashable type (or a type with a specialized std::hash function)
// T2 is the inner type (that inside of the vector: int, float, ..., any other type that std::hash<T> has been specialized or exists for)
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
// 