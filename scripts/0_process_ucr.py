'''
This script copies the data files in dataset folder to the parent
folder and renames *_TRAIN to *_QUERY, *_TEST to *_DATA.
'''
from __future__ import print_function
import pandas as pd
import argparse
import os
import shutil

parser = argparse.ArgumentParser(description='Process UCR datasets')
parser.add_argument('ucr_path', help='Path to the UCR dataset folder')

args = parser.parse_args()

root = args.ucr_path

all_datasets = os.listdir(root)
for i, ds in enumerate(all_datasets):
        if ds.startswith('.'):
            continue
	train_path = os.path.join(root, ds, ds + '_TRAIN')
	query_path = os.path.join(root, ds + '_QUERY')

	test_path = os.path.join(root, ds, ds + '_TEST')
	data_path = os.path.join(root, ds + '_DATA')

	print('Copying %s_TRAIN to %s_QUERY' % (ds, ds))
	shutil.copyfile(train_path, query_path)
	print('Copying %s_TEST to %s_DATA' % (ds, ds))
	shutil.copyfile(test_path, data_path)
	print('%d/%d done' % (i + 1, len(all_datasets)))


