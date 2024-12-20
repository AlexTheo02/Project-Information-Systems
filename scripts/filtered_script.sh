declare -A args_map=(
    ["Optimal"]="--filtered --stat --dummy -n_threads 16 -distance 1 --pqueue -extra_edges 50"                     # optimal       
    ["Naïve_Euclidean"]="--filtered --stat --dummy -n_threads 16 -distance 0 --pqueue -extra_edges 50"             # bad euclidean
    ["No_Extra_Edges"]="--filtered --stat --dummy -n_threads 16 -distance 1 --pqueue"                              # No Extra Edges
    ["Accumulate_Unfiltered"]="--filtered --stat --dummy -n_threads 16 -distance 1 --pqueue --acc_unfiltered"      # Accumulate Unfiltered
    ["GreedySearch_using_Set"]="--filtered --stat --dummy -n_threads 16 -distance 1 -extra_edges 50"               # No PQueue
    ["Random_Start"]="--filtered --stat --dummy -n_threads 16 -distance 1 --random_start --pqueue -extra_edges 50" # Random Start
)


main_exe="./bin/main"

for name in "${!args_map[@]}"; do
    args="${args_map[$name]}"
    echo "$name $main_exe $args"
    $main_exe $args > "evaluations/filtered/${name}.txt"
done