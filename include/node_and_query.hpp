#pragma once

#include "types.hpp"

// Node Overloads:
    template <typename T>        
    bool Node<T>::operator<(const Node& n){ return (this->value < n.value); }// less is defined by the value of the Node

    template <typename T>      
    bool Node<T>::operator==(const Node& n){ return ((this->value == n.value) && (this->category == n.category)); }// same value and same category is considered the same  


// Query Overloads:
    template <typename T>
    bool Query<T>::operator<(const Query& q) { return (this->value < q.value); } // less is defined by the value of the Query

    template <typename T>
    bool Query<T>::operator==(const Query& q) { return ( (this->value == q.value) && (this->type == q.type) && (this->category == q.category) ); } // all should be equal


// istream and ostream overloads for Node and Query (same structure: T, int, int)

    // Primary generic template for unsupported types           All types begin as unsupported
    template <typename Structure, typename Enable = void>
    struct is_supported_structure : false_type {};

    // Specialization for Node                                  Node is enabled
    template <typename T>
    struct is_supported_structure<Node<T>> : true_type {};

    // Specialization for Query                                 Query is enabled
    template <typename T>
    struct is_supported_structure<Query<T>> : true_type {};


    // template overload for printing Node or Query structure. [int | int | T ]
    template<typename Structure>
    enable_if_t<is_supported_structure<Structure>::value, ostream&>
    operator<<(ostream& stream, const Structure& structure) {

        stream << "[";
        
        if constexpr (is_same<Structure, const Query<typename Structure::value_type>>::value) // if Query use type, if Node use id
            stream << structure.type;
        else
            stream << structure.id;

        stream << "|";
        stream << structure.category;
        stream << "|";
        stream << structure.value;
        stream << "]";

        return stream;  // return stream to allow chaining
    }

    // template overload for printing Node or Query structure. [int|int|T]
    template<typename Structure>
    enable_if_t<is_supported_structure<Structure>::value, istream&>
    operator>>(istream& stream, Structure& structure) {

        stream.ignore(1);   // ignore [
        int id_or_type;
        stream >> id_or_type;
        stream.ignore(1);   // ignore |
        int category;
        stream >> category;
        stream.ignore(1);   // ignore |
        typename Structure::value_type value;
        stream >> value;
        stream.ignore(1);   // ignore ]

        // updating the structure
        structure.category = category;
        structure.value = value;
        
        if constexpr (is_same<Structure, Query<typename Structure::value_type>>::value) // if Query use type, if Node use id
            structure.type = id_or_type;
        else
            structure.id =  id_or_type;

        return stream;  // return stream to allow chaining
    }