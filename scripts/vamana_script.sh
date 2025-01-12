declare -A args_map=(
    ["Optimal"]="--vamana --stat --dummy --only_unfiltered -n_threads 26 -distance 1 --no_rgraph --pqueue -extra_edges 15"                     # optimal
    ["Serial"]="--vamana --stat --dummy --only_unfiltered -n_threads 1 -distance 1 --no_rgraph --pqueue -extra_edges 15"                       # Serial
    ["Naïve_Euclidean"]="--vamana --stat --dummy --only_unfiltered -n_threads 26 -distance 0 --no_rgraph --pqueue -extra_edges 15"             # bad euclidean
    ["No_Extra_Edges"]="--vamana --stat --dummy --only_unfiltered -n_threads 26 -distance 1 --no_rgraph --pqueue"                              # No Extra Edges
    ["GreedySearch_using_Set"]="--vamana --stat --dummy --only_unfiltered -n_threads 26 -distance 1 --no_rgraph -extra_edges 15"               # No PQueue
    ["Random_Start"]="--vamana --stat --dummy --only_unfiltered -n_threads 26 -distance 1 --random_start --no_rgraph --pqueue -extra_edges 15" # Random Start Instead of Medoid
    ["Initialized_with_Rgraph"]="--vamana --stat --dummy --only_unfiltered -n_threads 26 -distance 1 --pqueue -extra_edges 15"                 # Initialize with R graph
)

main_exe="./bin/main"

for name in "${!args_map[@]}"; do
    args="${args_map[$name]}"
    echo "$name $main_exe $args"
    $main_exe $args > "evaluations/vamana/${name}.txt"
done