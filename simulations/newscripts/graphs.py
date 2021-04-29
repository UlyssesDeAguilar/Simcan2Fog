#!/usr/bin/env python3.8
# *_* coding: utf-8 *_*


"""
module graphs - This module contains the code for all different graphs that can be used.

"""


from math import nan
from matplotlib import cm
import matplotlib.pyplot as plt
import matplotlib.ticker as plticker
import numpy as np
import os
import pandas as pd
import seaborn as sns


def user2d(data, graph_title, output_file, extensions=["eps"]):
    """
    Generate a context in which the current directory is other.

    Keyword arguments:
        path -- the new current working directory
    """
    plt.rcParams['patch.linewidth'] = 0
    plt.rcParams['patch.edgecolor'] = 'none'
    plt.rcParams["patch.force_edgecolor"] = False
    plt.rcParams.update({'font.size': 16})

    dfInit = pd.DataFrame(data["init"], columns=['user', 'arrival'])
    dfInit["user"] = pd.to_numeric(dfInit["user"],
                                   downcast="unsigned")
    dfInit["arrival"] = pd.to_numeric(dfInit["arrival"],
                                      downcast="float")

    ax = sns.histplot(data=dfInit, x="arrival", kde=True)
    ax.set_xlim(left=0)
    ax.set_ylabel('Number of users')
    ax.set_xlabel('Arrival time (h)')
    plt.title(graph_title)
    for ext in extensions:
        if ext[0] != '.':
            ext = '.' + ext
        plt.savefig(output_file + ext)
    plt.close()


def user_time(data, graph_title, output_file, extensions=["eps"]):
    """
    Generate a context in which the current directory is other.

    Keyword arguments:
        path -- the new current working directory
    """
    plt.rcParams['patch.linewidth'] = 0
    plt.rcParams['patch.edgecolor'] = 'none'
    plt.rcParams["patch.force_edgecolor"] = False
    plt.rcParams.update({'font.size': 16})

    dfInit = pd.DataFrame(data["init"], columns=['user', 'arrival'])
    dfInit["user"] = pd.to_numeric(dfInit["user"],
                                   downcast="unsigned")
    dfInit["arrival"] = pd.to_numeric(dfInit["arrival"],
                                      downcast="float")

    ax = dfInit.plot(x='user', y='arrival', kind='scatter')
    ax.set_xlim(left=0)
    ax.set_ylabel('Arrival time (h)')
    ax.set_xlabel('User ID')
    plt.title(graph_title)
    for ext in extensions:
        if ext[0] != '.':
            ext = '.' + ext
        plt.savefig(output_file + ext)
    plt.close()


