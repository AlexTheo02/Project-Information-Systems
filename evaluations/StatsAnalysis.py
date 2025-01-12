from parser import gather_data
import os
from textwrap import wrap
from matplotlib.patches import Patch

colorA = '#1f77b4'
colorB = '#ff7f0e'
colorC = '#2ca02c'
colorD = '#d62728'

vamana_data = { filename[:-4] : gather_data('vamana/'+filename) for filename in os.listdir('vamana/')}
filtered_data = { filename[:-4] : gather_data('filtered/'+filename) for filename in os.listdir('filtered/')}
stitched_data = { filename[:-4] : gather_data('stitched/'+filename) for filename in os.listdir('stitched/')}

all_data = {
    'Vamana' : vamana_data,
    'Filtered' : filtered_data,
    'Stitched' : stitched_data,
}

import matplotlib.pyplot as plt
import numpy as np

def plot_time(data):

    fig = plt.figure(figsize=(28, 9), layout="constrained")  # Use layout="constrained"
    gs = fig.add_gridspec(1, 4, width_ratios=[1, 1, 1, 0.2])  # Add an extra column for the legend

    axes = []
    for col in range(3):
        ax = fig.add_subplot(gs[0, col])
        axes.append(ax)

    index_types = list(data.keys())

    max_value = max(
        [max([ max(data[index_type][optimization]['index_creation_time'], data[index_type][optimization]['querying_time'])
        for optimization in data[index_type]])
        for index_type in index_types]
    ).total_seconds() + 1

    for i,ax in enumerate(axes):
        index_type = index_types[i]

        groups = list(data[index_type].keys())

        # Empty Initialization
        time_labels = 0
        time_colors = 0
        time_data = 0

        # Data for Graph 1 (Time)   # FIX LABELS AND DATA UNDER CONDITION OF ONLY_FILTERED
        if index_type == 'Vamana':
            time_labels = ['Index Creation Time', 'Total Querying Time']
            time_colors = [colorA, colorB]
            time_data = [[data[index_type][key]['index_creation_time'].total_seconds(), 
                    data[index_type][key]['querying_time'].total_seconds()] for key in groups]
        else:
            time_labels = ['Index Creation Time', 'Total Querying Time', 'Filtred Querying Time', 'Unfiltered Querying Time']
            time_colors = [colorA, colorB, colorC, colorD]
            time_data = [[data[index_type][key]['index_creation_time'].total_seconds(), 
                        data[index_type][key]['querying_time'].total_seconds(),
                        data[index_type][key]['f_querying_time'].total_seconds(),                         # redundant if only_filtered
                        data[index_type][key]['unf_querying_time'].total_seconds()] for key in groups]    # redundant if only_filtered

        # Graph 1: Time
        x = np.arange(len(groups))  # Group positions
        bar_width = 0.2

        for i, (label, color) in enumerate(zip(time_labels, time_colors)):
            bar_positions = list(x + i * bar_width)
            bar_values = list([time_data[j][i] for j in range(len(groups))])
            bars = ax.bar(bar_positions, bar_values, bar_width, label=label, color=color)

            # Add values on top of the bars
            for bar in bars:
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width() / 2, height, f'{height:.2f}', ha='center', va='bottom', fontsize=8)

        ax.set_title(index_type, fontsize=16)
        ax.set_xticks(x + bar_width * (len(time_labels) - 1) / 2)
        max_width = 12
        group_labels = ['\n'.join(wrap(label.replace('_', ' '), max_width)) for label in groups]
        ax.set_xticklabels(group_labels)
        ax.set_ylabel('Time (s)', fontsize=12)
        ax.set_ylim(top = max_value)

    ax_legend_fake_subplot = fig.add_subplot(gs[0,3])
    handles = [
        Patch(color=colorA),
        Patch(color=colorB),
        Patch(color=colorC),
        Patch(color=colorD),
    ]
    labels = ['Index Creation Time', 'Total Querying Time', 'Filtred Querying Time', 'Unfiltered Querying Time']
    ax_legend_fake_subplot.legend(handles, labels, loc='upper right', fontsize=9)
    ax_legend_fake_subplot.axis('off')

    fig.suptitle('Time Chart for each Index Type', fontweight='semibold', fontsize = 22)
    print('Time Graph saved successfully!')
    plt.savefig('charts/TimeChart.svg', format = 'svg')



