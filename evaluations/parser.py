import csv
from datetime import timedelta

def gather_data(file_name):

    f_query_mode = False
    unf_query_mode = False
    n_points = 10000

    try:
        with open(file_name, "r") as file:
            data_lines = file.readlines()
    except FileNotFoundError:
        print(f"File '{file_name}' not found")

    data = {}
    data["n_points"] = n_points

    def get_value(line):
        return line.split(":")[1].strip()

    def parse_time(time_string):
        time_parts = time_string.split(":")
        hours = int(time_parts[0])
        minutes = int(time_parts[1])
        seconds_parts = time_parts[2].split(".")
        seconds = int(seconds_parts[0])
        milliseconds = int(seconds_parts[1].split("'")[0])
        microseconds = int(seconds_parts[1].split("'")[1])
        return timedelta(
            hours=hours, 
            minutes=minutes, 
            seconds=seconds, 
            milliseconds=milliseconds, 
            microseconds=microseconds
        )

    def parse_line(line):
        nonlocal f_query_mode
        nonlocal unf_query_mode

        split_line = line.split()

        if "--F_QUERIES--" in line:
            f_query_mode = True
            return
        
        if "--F_QUERIES_END--" in line:
            f_query_mode = False
            return

        if "--UNF_QUERIES--" in line:
            unf_query_mode = True
            return
        
        if "--UNF_QUERIES_END--" in line:
            unf_query_mode = False
            return

        if "Number of threads:" in line:
            data["n_threads"] = int(get_value(line))
            return
        
        if "Indexing Type:" in line:
            data["index_type"] = get_value(line)
            return
        
        if "k:" in split_line:
            data["k"] = int(get_value(line))
            return

        if "L:" in split_line:
            data["L"] = int(get_value(line))
            return

        if "R:" in split_line:
            data["R"] = int(get_value(line))
            return

        if "a:" in split_line:
            data["a"] = int(get_value(line))
            return
        
        if "Number of extra random edges:" in line:
            data["extra_edges"] = int(get_value(line))
            return
        
        if "Time to create the index:" in line:
            time_string = line.split(" ")[5].strip()
            data['index_creation_time'] = parse_time(time_string)
        
        if "Time to query the index (unfiltered):" in line:
            time_string = line.split(" ")[6].strip()
            data["unf_querying_time"] = parse_time(time_string) # timedelta object
            return
        
        if "Time to query the index (filtered):" in line:
            time_string = line.split(" ")[6].strip()
            data["f_querying_time"] = parse_time(time_string) # timedelta object
            return
        
        if "Average recall score:" in line:
            data["avg_recall"] = float(get_value(line))
            return

        if "Unfiltered" in split_line:
            data["unfiltered"] = True
            return
        
        if "Filtered" in split_line:
            data["filtered"] = True
            return
        
        if "Accumulate unfiltered" in line:
            data["acc_unfiltered"] = True
            return
        
        if "Using priority queue" in line:
            data["p_queue"] = True
            return
        
        if "Not using rgraph initialization" in line:
            data["no_rgraph"] = True
            return
        
        if "Number of edges:" in line:
            data["n_edges"] = int(get_value(line))
            return
        
        if f_query_mode and not unf_query_mode:
            data["f_queries_recalls"].append(float(line.strip()))
            return

        if unf_query_mode and not f_query_mode:
            data["unf_queries_recalls"].append(float(line.strip()))
            return
        

    # Initializations
    data["unf_queries_recalls"] = []
    data["f_queries_recalls"] = []

    data['filtered'] = False
    data['unfiltered'] = False

    # Read file and gather data
    for line in data_lines:
        parse_line(line)

    # Perform some data analysis to gather extra information

    # Recall analysis

    # Unfiltered
    min_unf_recall = float("inf")
    max_unf_recall = 0
    unf_recall_sum = 0
    for recall in data["unf_queries_recalls"]:
        min_unf_recall = min(recall,min_unf_recall)
        max_unf_recall = max(recall,max_unf_recall)
        unf_recall_sum += recall
    avg_unf_recall = unf_recall_sum / len(data["unf_queries_recalls"]) if len(data["unf_queries_recalls"]) else -1

    data["unf_min_recall"] = min_unf_recall
    data["unf_max_recall"] = max_unf_recall
    data["unf_avg_recall"] = avg_unf_recall

    if not data['unfiltered']:
        data['unf_querying_time'] = timedelta()
    
    # Filtered
    min_f_recall = float("inf")
    max_f_recall = 0
    f_recall_sum = 0
    for recall in data["f_queries_recalls"]:
        min_f_recall = min(recall,min_f_recall)
        max_f_recall = max(recall,max_f_recall)
        f_recall_sum += recall
    avg_f_recall = f_recall_sum / len(data["f_queries_recalls"]) if len(data["f_queries_recalls"]) else -1

    data["f_min_recall"] = min_f_recall
    data["f_max_recall"] = max_f_recall
    data["f_avg_recall"] = avg_f_recall

    if not data['filtered']:
        data['f_querying_time'] = timedelta()

    # Both (Filtered + Unfiltered)
    data["min_recall"] = min(min_f_recall, min_unf_recall)
    data["max_recall"] = max(max_f_recall, max_unf_recall)

    total_queries = len(data["f_queries_recalls"]) + len(data["unf_queries_recalls"])
    data["avg_recall"] = (f_recall_sum + unf_recall_sum) / total_queries if total_queries else -1

    # Timing analysis
    querying_time = data["f_querying_time"] + data["unf_querying_time"]
    data["querying_time"] = querying_time

    if data['filtered']:
        f_QPS = len(data["f_queries_recalls"]) / data["f_querying_time"].total_seconds()
        data["f_QPS"] = f_QPS
        f_avg_query_time = 1/f_QPS if f_QPS else -1
        data["f_avg_query_time"] = f_avg_query_time
    
    if data['unfiltered']:
        unf_QPS = len(data["unf_queries_recalls"]) / data["unf_querying_time"].total_seconds()
        data["unf_QPS"] = unf_QPS
        unf_avg_query_time = 1/unf_QPS if unf_QPS else -1
        data["unf_avg_query_time"] = unf_avg_query_time

    
    QPS = total_queries / querying_time.total_seconds()
    data["QPS"] = QPS

    avg_query_time = 1/QPS if QPS else -1

    data["avg_query_time"] = avg_query_time

    return data

# file_path = "evaluations/stitched/Accumulate_Unfiltered.txt"
# gd = gather_data(file_path)
# print(gd)