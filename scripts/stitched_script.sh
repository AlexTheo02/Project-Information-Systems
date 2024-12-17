args_list=(
    "--stitched --stat --dummy -n_threads 16 -distance 1 --random_start --no_rgraph --pqueue -extra_edges 50"
    # stripping from optimal:
    "--stitched --stat --dummy -n_threads 16 -distance 1 --random_start --no_rgraph --pqueue"                    # bad recall for unfiltered queries
    "--stitched --stat --dummy -n_threads 16 -distance 1 --random_start --no_rgraph --pqueue --acc_unfiltered"   # very good recall for unfiltered queries, much longer query_time for unfiltered queries.
    "--stitched --stat --dummy -n_threads 16 -distance 1 --random_start --no_rgraph -extra_edges 50"             # no pqueue => more time, less recall
    "--stitched --stat --dummy -n_threads 16 -distance 1 --random_start --pqueue -extra_edges 50"                # initialize with Rgraph => more time (EFFECT ON RECALL?)
    "--stitched --stat --dummy -n_threads 16 -distance 1 --no_rgraph --pqueue -extra_edges 50"                   # medoid start nodes => more time for tiny improvement to recall
    "--stitched --stat --dummy -n_threads 16 -distance 0 --random_start --no_rgraph --pqueue -extra_edges 50"    # normal euclidean => MORE time
    "--stitched --stat --dummy -n_threads 16 -distance 2 --random_start --no_rgraph --pqueue -extra_edges 50"    # parallel euclidean => MORE MORE time
    "--stitched --stat --dummy -n_threads 1 -distance 1 --random_start --no_rgraph --pqueue -extra_edges 50"     # no threads => MORE time
)

main_exe="./bin/main"

for args in "${args_list[@]}"; do
    echo "$main_exe $args"
    $main_exe $args > "evaluations/stitched/${args}.txt"
done