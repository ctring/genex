'''
This script runs the GENEX method on all datasets
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

def run_genex(name, dist, k, queries_df, num_subseq, dry_run=False):
	'''Run GENEX experiment

	For each distance, this method iterates over the queries. It checks if
	the current query was already performed and recorded to the json result file.
	If it was not, it run the GENEX method with that query.

	For GENEX method, we run 1-NN and 15-NN differently. For 1-NN, we run similarly
	as brute-force and PAA. For 15-NN, we also find the smallest extent of the parameter
	k_e such that we reach 100% for each query.

	JSON result structure (1-NN, in the same file as BF and PAA)
	{
		'euclidean': [{ 
						query: [index, start, end, outside]
						result_genex_0.1: [{'data': {'index': ..., 'end': .., 'start': ...}, 'dist': ...}, ...]
						time_genex_0.1: ....
					},
					{ 
						query: [index, start, end, outside]
						result_genex_0.1: [{'data': {'index': ..., 'end': .., 'start': ...}, 'dist': ...}, ...]
						time_genex_0.1: ....},
					...]
		'manhattan': ...
	}

	JSON result structure (15-NN, in a different file)
	{
		'euclidean': [{
			query: [index, start, end, outside]
			result_genex_0.1: [<accuracy_0.1>, <accuracy_0.2>, ..., <accuracy_5.0>]
			time_genex_0.1: [<time_0.1>, <time_0.2>, ..., <time_5.0>]
		},
		{
			query: [index, start, end, outside]
			result_genex_0.2: [<accuracy_0.1>, <accuracy_0.2>, ..., <accuracy_5.0>]
			time_genex_0.2: [<time_0.1>, <time_0.2>, ..., <time_5.0>]
		},
		...]
	}
	accuracy_x means x percent of the dataset is explored
	'''
	load_and_normalize(name)

	results, experiment_path = get_results_object(name)
	results_15nn, experiment_path_15nn = get_results_object(name + '_15NN')

	for d in dist:

		if d not in results:
			results[d] = []
		
		if d not in results_15nn:
			results_15nn[d] = []

		for st in [0.1, 0.2, 0.3, 0.4, 0.5]:
			group_file_name = '{}_GROUPS_{}'.format(name, str(st))
			group_file_path = GROUPS_ROOT + '/{}/{}/{}'.format(name, d, group_file_name)

			if not os.path.exists(group_file_path):
				logging.info('Group %s not found. Moving on.' % group_file_name)
				continue

			logging.info('Loading group file %s', group_file_name)
			noOfGroups = pg.loadGroups(name, group_file_path)
			logging.info('Loaded %s with %d groups', group_file_name, noOfGroups)

			method_key = 'genex_' + str(st)
			for i in range(queries_df.shape[0]):
				query = {
					'index': queries_df['index'][i],
					'start': queries_df.start[i],
					'end': queries_df.end[i],
					'outside': queries_df.outside[i]
				}

				#####################################
				##				 1-NN experiment				 ##
				#####################################
				logging.info('1-NN experiment for %s', 
										 query_description('GENEX', 1, query, d))
				find_query = filter(lambda o: method_key in o 
																			and o['query'] == query, results[d])
				if len(find_query) == 0:
					logging.info('Running %s %s...(%d/%d)',
								name, query_description('GENEX', 1, query, d), 
								i, queries_df.shape[0])
					if not dry_run:
						# Run the query and measure response time					
						start = time.time()
						query_name = name if query['outside'] == 0 else name_out
						result_genex = pg.sim(name, query_name,
																	query['index'], query['start'], query['end'])
						end = time.time()
						
						time_genex = end - start

						# Append new result to the result array
						results[d].append({
							'query': query,
							'result_' + method_key: result_genex,
							'time_' + method_key: time_genex
						})

						# Dump result to file immediatelly
						with open(experiment_path, 'w') as f:
							json.dump(results, f)
						
						logging.info('Finished %s after %.1f seconds', 
												 query_description('GENEX', 1, query, d), end - start)
				else:
					logging.info('Query %s is already run',
											 query_description('GENEX', 1, query, d))


				#####################################
				##				15-NN experiment				 ##
				#####################################
 				logging.info('15-NN experiment for %s',
										 query_description('GENEX', k, query, d))
				result_bf = filter(lambda o: 'result_bf' in o and o['query'] == query, results[d])
				find_query = filter(lambda o: method_key in o 
															and o['query'] == query, results_15nn[d])
				if len(find_query) == 0 and len(result_bf) > 0:
					logging.info('Running %s %s...(%d/%d)',
								name, query_description('GENEX', k, query, d), 
								i, queries_df.shape[0])

					if not dry_run:
						accuracy = []
						time = []
						for ke in arange(0.1, 5.1, 0.1):
							start = time.time()

							query_name = name if query['outside'] == 0 else name_out
							result_genex = pg.ksim(k, ke, name, query_name,
																		query['index'], query['start'], query['end'])
							end = time.time()
							acc = compute_accuracy(result_genex, result_bf)
							accuracy.append(acc)
							time.append(end - start)

						results_15nn[d].append({
							'query': query,
							'result_' + method_key: accuracy,
							'time_' + method_key: time
						})

						with open(experiment_path_15nn, 'w') as f:
							json.dump(results_15nn, f)
						
						logging.info('Finished %s after %.1f seconds', 
								query_description('GENEX', k, query, d), end - start)
	 			else:
					logging.info('Query %s is already run or its bf result does not exist',
											 query_description('GENEX', k, query, d))

	pg.unloadDataset(name)
	logging.info('Unloaded %s', name)

	pg.unloadDataset(name_out)
	logging.info('Unloaded %s', name_out)


if __name__=='__main__':
	logging.basicConfig(format='%(asctime)s [%(levelname)s] %(name)s: %(message)s', 
						level=logging.INFO)

	parser = common_argparser('Generate random queries and run GENEX method over them')

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
				run_genex(name
									, args.dist
									, args.k
									, queries
									, subseq
									, args.dry_run)

	except Exception as e:
		content = 'GENEX stopped - ' + repr(e)
		logging.error(content)
		if not args.dry_run:
			send_notification(args.email_addr, 'Error occured. GENEX stopped', content)
		
