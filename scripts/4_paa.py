'''
This script runs the piecewise aggregatio approximation method on all datasets
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

def run_paa(name, dist, k, queries_df, dry_run=False):
	'''Run PAA experiment

	For each distance, this method iterates over the queries. It checks if
	the current query was already performed and recorded to the json result file.
	If it was not, it run the PAA method with that query

	JSON result structure
	{
		'euclidean': [{ query: [index, start, end, outside]
						result_paa: [{'data': {'index': ..., 'end': .., 'start': ...}, 'dist': ...}, ...]
						time_paa: ....},
					  ...]
	  'manhattan': ...
	}
	'''
	name, name_out = load_and_normalize(name)

	pg.preparePAA(name, 3)
	logging.info('Generate PAA of block size 3 for dataset %s.', name)

	results, experiment_path = get_results_object(name)

	for d in dist:
		if d not in results:
			results[d] = []

		for i in range(queries_df.shape[0]):
			query = { 
				'index': queries_df['index'][i],
				'start': queries_df.start[i],
				'end': queries_df.end[i],
				'outside': queries_df.outside[i]
			}

			find_query = filter(lambda o: 'result_paa' in o and o['query'] == query,
												  results[d])
			if len(find_query) == 0:
				logging.info('Running %s %s...(%d/%d)',
							 name, query_description('PAA', k, query, d), 
							 i, queries_df.shape[0])
				if not dry_run:
					# Run the query and measure response time
					start = time.time()
					if query['outside'] == 0: # is inside
						result_paa = pg.ksimpaa(k, name, name, query['index'], query['start'], query['end'], d)
					else:
						result_paa = pg.ksimpaa(k, name, name_out, query['index'], query['start'], query['end'], d)
					end = time.time()

					time_paa = end - start

					results[d].append({
						'query': query,
						'result_paa': result_paa,
						'time_paa': time_paa
					})

					# Dump result to file immediatelly
					with open(experiment_path, 'w') as f:
						json.dump(results, f)

					logging.info('Finished %s after %.1f seconds', 
								query_description('PAA', k, query, d), end - start)
			else:
				logging.info('Query %s is already run', query_description('PAA', k, query, d))

	pg.unloadDataset(name)
	logging.info('Unloaded %s', name)

	pg.unloadDataset(name_out)
	logging.info('Unloaded %s', name_out)


if __name__=='__main__':
	logging.basicConfig(format='%(asctime)s [%(levelname)s] %(name)s: %(message)s', 
						level=logging.INFO)

	parser = common_argparser('Generate random queries and run PAA method over them')

	args = parser.parse_args()
	logging.info('Args: %s', pprint.pformat(args))

	with open(GROUPING_RECORDS, 'r') as f:
		ds_info = json.load(f)

	try:
		for ds in get_dataset_order(ds_info):
			subseq = ds_info[ds]['subsequence']
			if check_subseq_range(subseq
								  , args.subseq_count_min
								  , args.subseq_count_max):
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
				run_paa(name
						, args.dist
						, args.k
						, queries
						, args.dry_run)

	except Exception as e:
		logging.exception('PAA stopped')
		if not args.dry_run:
			send_notification(args.email_addr, 'Error occured. PAA stopped', repr(e))
		
