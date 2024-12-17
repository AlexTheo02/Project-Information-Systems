args_list=(
    # Maybe optimal for Vamana
    "--vamana --stat --dummy --crop_filters -n_threads 16 -distance 1 --no_rgraph --pqueue -extra_edges 15"   
    # stripping from optimal:
    "--vamana --stat --dummy --crop_filters -n_threads 16 -distance 1 --no_rgraph --pqueue"                   # no extra edges => worse recall
    "--vamana --stat --dummy --crop_filters -n_threads 16 -distance 1 --no_rgraph -extra_edges 15"            # no pqueue => more time, less recall
    "--vamana --stat --dummy --crop_filters -n_threads 16 -distance 1 --pqueue -extra_edges 15"               # initialize with Rgraph => more time (EFFECT ON RECALL?)
    "--vamana --stat --dummy --crop_filters -n_threads 16 -distance 1 --random_start --no_rgraph --pqueue -extra_edges 15"                  # random start nodes
    "--vamana --stat --dummy --crop_filters -n_threads 16 -distance 0 --no_rgraph --pqueue -extra_edges 15"   # normal euclidean => MORE time
    "--vamana --stat --dummy --crop_filters -n_threads 16 -distance 2 --no_rgraph --pqueue -extra_edges 15"   # parallel euclidean => MORE MORE time
    "--vamana --stat --dummy --crop_filters -n_threads 1 -distance 1 --no_rgraph --pqueue -extra_edges 15"    # no threads => MORE time
)

main_exe="./bin/main"

for args in "${args_list[@]}"; do
    echo "$main_exe $args"
    $main_exe $args > "evaluations/vamana/${args}.txt"
done