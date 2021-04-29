from datetime import datetime
from pathlib import Path
import os
import os.path
import pandas as pd
import sys

separator = ','
labels = [['3h', '5h', '10h', '240h'], ['128nodes', '256nodes', '512nodes', '1024nodes']]
max_vars = [4,4]
num_simulations = 16
targets = ["init", "success", "timeout", "rentTimeout",
                           "unprovided", "cost", "rentTimeoutCost",
                           "compensation", "cpuTime", "cpuUsage"]
data = {}
df_times = pd.DataFrame()
for current_sim in range(num_simulations):
    for target in targets:
        subfile = f"../exp_cost/test_w1n128n1024s3s240e/" + \
                  f"dat/test_{target}_{current_sim}.dat"
        data[target] = []
        if os.path.isfile(subfile):
            rawdata = None
            with open(subfile, "r") as f:
                rawdata = f.readlines()
            for line in rawdata:
                aux = line.strip()
                if aux != "":
                    data[target].append(aux.split(separator))

    vars = []
    aux = current_sim
    for max_var in max_vars[::-1]:
        vars.append(aux % max_var)
        aux = aux // max_var
    vars = vars[::-1]
    labe_subtime = labels[0][vars[0]]
    labe_nodes = labels[1][vars[1]]

    data_time = data['success'] + data['timeout']
    serie_time = pd.Series(data=[float(d[1]) for d in data_time],
                           index=pd.Index(name='user', data=[int(d[0]) - 1 for d in data_time]), name='time',
                           dtype='float64')
    serie_init = pd.Series(data=[float(d[1]) for d in data['init']],
                           index=pd.Index(name='user', data=[int(d[0]) for d in data['init']]), dtype='float64')
    df_times_sim = pd.DataFrame({'init': serie_init, 'time': serie_time})
    df_times_sim['nodes'] = labe_nodes
    df_times_sim['subtime'] = labe_subtime
    df_times_sim = df_times_sim.set_index(['subtime', 'nodes'], append=True)

    if df_times.empty:
        df_times = df_times_sim
    else:
        df_times = df_times.append(df_times_sim)

df_times