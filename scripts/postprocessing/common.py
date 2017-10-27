import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

HEADER_COLUMNS = ['k', 'h', 'h_extra', 'paa_block', 'ts_index',
                  'ts_start', 'ts_end', 'jaccard_ksim', 'wjaccard_ksim',
                  'jaccard_paa', 'wjaccard_paa', 'time_ksim', 'time_bf', 'time_paa']

all_dataset_metadata = [
    {
        'file_name': 'ItalyPowerDemand',
        'name': 'ItalyPowerDemand',
        'size': 30084,
        'xlim': 1700
    },
    {
        'file_name': 'ECG200',
        'name': 'ECG',
        'size': 456000,
        'xlim': 2500
    },
    {
        'file_name': 'synthetic_control',
        'name': 'synthetic_control',
        'size': 531000,
        'xlim': 2000
    },
    {
        'file_name': 'Gun_Point',
        'name': 'Gun-Point',
        'size': 1676250,
        'xlim': 6000
    },
    {
        'file_name': 'MedicalImages',
        'name': 'MedicalImages',
        'size': 3686760,
        'xlim': 40000
    },
    {
        'file_name': 'FaceAll',
        'name': 'Face (All)',
        'size': 14390350,
        'xlim': 12500
    },
    {
        'file_name': '50words',
        'name': '50Words',
        'size': 16523325,
        'xlim': 22500
    },
    {
        'file_name': 'wafer',
        'name': 'Wafer',
        'size': 70852824,
        'xlim': 200000
    },
]

sorted_name_by_size = pd.DataFrame(all_dataset_metadata).sort_values(by='size')['file_name'].values

def read_results_file(result_path):
    # Get number of columns
    num_cols = 0
    with open(result_path, 'r') as f:
        for line in f:
            num_cols = max(num_cols, line.count(',') + 1)

    # Generate column names
    names = HEADER_COLUMNS + range(num_cols - len(HEADER_COLUMNS))

    # Read data
    df = pd.read_csv(result_path, header=None, names=names, index_col=False)
    # df = df[~df.isin([np.inf, -np.inf]).any(1)]
    # df.index = pd.RangeIndex(len(df.index))
    return df


def combine_in_out(df_in, df_out):
    return pd.concat([df_in, df_out], ignore_index=True)


def sort_and_clamp(s, limit):
    s = s.sort_index()
    last = s.iloc[-1]
    last_index = s.index[-1]
    if last_index < limit:
        s = s.append(pd.Series([last], index=[limit]))
    else:
        s = s[s.index <= limit]
    return s


def average_metric(df, metric, k, larger_is_better=True):
    dfk = df[df.k == k]
    examples = dfk[['ts_index', 'ts_start', 'ts_end']].drop_duplicates()

    counter = 0

    df_metric = None
    sum_max_metric = 0
    sum_min_extent = 0
    sum_min_real_extent = 0
    metric_col = metric + '_ksim'    
    for index, row in examples.iterrows():
        selector = ((dfk['ts_index'] == row['ts_index']) 
                    & (dfk['ts_start'] == row['ts_start'])
                    & (dfk['ts_end'] == row['ts_end']))

        metric_index_values = dfk[selector][['h', metric_col]].drop_duplicates()
        # Create index 
        metric_index = metric_index_values['h'].values
        
        last_metric_index = metric_index[-1] 

        step = metric_index[1] - metric_index[0]
        more = 100 - metric_index.shape[0]
        tail = np.arange(last_metric_index + 2*step, last_metric_index + step * (more + 2), step)
        metric_index = np.concatenate((metric_index, tail))

        # Repeat last value to reach the size of index
        metric_values = metric_index_values[metric_col].values

        last_metric_value = metric_values[-1]
        repeat = len(metric_index) - len(metric_values)
        metric_values = np.concatenate((metric_values, [last_metric_value] * repeat))
        metric_series = pd.Series(metric_values, index=metric_index)

        if df_metric is None:
            df_metric = metric_series.to_frame(name=index)
        else:
            df_metric[index] = metric_series

        if larger_is_better:
            sum_max_metric += metric_series.max()
            sum_min_extent += metric_series.idxmax()
        else:
            sum_max_metric += metric_series.min()
            sum_min_extent += metric_series.idxmin()

        counter += 1

    return (df_metric,
            float(sum_max_metric) / counter,
            float(sum_min_extent) / counter)


def compute_metric(df, metric):
    '''
    Computes similarity between results of kSim and brute-force, and between
    PAA and brute-force, using a the given metric function
    '''
    met = np.zeros((df.shape[0], 2))
    for index, row in df.iterrows():
        k = int(row['k'])
        results = row.drop(HEADER_COLUMNS).values
        ksim_results = np.zeros((k, 4))
        bf_results = np.zeros_like(ksim_results)
        paa_results = np.zeros_like(ksim_results)
        for i in range(k):
            ksim_results[i] = results[4*i:4*i+4]
            bf_results[i] = results[4*(k+i):4*(k+i)+4]
            paa_results[i] = results[4*(2*k+i):4*(2*k+i)+4]
        met[index, 0] = metric(ksim_results[:, 0:], bf_results[:, 0:])
        met[index, 1] = metric(paa_results[:, 0:], bf_results[:, 0:])
    return met