def plot_recall(data):

    fig = plt.figure(figsize=(28, 9), layout="constrained")  # Use layout="constrained"
    gs = fig.add_gridspec(1, 4, width_ratios=[1, 1, 1, 0.2])  # Add an extra column for the legend

    axes = []
    for col in range(3):
        ax = fig.add_subplot(gs[0, col])
        axes.append(ax)

    index_types = list(data.keys())

    for i,ax in enumerate(axes):
        index_type = index_types[i]

        groups = list(data[index_type].keys())

        # Empty Initialization
        recall_labels = 0
        recall_colors = 0

        if index_type == 'Vamana':
            recall_labels = ['Average Recall']
            recall_data = [[(data[index_type][key]['min_recall'], data[index_type][key]['avg_recall'], data[index_type][key]['max_recall'])] for key in groups]
        else:
            recall_labels = ['Average Recall', 'Average Filtered Recall', 'Average Unfiltered Recall']
            recall_data = [[
                (data[index_type][key]['min_recall'], data[index_type][key]['avg_recall'], data[index_type][key]['max_recall']),
                (data[index_type][key]['f_min_recall'], data[index_type][key]['f_avg_recall'], data[index_type][key]['f_max_recall']),
                (data[index_type][key]['unf_min_recall'], data[index_type][key]['unf_avg_recall'], data[index_type][key]['unf_max_recall'])
            ] for key in groups]

        x = np.arange(len(groups))  # Group positions
        bar_width = 0.2
    
        for i, label in enumerate(recall_labels):
            bar_positions = x + i * bar_width
            bar_values = [recall_data[j][i][1] for j in range(len(groups))]  # Avg recall values
            bars = ax.bar(bar_positions, bar_values, bar_width, label=label)

            # Add error bars for min and max recall
            for j, bar in enumerate(bars):
                avg = recall_data[j][i][1]
                min_val = recall_data[j][i][0]
                max_val = recall_data[j][i][2]
                ax.errorbar(
                    bar.get_x() + bar.get_width() / 2, avg, yerr=[[avg - min_val], [max_val - avg]], 
                    fmt='.',
                    color = '#333333',
                    ecolor='#333333',
                    alpha = 0.5,
                    capsize=5,
                    elinewidth=1,
                    linestyle = '--')

            # Add values on top of the bars
            for bar in bars:
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width() / 2, height + 0.005, f'{bar.get_height():.2f}', ha='center', va='bottom', fontsize=8)

        ax.set_title(index_type, fontsize=16)
        ax.set_xticks(x + bar_width * (len(recall_labels) - 1) / 2)
        max_width = 12
        group_labels = ['\n'.join(wrap(label.replace('_', ' '), max_width)) for label in groups]
        ax.set_xticklabels(group_labels)
        ax.set_ylabel('Recall Score', fontsize=12)

    ax_legend_fake_subplot = fig.add_subplot(gs[0,3])
    handles = [
        Patch(color=colorA),
        Patch(color=colorB),
        Patch(color=colorC),
    ]
    labels = ['Average Recall', 'Average Filtered Recall', 'Average Unfiltered Recall']
    ax_legend_fake_subplot.legend(handles, labels, loc='upper right', fontsize=9)
    ax_legend_fake_subplot.axis('off')

    fig.suptitle('Recall Chart for each Index Type', fontweight='semibold', fontsize = 22)
    print('Recall Graph saved successfully!')
    plt.savefig('charts/RecallChart.svg', format = 'svg')

# The following three graphs will contain 1 bar per group.
# Maybe do one general function and pass an argument each time to get the different data (as all data will be the same for all three index types)

