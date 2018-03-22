'''
This script iterates over each dataset in the UCR archive and extracts
its time series count, time series length and total number of subsequences,
then saves these information to a file. The output file of this script serves
as a bookkeeper for the experiment.

Copy the pygenex.so to the same folder as this script to run.
'''
from __future__ import print_function
import os
import pygenex as pg
import pandas as pd

from common import GROUPING_RECORDS, DATASET_ROOT

all_datasets = [n for n in os.listdir(DATASET_ROOT) if n.endswith('DATA')]

dataset_info = {}

for i, ds in enumerate(all_datasets):
	name = ds[:len(ds) - len('_DATA')]
	print('(%d/%d) Processing...%s' % (i, len(all_datasets), name))
	info = pg.loadDataset(name, os.path.join(DATASET_ROOT, ds), ',', -1, 1)
	dataset_info[name] = info

df = pd.DataFrame.from_dict(dataset_info, orient='index')
df['subsequence'] = (df['count'] * df['length'] * (df['length'] - 1) / 2).astype('int')

df.to_json(GROUPING_RECORDS, orient='index')
print('Preview the first few datasets')
print(df.head())
print('Saved info to', GROUPING_RECORDS)
