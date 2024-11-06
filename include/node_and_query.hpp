#pragma once

#include "types.hpp"

// Node Overloads:
    template <typename T>        
    bool Node<T>::operator<(const Node& n){ return (this->value < n.value); }// less is defined by the value of the Node

    template <typename T>      
    bool Node<T>::operator==(const Node& n){ return ((this->value == n.value) && (this->category == n.category)); }// same value and same category is considered the same  

    template <typename T>
    bool Node<T>::empty(){ return (this->belongsIn->isEmpty(this->value)); }


// Query Overloads:
    template <typename T>
    bool Query<T>::operator<(const Query& q) { return (this->value < q.value); } // less is defined by the value of the Query

    template <typename T>
    bool Query<T>::operator==(const Query& q) { return ( (this->value == q.value) && (this->type == q.type) && (this->category == q.category) ); } // all should be equal

    template <typename T>
    bool Query<T>::empty(){ return (this->category == -1 || this->belongsIn->isEmpty(this->value)); }


// istream and ostream overloads for Node and Query (same _class: T, int, int)

    // Primary generic template for unsupported types           All types begin as unsupported
    template <typename _Class, typename Enable = void>
    struct is_supported__class : false_type {};

    // Specialization for Node                                  Node is enabled
    template <typename T>
    struct is_supported__class<Node<T>> : true_type {};

    // Specialization for Query                                 Query is enabled
    template <typename T>
    struct is_supported__class<Query<T>> : true_type {};


    // template overload for printing Node or Query _class. Node: [int|int|T], Query: [int|int|int|T]
    template<typename _Class>
    enable_if_t<is_supported__class<_Class>::value, ostream&>
    operator<<(ostream& stream, const _Class& _class) {

        stream << "[";
                
        stream << _class.id;
        stream << "|";
        stream << _class.category;
        stream << "|";
        if constexpr (is_same<_Class, const Query<typename _Class::value_type>>::value){ // if Query use type, if Node use id
            stream << _class.filtered;
            stream << "|";
        }
        stream << _class.value;
        stream << "]";

        return stream;  // return stream to allow chaining
    }

    // template overload for printing Node or Query _class. Node: [int|int|T], Query: [int|int|int|T]
    template<typename _Class>
    enable_if_t<is_supported__class<_Class>::value, istream&>
    operator>>(istream& stream, _Class& _class) {

        stream.ignore(1);       // ignore [
        int id;
        stream >> id;
        stream.ignore(1);       // ignore |
        int category;
        stream >> category;
        stream.ignore(1);       // ignore |
        if constexpr (is_same<_Class, Query<typename _Class::value_type>>::value){ // if Query use type, if Node use id
            bool filtered;
            stream >> filtered;
            _class.filtered = filtered;
            stream.ignore(1);   // ignore |
        }
        typename _Class::value_type value;
        stream >> value;
        stream.ignore(1);       // ignore ]

        // updating the _class
        _class.id = id;
        _class.category = category;
        _class.value = value;

        return stream;  // return stream to allow chaining
    }