def plot_QPS(data):

    fig = plt.figure(figsize=(28, 9), layout="constrained")  # Use layout="constrained"
    gs = fig.add_gridspec(1, 4, width_ratios=[1, 1, 1, 0.2])  # Add an extra column for the legend

    axes = []
    for col in range(3):
        ax = fig.add_subplot(gs[0, col])
        axes.append(ax)

    index_types = list(data.keys())

    max_value = max(
        [max(
            [data[index_type][optimization]['QPS']
        for optimization in data[index_type]] if index_type == 'Vamana' else
            [max([data[index_type][optimization]['QPS'], data[index_type][optimization]['f_QPS'], data[index_type][optimization]['unf_QPS']
                  ])
        for optimization in data[index_type]])
        for index_type in index_types]
    ) + 400

    for i,ax in enumerate(axes):
        index_type = index_types[i]

        groups = list(data[index_type].keys())

        # Empty Initialization
        qps_labels = 0
        qps_colors = 0
        qps_data = 0

        # Data for QPS
        if index_type == 'Vamana':
            qps_labels = ['Queries per Second']
            qps_data = [[data[index_type][key]['QPS']] for key in groups]
            qps_colors = [colorA]
        else:
            qps_labels = ['Queries per Second', 'Filtered Queries per Second', 'Unfiltered Queries per Second']
            qps_data = [[data[index_type][key]['QPS'], 
                        data[index_type][key]['f_QPS'],
                        data[index_type][key]['unf_QPS']] for key in groups]
        
            qps_colors = [colorA, colorB, colorC]

        # Graph 1: Time
        x = np.arange(len(groups))  # Group positions
        bar_width = 0.2


        for i, (label, color) in enumerate(zip(qps_labels, qps_colors)):
            bar_positions = list(x + i * bar_width)
            bar_values = list([qps_data[j][i] for j in range(len(groups))])
            bars = ax.bar(bar_positions, bar_values, bar_width, label=label, color=color)

            # Add values on top of the bars
            for bar in bars:
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width() / 2, height, f'{height:.0f}', ha='center', va='bottom', fontsize=8)

        ax.set_title(index_type, fontsize=16)
        ax.set_xticks(x + bar_width * (len(qps_labels) - 1) / 2)
        max_width = 12
        group_labels = ['\n'.join(wrap(label.replace('_', ' '), max_width)) for label in groups]
        ax.set_xticklabels(group_labels)
        ax.set_ylabel('Queries per Second', fontsize=12)
        ax.set_ylim(top = max_value)

    ax_legend_fake_subplot = fig.add_subplot(gs[0,3])
    handles = [
        Patch(color=colorA),
        Patch(color=colorB),
        Patch(color=colorC),
    ]
    labels = ['Queries per Second', 'Filtered Queries per Second', 'Unfiltered Queries per Second']
    ax_legend_fake_subplot.legend(handles, labels, loc='upper right', fontsize=9)
    ax_legend_fake_subplot.axis('off')

    fig.suptitle('Queries per Second Chart for each Index Type', fontweight='semibold', fontsize = 22)
    print('QPS Graph saved successfully!')
    plt.savefig('charts/QPSChart.svg', format = 'svg')


def plot_AvgTimePerQuery(data):
    fig = plt.figure(figsize=(28, 9), layout="constrained")  # Use layout="constrained"
    gs = fig.add_gridspec(1, 4, width_ratios=[1, 1, 1, 0.2])  # Add an extra column for the legend

    axes = []
    for col in range(3):
        ax = fig.add_subplot(gs[0, col])
        axes.append(ax)

    index_types = list(data.keys())

    max_value = max(
        [max(
            [data[index_type][optimization]['avg_query_time']
        for optimization in data[index_type]] if index_type == 'Vamana' else
            [max([data[index_type][optimization]['avg_query_time'], data[index_type][optimization]['f_avg_query_time'], data[index_type][optimization]['unf_avg_query_time']
                  ])
        for optimization in data[index_type]])
        for index_type in index_types]
    )*1000 + 0.12  # *1000 for s -> ms
    
    for i,ax in enumerate(axes):
        index_type = index_types[i]

        groups = list(data[index_type].keys())

        # Empty Initialization
        aqt_labels = 0
        aqt_colors = 0
        aqt_data = 0

        # Data for QPS
        if index_type == 'Vamana':
            aqt_labels = ['Average Time for Query']
            aqt_data = [[data[index_type][key]['avg_query_time']*1000] for key in groups]
            aqt_colors = [colorA]
        else:
            aqt_labels = ['Average Time for Query', 'Average Time for Filtered Query', 'Average Time for Unfiltered Query']
            aqt_data = [[data[index_type][key]['avg_query_time']*1000, 
                        data[index_type][key]['f_avg_query_time']*1000,
                        data[index_type][key]['unf_avg_query_time']*1000] for key in groups]    # * 1000 for milliseconds
        
            aqt_colors = [colorA, colorB, colorC]

        # Graph 1: Time
        x = np.arange(len(groups))  # Group positions
        bar_width = 0.2


        for i, (label, color) in enumerate(zip(aqt_labels, aqt_colors)):
            bar_positions = list(x + i * bar_width)
            bar_values = list([aqt_data[j][i] for j in range(len(groups))])
            bars = ax.bar(bar_positions, bar_values, bar_width, label=label, color=color)

            # Add values on top of the bars
            for bar in bars:
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width() / 2, height, f'{height:.2f}', ha='center', va='bottom', fontsize=8)

        ax.set_title(index_type, fontsize=16)
        ax.set_xticks(x + bar_width * (len(aqt_labels) - 1) / 2)
        max_width = 12
        group_labels = ['\n'.join(wrap(label.replace('_', ' '), max_width)) for label in groups]
        ax.set_xticklabels(group_labels)
        ax.set_ylabel('Average Time (ms) for Single Query', fontsize=12)
        ax.set_ylim(top = max_value)

    ax_legend_fake_subplot = fig.add_subplot(gs[0,3])
    handles = [
        Patch(color=colorA),
        Patch(color=colorB),
        Patch(color=colorC),
    ]
    labels = ['Average Time for Query', 'Average Time for Filtered Query', 'Average Time for Unfiltered Query']
    ax_legend_fake_subplot.legend(handles, labels, loc='upper right', fontsize=9)
    ax_legend_fake_subplot.axis('off')

    fig.suptitle('Average Time (ms) for Single Query Chart for each Index Type', fontweight='semibold', fontsize = 22)
    print('ATSQ Graph saved successfully!')
    plt.savefig('charts/ATSQChart.svg', format = 'svg')

