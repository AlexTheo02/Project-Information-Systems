args_list=(
    # Maybe optimal for Vamana
    "--vamana --stat --dummy --unfiltered -n_threads 16 -distance 1 --no_rgraph --pqueue -extra_edges 15"   
    # stripping from optimal:
    "--vamana --stat --dummy --unfiltered -n_threads 16 -distance 1 --no_rgraph --pqueue"                   # no extra edges => worse recall
    "--vamana --stat --dummy --unfiltered -n_threads 16 -distance 1 --no_rgraph -extra_edges 15"            # no pqueue => more time, less recall
    "--vamana --stat --dummy --unfiltered -n_threads 16 -distance 1 --pqueue -extra_edges 15"               # initialize with Rgraph => more time (EFFECT ON RECALL?)
    "--vamana --stat --dummy --unfiltered -n_threads 16 -distance 1 --random_start --no_rgraph --pqueue -extra_edges 15"                  # random start nodes
    "--vamana --stat --dummy --unfiltered -n_threads 16 -distance 0 --no_rgraph --pqueue -extra_edges 15"   # normal euclidean => MORE time
    # "--vamana --stat --dummy --crop_filters -n_threads 16 -distance 2 --no_rgraph --pqueue -extra_edges 15"   # parallel euclidean => MORE MORE time
    # "--vamana --stat --dummy --crop_filters -n_threads 1 -distance 1 --no_rgraph --pqueue -extra_edges 15"    # no threads => MORE time
)

declare -A args_map=(
    ["Optimal"]="--vamana --stat --dummy --unfiltered -n_threads 16 -distance 1 --no_rgraph --pqueue -extra_edges 15.txt" # optimal
    ["Naïve_Euclidean"]="--vamana --stat --dummy --unfiltered -n_threads 16 -distance 0 --no_rgraph --pqueue -extra_edges 15.txt" # bad euclidean
    ["No_Extra_Edges"]="--vamana --stat --dummy --unfiltered -n_threads 16 -distance 1 --no_rgraph --pqueue.txt"                 # No Extra Edges
    ["GreedySearch_using_Set"]="--vamana --stat --dummy --unfiltered -n_threads 16 -distance 1 --no_rgraph -extra_edges 15.txt"          # No PQueue
    ["Random_Start"]="--vamana --stat --dummy --unfiltered -n_threads 16 -distance 1 --random_start --no_rgraph --pqueue -extra_edges 15.txt" # Random Start Instead of Medoid
    ["Initialized_with_Rgraph"]="--vamana --stat --dummy --unfiltered -n_threads 16 -distance 1 --pqueue -extra_edges 15.txt"             # Initialize with R graph
)

main_exe="./bin/main"

for name in "${!args_map[@]}"; do
    args="${args_map[$name]}"
    echo "$name $main_exe $args"
    $main_exe $args > "evaluations/vamana/${name}.txt"
done