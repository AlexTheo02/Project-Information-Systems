import matplotlib.pyplot as plt
import numpy as np
import pickle

IndexCreation = [
    'SET_VAMANA_greedySearchIndexStats.csv',
    'PQ_VAMANA_greedySearchIndexStats.csv',
    'SET_FILTERED_greedySearchIndexStats.csv',
    'PQ_FILTERED_greedySearchIndexStats.csv',
    'SET_STITCHED_greedySearchIndexStats.csv',
    'PQ_STITCHED_greedySearchIndexStats.csv'
]

Querying = [
    'SET_VAMANA_greedySearchQueryStats.csv',
    'PQ_VAMANA_greedySearchQueryStats.csv',
    'SET_FILTERED_greedySearchQueryStats.csv',
    'PQ_FILTERED_greedySearchQueryStats.csv',
    'SET_STITCHED_greedySearchQueryStats.csv',
    'PQ_STITCHED_greedySearchQueryStats.csv'
]

stackedIndex = {}
stackedQuery = {}

colorA = '#1f77b4'
colorB = '#ff7f0e'

def stackGSC(filename):
    data = np.loadtxt(filename, delimiter = ',', skiprows = 1, dtype = np.float32)

    # extracting columns

    unique_gsc = range(int(data[-1, 0]))
    costs = []
    maxlen = 0
    # creating masks
    for gsc in unique_gsc:
        current_gs = data[data[:, 0] == np.float32(gsc)]
        cost = current_gs[:, 1]
        if len(cost) > maxlen:
            maxlen = len(cost)
        costs.append(cost)

    padded_costs = []
    for cost in costs:
        padding = maxlen - len(cost)
        padded_cost = np.pad(cost, (0,padding), mode = 'constant', constant_values = np.nan)    # pad with nan
        padded_costs.append(padded_cost)
    
    stacked_costs = np.vstack(padded_costs)

    averaged_costs = np.nanmean(stacked_costs, axis = 0)    # average column-wise

    return averaged_costs

def generateStackedDictionaries():

    global stackedIndex
    global stackedQuery

    stackedIndex = {
        'Vamana' : [stackGSC(IndexCreation[0]), stackGSC(IndexCreation[1])],    # set, pqueue
        'Filtered' : [stackGSC(IndexCreation[2]), stackGSC(IndexCreation[3])],
        'Stitched' : [stackGSC(IndexCreation[4]), stackGSC(IndexCreation[5])]
    }

    stackedQuery = {
        'Vamana' : [stackGSC(Querying[0]), stackGSC(Querying[1])],    # set, pqueue
        'Filtered' : [stackGSC(Querying[2]), stackGSC(Querying[3])],
        'Stitched' : [stackGSC(Querying[4]), stackGSC(Querying[5])]
    }

    with open('stackedIndex.pkl', 'wb') as file:
        pickle.dump(stackedIndex, file)

    with open('stackedQuery.pkl', 'wb') as file:
        pickle.dump(stackedQuery, file)

def loadStackedDictionaries():
    global stackedIndex
    global stackedQuery

    with open('stackedIndex.pkl', 'rb') as file:
        stackedIndex = pickle.load(file)

    with open('stackedQuery.pkl', 'rb') as file:
        stackedQuery = pickle.load(file)

def visualize():
    fig = plt.figure(figsize=(15.5, 8), layout="constrained")  # Use layout="constrained"
    gs = fig.add_gridspec(2, 4, width_ratios=[1, 1, 1, 0.3])  # Add an extra column for the legend

    axes = []
    for row in range(2):
        for col in range(3):
            ax = fig.add_subplot(gs[row, col])
            axes.append(ax)

    keys = list(stackedIndex.keys()) + list(stackedIndex.keys())

    for i, ax in enumerate(axes):
        key = keys[i]

        set_values = stackedIndex[key][0] if i < 3 else stackedQuery[key][0]
        pq_values = stackedIndex[key][1] if i < 3 else stackedQuery[key][1]

        set_avg = np.mean(set_values)
        pq_avg = np.mean(pq_values)

        set_t = range(len(set_values))
        pq_t = range(len(pq_values))

        ax.plot(set_t, set_values, color=colorA)
        ax.plot(pq_t, pq_values, color=colorB)

        ax.axhline(y=set_avg, color=colorA, linestyle='--', linewidth=1)
        ax.axhline(y=pq_avg, color=colorB, linestyle='--', linewidth=1)

        ax.set_title(f"""{key} - {'Index Creation' if i < 3 else 'Querying'}""")
        ax.set_xlabel('Timestep')
        ax.set_ylabel('Operations Cost per Timestep')

    # Create the legend in the extra subplot space
    ax_legend = fig.add_subplot(gs[:, 3])  # Span all rows in the last column
    handles = [plt.Line2D([0], [0], color=colorA), plt.Line2D([0], [0], color=colorB),
               plt.Line2D([0], [0], color=colorA, linestyle='--'), plt.Line2D([0], [0], color=colorB, linestyle='--')]
    labels = ['Unordered Set', 'Priority Queue', 'Unordered Set (Average)', 'Priority Queue (Average)']

    ax_legend.legend(handles, labels, loc='upper right', fontsize=9)

    # Hide the axes of the legend subplot
    ax_legend.axis('off')

    fig.suptitle('Optimization of GreedySearch: Priority Queue Vs Unordered Set', fontsize=18, fontweight='bold')
    # plt.tight_layout()

    plt.savefig('../charts/PQueueOptimizationPlots.svg', format='svg')
    print('Figure stored successfully.')

def main():
    
    global stackedIndex
    global stackedQuery

    # generateStackedDictionaries()
    loadStackedDictionaries()
    
    visualize()

if __name__ == "__main__":
    main()