def plot_AvgOutDegree(data):

    fig = plt.figure(figsize=(28, 9), layout="constrained")  # Use layout="constrained"
    gs = fig.add_gridspec(1, 4, width_ratios=[1, 1, 1, 0.2])  # Add an extra column for the legend

    axes = []
    for col in range(3):
        ax = fig.add_subplot(gs[0, col])
        axes.append(ax)

    index_types = list(data.keys())

    max_value = max(
        [max(
            [data[index_type][optimization]['n_edges'] / data[index_type][optimization]['n_points']
        for optimization in data[index_type]])
        for index_type in index_types]
    ) + 3

    for i,ax in enumerate(axes):
        index_type = index_types[i]

        groups = list(data[index_type].keys())

        # Initialization
        aod_labels = ['Average Out-Degree']
        aod_colors = [colorA]
        aod_data = [data[index_type][key]['n_edges'] / data[index_type][key]['n_points'] for key in groups]

        # Graph 1: Time
        x = np.arange(len(groups))  # Group positions
        bar_width = 0.2


        for i, (label, color) in enumerate(zip(aod_labels, aod_colors)):
            bar_positions = list(x + i * bar_width)
            bar_values = list([aod_data[j] for j in range(len(groups))])
            bars = ax.bar(bar_positions, bar_values, bar_width, label=label, color=color)

            # Add values on top of the bars
            for bar in bars:
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width() / 2, height, f'{height:.2f}', ha='center', va='bottom', fontsize=8)

        ax.set_title(index_type, fontsize=16)
        ax.set_xticks(x + bar_width * (len(aod_labels) - 1) / 2)
        max_width = 12
        group_labels = ['\n'.join(wrap(label.replace('_', ' '), max_width)) for label in groups]
        ax.set_xticklabels(group_labels)
        ax.set_ylabel('Average Out-Degree', fontsize=12)
        ax.set_ylim(top = max_value)

    ax_legend_fake_subplot = fig.add_subplot(gs[0,3])
    handles = [
        Patch(color=colorA),
    ]
    labels = ['Average Out-Degree']
    ax_legend_fake_subplot.legend(handles, labels, loc='upper right', fontsize=9)
    ax_legend_fake_subplot.axis('off')

    fig.suptitle('Average Out-Degree Chart for each Index Type', fontweight='semibold', fontsize = 22)
    print('AOD Graph saved successfully!')
    plt.savefig('charts/AODChart.svg', format = 'svg')


def visualize(data = all_data):
    
    plot_time(data)
    plot_recall(data)
    plot_QPS(data)
    plot_AvgTimePerQuery(data)
    plot_AvgOutDegree(data)


def main():

    # print(filtered_data[list(filtered_data.keys())[0]].keys())

    visualize()
if __name__ == "__main__":
    main()