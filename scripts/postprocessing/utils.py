import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

HEADER_COLUMNS = ['k', 'h', 'h_extra', 'paa_block', 'ts_index',
                  'ts_start', 'ts_end', 'jaccard_ksim', 'wjaccard_ksim',
                  'jaccard_paa', 'wjaccard_paa', 'time_ksim', 'time_bf', 'time_paa']


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
    pd.concat([df_in, df_out], ignore_index=True)


def sort_and_clamp(s, limit):
    s = s.sort_index()
    last = s.iloc[-1]
    last_index = s.index[-1]
    if last_index < limit:
        s = s.append(pd.Series([last], index=[limit]))
    else:
        s = s[s.index <= limit]
    return s


def quick_view_metric(df, metric='wjaccard', title='', k=15, xlim=-1):
    examples = df[['ts_index', 'ts_start', 'ts_end']].drop_duplicates()
    samples = examples.sample(12)
    dfk = df[df.k == k]
    plt.figure(1, figsize=(12, 7))
    plt.suptitle(title, fontsize=16)
    
    ax_counter = 0
    for index, row in samples.iterrows():
        selector = ((dfk['ts_index'] == row['ts_index']) 
                    & (dfk['ts_start'] == row['ts_start'])
                    & (dfk['ts_end'] == row['ts_end']))
        current = dfk[selector][['h_extra', metric + '_ksim', metric + '_paa']].drop_duplicates()
        ax_counter += 1
        plt.subplot(3, 4, ax_counter)
        h = current['h_extra']

        metric_ksim = pd.Series(current[metric + '_ksim'].values, index=h)
        metric_paa = pd.Series(current[metric + '_paa'].values, index=h)

        metric_ksim = sort_and_clamp(metric_ksim, xlim)
        metric_paa = sort_and_clamp(metric_paa, xlim)
        
        metric_ksim.plot(title='Index: %d'%index)
        metric_paa.plot(title='Index: %d'%index)


def average_metric(df, metric, k):
    dfk = df[df.k == k]
    examples = dfk[['ts_index', 'ts_start', 'ts_end']].drop_duplicates()

    counter = 0
    # new_index = None
    # # Average the indices
    # for index, row in examples.iterrows():
    #     selector = ((dfk['ts_index'] == row['ts_index']) 
    #                 & (dfk['ts_start'] == row['ts_start'])
    #                 & (dfk['ts_end'] == row['ts_end']))
    #     current = dfk[selector][['h', 'h_extra']]

    #     # Use values in 'h' column to fill 'h_extra' until 'h_extra' has 100 elements
    #     h_col = current['h']        
    #     more = 100 - h_col.size
    #     step = h_col.iloc[1] - h_col.iloc[0]
    #     tail = np.arange(h_col.iloc[-1] + 2*step, h_col.iloc[-1] + step * (more + 2), step)
    #     h_extra_col = np.concatenate((current['h_extra'].values, tail))

    #     # Sum all 'h_extra' then take average
    #     if new_index is None:
    #         new_index = h_extra_col
    #     else:
    #         new_index += h_extra_col
    #     counter += 1  
    # new_index /= counter

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

        sum_max_metric += metric_series.max()
        sum_min_extent += metric_series.idxmax()
        counter += 1

    return (df_metric,
            float(sum_max_metric) / counter,
            float(sum_min_extent) / counter)


def report_accuracy(df, k, metric='wjaccard_dist', ylabel='L1', title='', xlim=5000, plot_paa=True):
    df_metric, avg_max_acc, avg_min_extent = average_metric(df, metric=metric, k=k)

    avg_metric = sort_and_clamp(df_metric.mean(axis=1), limit=xlim)
    std_metric = sort_and_clamp(df_metric.std(axis=1), limit=xlim)
    paa_metric_value = df[df.k == k][metric + '_paa'].mean()
    paa_metric = pd.Series(np.repeat(paa_metric_value, avg_metric.size), 
                             index=avg_metric.index)
    print title, 'k =', k
    print ' max accuracy: %f' % avg_max_acc
    print ' min extent: %d' % avg_min_extent
    print ' max PAA accuracy: %f' % paa_metric_value
    plt.grid(True)
    ax = avg_metric.plot(legend=True, label='k = %d (K-ONEX)' % k, linewidth=2)
    ax.set_title(title, fontsize=16)
    ax.set_xlabel("$k_e$", fontsize=15)
    ax.set_ylabel(ylabel, fontsize=15)
    if plot_paa:
        most_recent_color = ax.lines[-1].get_color()
        paa_metric.plot(title=title, label='k = %d (PAA)' % k,
                        color=most_recent_color, linestyle='--')
    ax.legend(fontsize=10, loc='upper right')


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
        met[index, 0] = metric(ksim_results[:, 1:], bf_results[:, 1:])
        met[index, 1] = metric(paa_results[:, 1:], bf_results[:, 1:])
    return met