def time2d(data, graph_title, output_file_bar, output_file_scatter,
           extensions_bar=["eps"], extensions_scatter=["eps"]):
    """
    Generate a context in which the current directory is other.

    Keyword arguments:
        path -- the new current working directory
    """
    plt.rcParams['patch.linewidth'] = 0
    plt.rcParams['patch.edgecolor'] = 'none'
    plt.rcParams["patch.force_edgecolor"] = False
    plt.rcParams.update({'font.size': 16})

    figs, axes = plt.subplots(ncols=2, gridspec_kw={'width_ratios': [5, 1]})

    figs.suptitle(graph_title, ha='right')

    with_cost = "rentTimeout" in data or "unprovided" in data

    dfSuccess = pd.DataFrame(data["success"],
                             columns=['user', 'subTime', 'type'])
    dfSuccess["user"] = pd.to_numeric(dfSuccess["user"],
                                      downcast="unsigned")
    dfSuccess["subTime"] = pd.to_numeric(dfSuccess["subTime"],
                                         downcast="float")
    dfSuccess["type"] = pd.to_numeric(dfSuccess["type"],
                                      downcast="integer")
    dfSuccess['service'] = 'Provided'

    if with_cost:
        if "rentTimeout" not in data:
            data["rentTimeout"] = []
        if "unprovided" not in data:
            data["unprovided"] = []
        dfRentTimeout = pd.DataFrame(data["rentTimeout"],
                                     columns=['user', 'subTime', 'type'])
        dfRentTimeout["user"] = pd.to_numeric(dfRentTimeout["user"],
                                              downcast="unsigned")
        dfRentTimeout["subTime"] = pd.to_numeric(dfRentTimeout["subTime"],
                                                 downcast="float")
        dfRentTimeout["type"] = pd.to_numeric(dfRentTimeout["type"],
                                              downcast="integer")
        dfUnprovided = pd.DataFrame(data["unprovided"],
                                    columns=['user', 'type', 'subTime'])
        dfRentTimeout['service'] = 'Provided'
        dfProvided = pd.concat([dfSuccess, dfRentTimeout])
    else:
        if "timeout" not in data:
            data["timeout"] = []
        dfUnprovided = pd.DataFrame(data["timeout"],
                                    columns=['user', 'type', 'subTime'])
        dfProvided = dfSuccess
    dfUnprovided["user"] = pd.to_numeric(dfUnprovided["user"],
                                         downcast="unsigned")
    dfUnprovided["subTime"] = pd.to_numeric(dfUnprovided["subTime"],
                                            downcast="float")
    dfUnprovided["type"] = pd.to_numeric(dfUnprovided["type"],
                                         downcast="integer")
    dfUnprovided['service'] = 'Unprovided'

    dfAll = pd.concat([dfProvided, dfUnprovided], sort=True)

    # print(dfAll)

    # sns.distplot(dfProvided[dfProvided['subTime'] > 0]['subTime'],
    #              ax=axes[0], kde=False)
    sns.histplot(data=dfProvided[dfProvided['subTime'] > 0], x="subTime",
                 ax=axes[0], kde=False, bins=20)
    axes[0].set_xlim(left=0)
    axes[0].set_ylabel('Number of users')
    axes[0].set_xlabel('Subscription time (h)')

    grouped = pd.DataFrame.groupby(dfAll.service, dfAll.type)
    data = [d for _, d in grouped]
    if with_cost:
        axes[1].hist(data, bins=3, histtype="barstacked",
                     label=['Regular', 'PrioritaryAsRegular', 'Prioritary'],
                     align='left')
    else:
        axes[1].hist(data, histtype="barstacked",
                     label='Regular',
                     align='left')
    axes[1].set_xticks([0, 0.65])
    axes[1].yaxis.tick_right()
    axes[1].xaxis.set_tick_params(rotation=90)
    axes[1].set_ylim(0, len(dfAll.index))
    axes[1].yaxis.set_label_position("right")
    axes[1].set_ylabel('Number of users')

    # ax = stackhist(dfAll.service, dfAll.type)
    plt.legend(bbox_to_anchor=(1, 1), loc='lower right')
    plt.tight_layout()

    for ext in extensions_bar:
        if ext[0] != '.':
            ext = '.' + ext
        plt.savefig(output_file_bar + ext)
    plt.close()
    markers = {"Provided": ".", "Unprovided": "X"}
    ax = sns.scatterplot(data=dfSuccess, x="user", y="subTime", hue="service",
                         linewidth=0, s=25, palette={"Provided": "black"},
                         legend=False, style="service", markers=markers)
    ax = sns.scatterplot(data=dfUnprovided, x="user", y="subTime",
                         hue="service", ax=ax, edgecolor="lightcoral",
                         linewidth=0.05, s=25, palette={"Unprovided": "red"},
                         legend=False, style="service", markers=markers)
    ax.set(xlabel='User ID', ylabel='Waiting time (h)')
    plt.title(graph_title)

    for ext in extensions_scatter:
        if ext[0] != '.':
            ext = '.' + ext
        plt.savefig(output_file_scatter + ext)
    plt.close()
    print(graph_title + ":", dfAll["subTime"].max())

def cpuUsage2d(data, graph_title, output_file, extensions=["eps"]):
    """
    Generate a context in which the current directory is other.

    Keyword arguments:
        path -- the new current working directory
    """
    plt.rcParams['patch.linewidth'] = 0
    plt.rcParams['patch.edgecolor'] = 'none'
    plt.rcParams["patch.force_edgecolor"] = False
    plt.rcParams.update({'font.size': 16})

    dfCpuTime = pd.DataFrame(data["cpuUsage"], columns=['dataCenter', 'simTime', 'percentage'])
    dfCpuTime["simTime"] = pd.to_numeric(dfCpuTime["simTime"],
                                   downcast="float") / 3600
    dfCpuTime["percentage"] = pd.to_numeric(dfCpuTime["percentage"],
                                      downcast="float")

    ax = dfCpuTime.plot(x='simTime', y='percentage', kind='line')
    #step = dfCpuTime["simTime"].max() // 10
    step = 200 // 10
    loc = plticker.MultipleLocator(base=step)  # ticks at regular intervals
    # loc = plticker.AutoLocator()
    ax.xaxis.set_major_locator(loc)
    ax.set_xlim(left=0)
    ax.set_ylabel('Cores in use (%)')
    ax.set_xlabel('Simulation time (h)')
    ax.legend().remove()
    plt.xlim(0,190)
    #plt.title(graph_title)
    for ext in extensions:
        if ext[0] != '.':
            ext = '.' + ext
        plt.savefig(output_file + ext, bbox_inches='tight')
    plt.close()

