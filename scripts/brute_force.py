'''
This script ...
'''
from __future__ import print_function
import argparse
import json
import logging
import numpy as np
import os
import pygenex as pg
import pandas as pd
import pprint
import time
from datetime import datetime

from common import *

def generate_random_queries(n, count, length):
	queries = None
	while queries is None or queries.shape[0] < n:
		indices = np.random.randint(count, size=n)
		subindices = np.random.randint(length, size=(n, 2))
		new = np.hstack((indices[:, np.newaxis],
						subindices.min(axis=1, keepdims=True),
						subindices.max(axis=1, keepdims=True)))
		new = new[(new[:, 2] - new[:, 1]) > 1]
		if queries is None:
			queries = new
		else:
			queries = np.vstack((queries, new))
	return queries[:n]


def get_queries(name, nquery, incount, inlength, outcount, outlength):
	query_path = os.path.join(EXPERIMENT_ROOT, name + '_query.csv')
	if os.path.exists(query_path):
		logging.info('Found query find for %s', name)
		return pd.read_csv(query_path)

	nin = int(nquery / 2)
	nout = nquery - nin

	inquery = generate_random_queries(nin, incount, inlength)
	outquery = generate_random_queries(nout, outcount, outlength)
	inquery = np.hstack((inquery, np.tile(0, (nin, 1))))
	outquery = np.hstack((outquery, np.tile(1, (nin, 1))))
	df = pd.DataFrame(np.vstack((inquery, outquery)),
					  columns=['index', 'start', 'end', 'outside'])
	df.to_csv(query_path, index=False)
	logging.info('Generated %d queries for %s', df.shape[0], name)
	return df


def run_brute_force(name, dist, k, queries_df, dry_run=False):
	name_out = name + '_out'
	dataset_path = os.path.join(DATASET_ROOT, name + '_DATA')
	query_path   = os.path.join(DATASET_ROOT, name + '_QUERY')
	info = pg.loadDataset(name, dataset_path, ',', -1, 1)
	logging.info('Loaded dataset %s. Count = %d. Length = %d', 
				 name, info['count'], info['length'])
	
	info = pg.loadDataset(name_out, query_path, ',', -1, 1)
	logging.info('Loaded dataset %s. Count = %d. Length = %d', 
				 name_out, info['count'], info['length'])
	
	pg.normalize(name)
	logging.info('Normalized the dataset %s.', name)

	pg.normalize(name_out)
	logging.info('Normalized the dataset %s.', name_out)

	experiment_path = os.path.join(EXPERIMENT_ROOT, name + '.json')
	if os.path.exists(experiment_path):
		logging.info('Result file for %s exists', name)
		with open(experiment_path, 'r') as f:
			results = json.load(f)
	else:
		results = {}
	# result structure
	# {
	#	'euclidean': [{ query: [index, start, end, outside]
	#					result_bf: [{'data': {'index': ..., 'end': .., 'start': ...}, 'dist': ...}, ...]
	#					time_bf: ....},
	#				  ...]
	#   'manhattan': ...
	# }
	#
	for d in dist:
		if d not in results:
			results[d] = []
		for i in range(queries_df.shape[0]):
			query = { 'index': queries_df['index'][i],
					  'start': queries_df.start[i],
					  'end': queries_df.end[i],
					  'outside': queries_df.outside[i]
					}

			find_query = filter(lambda o: o['query'] == query, results[d])
			if len(find_query) == 0 or 'result_bf' not in find_query[0]:
				logging.info('Running BF[%d, %d, %d, %d, %d, %s]...(%d/%d)',
							 k, query['index'], query['start'], query['end'], query['outside'],
							 d, i, queries_df.shape[0])
				if not dry_run:
					start = time.time()
					if query['outside'] == 0: # is inside
						result_bf = pg.ksimbf(k, name, name, query['index'], query['start'], query['end'], d)
					else:
						result_bf = pg.ksimbf(k, name, name_out, query['index'], query['start'], query['end'], d)
					end = time.time()
					time_bf = end - start

					results[d].append({
						'query': query,
						'result_bf': result_bf,
						'time_bf': time_bf
					})
					with open(experiment_path, 'w') as f:
						json.dump(results, f)
					logging.info('Finished BF[%d, %d, %d, %d, %d, %s] after %.1f seconds', 
								 k, query['index'], query['start'], query['end'], query['outside'], d, end - start)
			else:
				logging.info('Query BF[%d, %d, %d, %d, %d, %s] is already run',
							 k, query['index'], query['start'], query['end'], query['outside'], d)

	pg.unloadDataset(name)
	logging.info('Unloaded %s', name)

	pg.unloadDataset(name_out)
	logging.info('Unloaded %s', name_out)


if __name__=='__main__':
	logging.basicConfig(format='%(asctime)s [%(levelname)s] %(name)s: %(message)s', 
						level=logging.INFO)
	parser = argparse.ArgumentParser('Generate random queries and run brute\
									 force method over them')
	parser.add_argument('--dist', nargs='+', default=['euclidean'],
						help='List of distances to group with')
	parser.add_argument('--k', type=int, default=15, help='Number of best matches to find')
	parser.add_argument('--start-over', action='store_true', help='Start from the beginning')
	parser.add_argument('--nquery', type=int, default=100, help='Number of random queries to generate')
	parser.add_argument('--dry-run', action='store_true', help='Only print the datasets and params to group')
	parser.add_argument('--email-addr', default=None, help='Email to send notification when finishing grouping')
	parser.add_argument('--subseq-count-min', type=int, default=-1,
						help='Group datasets with total subsequences larger than this.'
							 'Set a negative number to group all. Default: -1')
	parser.add_argument('--subseq-count-max', type=int, default=-1,
						help='Group datasets with total subsequences smaller than this.'
							 'Set a negative number to group all. Default: -1')

	args = parser.parse_args()
	logging.info('Args: %s', pprint.pformat(args))

	with open(GROUPING_RECORDS, 'r') as f:
		ds_info = json.load(f)

	try:
		subseq_max = args.subseq_count_max
		subseq_min = args.subseq_count_min
		order = sorted([(ds_info[ds]['subsequence'], ds) 
						for ds in ds_info if not ds.endswith('_out')])
		order = zip(*order)[1]
		for ds in order:
			subseq = ds_info[ds]['subsequence']
			if (subseq_max < 0 and subseq_min < 0) or\
			   (subseq_min <= subseq <= subseq_max) or\
			   (subseq_min < 0 and subseq_max >= 0 and subseq <= subseq_max) or\
			   (subseq_max < 0 and subseq_min >= 0 and subseq >= subseq_min):
				name = ds.encode('ascii', 'ignore')
				logging.info('%s. Number of subsequences %d', name, subseq)

				# Generate queries or load existing ones
				queries = get_queries(name
									 , args.nquery
									 , ds_info[ds]['count']
									 , ds_info[ds]['length']
									 , ds_info[ds + '_out']['count']
									 , ds_info[ds + '_out']['length'])

				# Run the experiment on the current dataset
				run_brute_force(name
								, args.dist
							, args.k
								, queries
								, args.dry_run)

	except Exception as e:
		content = 'Brute force stopped - ' + repr(e)
		logging.error(content)
		if not args.dry_run:
			send_notification(args.email_addr, 'Error occured. Brute force stopped', content)
		
