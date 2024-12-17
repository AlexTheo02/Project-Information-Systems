#!/bin/bash

index_types=("vamana" "filtered" "stitched")

if [ ! -d "bin" ]; then
    echo "building the project. . ."
    make
fi

for index_type in "${index_types[@]}"; do
    echo "Evaluating ${index_type}:"
    bash "scripts/${index_type}_script.sh"
done