def cpuTime2d(data, graph_title, output_file, extensions=["eps"]):
    """
    Generate a context in which the current directory is other.

    Keyword arguments:
        path -- the new current working directory
    """
    plt.rcParams['patch.linewidth'] = 0
    plt.rcParams['patch.edgecolor'] = 'none'
    plt.rcParams["patch.force_edgecolor"] = False
    plt.rcParams.update({'font.size': 16})

    dfCpuTime = pd.DataFrame(data["cpuTime"], columns=['dataCenter', 'numCore', 'time', 'percentage'])
    dfCpuTime["numCore"] = pd.to_numeric(dfCpuTime["numCore"],
                                   downcast="unsigned")
    dfCpuTime["time"] = pd.to_numeric(dfCpuTime["time"],
                                      downcast="unsigned")
    dfCpuTime["percentage"] = pd.to_numeric(dfCpuTime["percentage"],
                                      downcast="float")

    ax = dfCpuTime.plot(x='numCore', y='percentage', kind='bar', width=1.0)
    step = dfCpuTime["numCore"].max() // 10
    loc = plticker.MultipleLocator(base=step)  # ticks at regular intervals
    # loc = plticker.AutoLocator()
    ax.xaxis.set_major_locator(loc)
    ax.set_xlim(left=0)
    ax.set_ylabel('Time usage (%)')
    ax.set_xlabel('Num of core')
    ax.legend().remove()
    #plt.title(graph_title)
    for ext in extensions:
        if ext[0] != '.':
            ext = '.' + ext
        plt.savefig(output_file + ext, bbox_inches='tight')
    plt.close()


def cost2d(df_3dcost, name, xlabels, ylabels, attribute, output_dir,
           prefix="", transpose=False, extensions=["eps"]):
    """
    Save cost2d graphs to specified output directory.

    Keyword arguments:
        path -- the new current working directory
    """
    df = df_3dcost
    tlab = xlabels
    glab = ylabels
    kind = "row"
    if not transpose:
        df = df.transpose()
        tlab, glab = glab, tlab
        kind = "column"
    for i in range(len(df.columns)):
        axes = df[df.columns[i]].plot.line()
        graph_title = prefix + f"{name} - {tlab[i]}"
        fig = axes.figure
        fig.suptitle(graph_title, ha='right')
        x_ticks = [glab[int(tick)] for tick in axes.get_xticks()[1:-1]]
        x_ticks = [""] + x_ticks + [""]
        axes.set_xticks(axes.get_xticks().tolist())
        axes.set_xticklabels(x_ticks)
        for ext in extensions:
            if ext[0] == '.':
                ext = ext[1:]
            fig.savefig(output_dir + os.path.sep +
                        f"output_{attribute}_{kind}_{i}.{ext}")
        plt.close()


def cost3d(df_table, output_file, graph_title,  min_key=nan, max_key=nan,
           extensions=["eps"]):
    """
    Save cost3d chart to specified output file.

    Keyword arguments:
    path -- the new current working directory
    """
    data = {}
    for index, df_row in df_table.iterrows():
        row, col = df_row["Sim"].split('-')
        value = df_row["TotalCost"]
        if row not in data:
            data[row] = {}
        data[row][col] = value
    x_data = np.array(list(data.keys()))
    y_data = np.array(list(data[x_data[0]].keys()))
    x_mesh, y_mesh = np.meshgrid([i for i in range(x_data.size)],
                                 [i for i in range(y_data.size)])
    z_mesh = np.zeros(x_mesh.shape)
    for i in range(x_mesh.shape[0]):
        for j in range(x_mesh.shape[1]):
            z_mesh[i, j] = data[x_data[x_mesh[i, j]]][y_data[y_mesh[i, j]]]
    fig = plt.figure()
    fig.suptitle(graph_title)
    ax = fig.gca(projection='3d')
    ax.plot_surface(x_mesh,
                    y_mesh,
                    z_mesh,
                    cmap=cm.coolwarm, linewidth=0, antialiased=False)
    ax.set_xlim(0, x_data.size - 1)
    x_ticks = [x_data[int(tick)] for tick in ax.get_xticks()[1:-1]]
    x_ticks = [""] + x_ticks + [""]
    ax.set_xticks(ax.get_xticks().tolist())
    ax.set_xticklabels(x_ticks)
    ax.set_ylim(0, y_data.size - 1)
    y_ticks = [y_data[int(tick)] for tick in ax.get_yticks()[1:-1]]
    y_ticks = [""] + y_ticks + [""]
    ax.set_yticks(ax.get_yticks().tolist())
    ax.set_yticklabels(y_ticks)
    if not np.isnan(min_key):
        ax.set_zlim(bottom=min_key)
    if not np.isnan(max_key):
        ax.set_zlim(top=max_key)
    for ext in extensions:
        if ext[0] != '.':
            ext = '.' + ext
        fig.savefig(output_file + ext)


