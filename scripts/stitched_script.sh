declare -A args_map=(
    ["Optimal"]="--stitched --stat --dummy -n_threads 16 -distance 1 --random_start --no_rgraph --pqueue -extra_edges 50"                # optimal
    ["Naïve_Euclidean"]="--stitched --stat --dummy -n_threads 16 -distance 0 --random_start --no_rgraph --pqueue -extra_edges 50"        # bad euclidean
    ["No_Extra_Edges"]="--stitched --stat --dummy -n_threads 16 -distance 1 --random_start --no_rgraph --pqueue"                         # No Extra Edges
    ["Accumulate_Unfiltered"]="--stitched --stat --dummy -n_threads 16 -distance 1 --random_start --no_rgraph --pqueue --acc_unfiltered" # Accumulate Unfiltered
    ["GreedySearch_using_Set"]="--stitched --stat --dummy -n_threads 16 -distance 1 --random_start --no_rgraph -extra_edges 50"          # No PQueue
    ["Subgraph_Medoid_Start"]="--stitched --stat --dummy -n_threads 16 -distance 1 --no_rgraph --pqueue -extra_edges 50"                 # Subgraph Medoid Start
    ["Subgraphs_initialized_with_Rgraph"]="--stitched --stat --dummy -n_threads 16 -distance 1 --random_start --pqueue -extra_edges 50"  # Subgraph Rgraph Initialization
)

main_exe="./bin/main"

for name in "${!args_map[@]}"; do
    args="${args_map[$name]}"
    echo "$name $main_exe $args"
    $main_exe $args > "evaluations/stitched/${name}.txt"
done