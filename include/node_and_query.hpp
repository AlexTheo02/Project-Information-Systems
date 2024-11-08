#pragma once

#include "types.hpp"
#include "id.hpp"

// Node Overloads:

    // overloads the less operator <
    template <typename T>    
    bool Node<T>::operator<(const Node& n) const{ return (this->value < n.value); }// less is defined by the value of the Node

    // overloads the equality operator ==
    template <typename T>      
    bool Node<T>::operator==(const Node& n) const { return ((this->value == n.value) && (this->category == n.category)); }// same value and same category is considered the same  

    // overloads the ostream operator <<        Node: [int|int|T]
    template <typename T>
    ostream& operator<<(ostream& stream, const Node<T>& node){

        stream << "[" 
        << node.id << "|"
        << node.category << "|"
        << node.value << "]";

        return stream;  // return stream to allow chaining 
    }

    // overloads the istream operator >> 
    template<typename T>
    istream& operator>>(istream& stream, Node<T>& node) {

        stream.ignore(1);       // ignore [
        int id;
        stream >> id;
        stream.ignore(1);       // ignore |
        int category;
        stream >> category;
        stream.ignore(1);       // ignore |
        T value;
        stream >> value;
        stream.ignore(1);       // ignore ]

        // updating the _class
        node.id = id;
        node.category = category;
        node.value = value;

        return stream;  // return stream to allow chaining
    }

// Node method implementation:

    // a Node is empty when its value is empty
    template <typename T>
    bool Node<T>::empty(){ return (this->isEmpty(this->value)); }


// Query Overloads:

    // overloads the less operator <
    template <typename T>
    bool Query<T>::operator<(const Query& q) { return (this->value < q.value); } // less is defined by the value of the Query

    // overloads the equality operator ==
    template <typename T>
    bool Query<T>::operator==(const Query& q) { return ( (this->value == q.value) && (this->type == q.type) && (this->category == q.category) ); } // all should be equal

    // overloads the ostream operator <<        Query: [int|int|int|T]
    template <typename T>
    ostream& operator<<(ostream& stream, const Query<T>& query){

        stream << "["
        << query.id << "|"
        << query.category << "|"
        << query.filtered << "|"
        << query.value << "]";

        return stream;  // return stream to allow chaining 
    }

    // overloads the istream operator >>
    template<typename T>
    istream& operator>>(istream& stream, Query<T>& query) {

        stream.ignore(1);       // ignore [
        int id;
        stream >> id;
        stream.ignore(1);       // ignore |
        int category;
        stream >> category;
        stream.ignore(1);       // ignore |
        bool filtered;
        stream >> filtered;
        stream.ignore(1);       // ignore |
        T value;
        stream >> value;
        stream.ignore(1);       // ignore ]

        // updating the query
        query.id = id;
        query.category = category;
        query.filtered = filtered;
        query.value = value;

        return stream;  // return stream to allow chaining
    }

// Query method implementation:

    // a Query is empty when either the category is -1 or the value is empty
    template <typename T>
    bool Query<T>::empty(){ return (this->category == -1 || this->isEmpty(this->value)); }