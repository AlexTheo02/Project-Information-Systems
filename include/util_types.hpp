#pragma once

#include "types.hpp"

#include <functional>
#include <algorithm>
#include <limits>
#include <stdexcept>

using namespace std;

// Calculates the euclidian distance between two Nodes (holding vectors)
float euclideanDistance(Node n1, Node n2);

// Returns the node with the minimum distance from a specific node
Node myArgMin(set<Node>, vector<float>, function<float(Node, Node)>);

// Retains the N closest elements of S to X based on distance d
set<Node> closestN(int N, set<Node> S, vector<float> X, function<float(Node, Node)> d);