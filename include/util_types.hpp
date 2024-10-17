#pragma once

#include "types.hpp"

#include <functional>
#include <algorithm>
#include <limits>

using namespace std;

// Returns the node with the minimum distance from a specific node
Node myArgMin(set<Node>, vector<float>, function<float(vector<float>, vector<float>)>);

// Retains the N closest elements of S to X based on distance d
set<Node> closestN(int N, set<Node> S, vector<float> X, function<float(vector<float>, vector<float>)> d);

// finds and returns the node in S with specified id.
Node findByID(int id, set<Node> S);