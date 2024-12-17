args_list=(
    # Maybe optimal for Filtered
    "--filtered --stat --dummy -n_threads 16 -distance 1 --random_start --pqueue -extra_edges 50"   
    # stripping from optimal:
    "--filtered --stat --dummy -n_threads 16 -distance 1 --random_start --pqueue"                    # bad recall for unfiltered queries
    "--filtered --stat --dummy -n_threads 16 -distance 1 --random_start --pqueue --acc_unfiltered"   # very good recall for unfiltered queries, much longer query_time for unfiltered queries.
    "--filtered --stat --dummy -n_threads 16 -distance 1 --random_start -extra_edges 50"             # no pqueue => more time, less recall
    "--filtered --stat --dummy -n_threads 16 -distance 1 --pqueue -extra_edges 50"                   # medoid start nodes => more time for tiny improvement to recall
    "--filtered --stat --dummy -n_threads 16 -distance 0 --random_start --pqueue -extra_edges 50"    # normal euclidean => MORE time
    "--filtered --stat --dummy -n_threads 16 -distance 2 --random_start --pqueue -extra_edges 50"    # parallel euclidean => MORE MORE time
    "--filtered --stat --dummy -n_threads 1 -distance 1 --random_start --pqueue -extra_edges 50"     # no threads => MORE time
)

main_exe="./bin/main"

for args in "${args_list[@]}"; do
    echo "$main_exe $args"
    $main_exe $args > "evaluations/filtered/${args}.txt"
done