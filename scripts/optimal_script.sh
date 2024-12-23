declare -A args_map=(
    ["OptimalStitched-1m"]="--stitched -data data/contest-data-release-1m.bin -queries data/contest-queries-release-1m.bin -groundtruth data/contest-groundtruth-custom-1m.txt -store storedIndices/new_and_optimal/OptimalStitched_latest.txt -dim_data 102 -dim_query 104 -n_data 1000000 -n_queries 10000 -n_groundtruths 10000 --stat -n_threads 26 -distance 1 --no_rgraph --pqueue -extra_edges 50"
    ["OptimalVamana-1m"]="--vamana -data data/contest-data-release-1m.bin -queries data/contest-queries-release-1m.bin -groundtruth data/contest-groundtruth-custom-1m.txt -store storedIndices/new_and_optimal/OptimalVamana_latest-1m.txt -dim_data 102 -dim_query 104 -n_data 1000000 -n_queries 10000 -n_groundtruths 10000 --stat --only_unfiltered -n_threads 26 -distance 1 --no_rgraph --pqueue -extra_edges 15"
    ["OptimalFiltered-1m"]="--filtered -data data/contest-data-release-1m.bin -queries data/contest-queries-release-1m.bin -groundtruth data/contest-groundtruth-custom-1m.txt -store storedIndices/new_and_optimal/OptimalFiltered_latest.txt -dim_data 102 -dim_query 104 -n_data 1000000 -n_queries 10000 -n_groundtruths 10000 --stat -n_threads 26 -distance 1 --pqueue -extra_edges 50"
)


main_exe="./bin/main"

for name in "${!args_map[@]}"; do
    args="${args_map[$name]}"
    echo "$name $main_exe $args"
    $main_exe $args > "evaluations/optimal-1m/${name}.txt"
done