def provided2d(df_table, current_row, x_coord, with_cost, output_file,
               extensions=["eps"]):
    """
    Save provided2d chart to specified output file.

    Keyword arguments:
    path -- the new current working directory
    """
    plt.rcParams["figure.figsize"] = (10, 5)
    plt.rcParams.update({'font.size': 20})

    fig, ax = plt.subplots()
    # plt.title(args.title.split('p')[0]+'% of users are prioritary ')

    c_x = current_row * x_coord
    max_nusers = 0
    # x_labels = [s.split('r')[0]+'%'
    #             for s in df_table['Sim'][c_x:c_x + x_coord]]
    x_labels = df_table['Sim'][c_x:c_x + x_coord]
    if with_cost:
        palette = ["#446DB1", "#DBC256", "#56DB7F", "#DD8452", "#DB5E56"]
        max_nusers = max(df_table['RProvi'][c_x:c_x + x_coord] +
                         df_table['PRProvi'][c_x:c_x + x_coord] +
                         df_table['PProvi'][c_x:c_x + x_coord] +
                         df_table['RegUnpro'][c_x:c_x + x_coord] +
                         df_table['PriUnpro'][c_x:c_x + x_coord])
        ax.stackplot(x_labels,
                     df_table['RProvi'][c_x:c_x + x_coord],
                     df_table['PRProvi'][c_x:c_x + x_coord],
                     df_table['PProvi'][c_x:c_x + x_coord],
                     df_table['RegUnpro'][c_x:c_x + x_coord],
                     df_table['PriUnpro'][c_x:c_x + x_coord],
                     labels=['Provided Regular',
                             'Provided Prioritary as Regular',
                             'Provided Prioritary',
                             'Unprovided Regular',
                             'Unprovided Prioritary'],
                     colors=palette)
    else:
        palette = ["#446DB1", "#DD8452"]
        max_nusers = max(df_table['NUsers'][c_x:c_x + x_coord])
        ax.stackplot(x_labels,
                     df_table['NUsers'][c_x:c_x + x_coord]
                     - df_table['TotalUnpro'][c_x:c_x + x_coord],
                     df_table['TotalUnpro'][c_x:c_x + x_coord],
                     labels=['Provided User',
                             'Unprovided User'],
                     colors=palette)
    ax.set_ylim(0, max_nusers + max_nusers // 10 + 1)
    fig.autofmt_xdate()
    # ax.locator_params(axis='x', nbins=15)

    step = x_coord // 7  # alligned vlines and labels
    step = x_coord / 10  # misalligned vlines and labels
    loc = plticker.MultipleLocator(base=step)  # ticks at regular intervals
    # loc = plticker.AutoLocator()
    ax.xaxis.set_major_locator(loc)
    ax.set_ylabel('Number of users')
    # plt.xlabel('Reserved nodes')
    ax.legend(loc='lower right')
    ax.vlines(np.arange(0, x_coord, step), 0, max_nusers, linestyles='dotted')
    # plt.hlines(range(0, 10000, 200), 0, x - 1, linestyles='dotted')
    for ext in extensions:
        if ext[0] != '.':
            ext = '.' + ext
        fig.savefig(output_file + ext)

def catplot2d(df_table, output_file, extensions=["png"]):
    g = sns.catplot(x="nodes", y="time", hue="subtime", data=df_table, kind="violin")

    for ext in extensions:
        if ext[0] != '.':
            ext = '.' + ext
        g.savefig(output_file + ext)

def catplot2d2(df_table, current_row, x_coord,graph_title, output_file, extensions=["png"]):
    g = sns.catplot(x="nodes", y="time", hue=None, data=df_table.reset_index(), kind="violin")

    g.ax.set_title(graph_title)
    g.set_xticklabels(rotation=30, ha='right')

    for ext in extensions:
        if ext[0] != '.':
            ext = '.' + ext
        g.savefig(output_file + ext, bbox_inches='tight')


def catplotp2d(df_table, current_row, x_coord,graph_title, output_file, extensions=["png"], column=False):
    g = sns.catplot(x="nodes", y=column, hue=None, data=df_table.reset_index(), s=1, jitter="0.4")
    # g.set(ylim=(0, 11))
    # g.ax.set_title(graph_title)
    g.set_xticklabels(rotation=30, ha='right')
    g.set_xlabels("Number of nodes")
    g.set_ylabels("Waiting time (h)")

    for ext in extensions:
        if ext[0] != '.':
            ext = '.' + ext
        g.savefig(output_file + ext, bbox_inches='tight')

def boxplot(df_table, current_row, x_coord,graph_title, output_file, extensions=["png"]):
    fig, ax = plt.subplots()
    b = sns.boxplot(x="nodes", y="time", hue=None, data=df_table.reset_index(), ax=ax)
    ax.set_title(graph_title)

    for ext in extensions:
        if ext[0] != '.':
            ext = '.' + ext
        fig.savefig(output_file + ext, bbox_inches='tight')