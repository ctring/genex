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
        return pd.read_csv(query_path)

    nin = int(nquery / 2)
    nout = nquery - nin

    inquery = generate_random_queries(nin, incount, inlength)
    outquery = generate_random_queries(nout, outcount, outlength)
    inquery = np.hstack(inquery, np.tile(0, (nin, 1)))
    outquery = np.hstack(outquery, np.tile(1, (nin, 1)))
    df = pd.DataFrame(np.vstack((inquery, outquery)),
                      columns=['index', 'start', 'end', 'outside'])
    return df


def run_brute_force(name, dist, k, queries, dry_run=False):
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

    for d in dist:
        for st in st_list:
            if not dry_run:
                # LOAD GROUP HERE

                start = time.time()

                # LOOP OVER QUERIES AND FIND K BEST MATCHES HERE
                # RECORD THE RESULT FOR EACH QUERY

                end = time.time()

        logging.info('Finished [%s, %s], k = %d after %f seconds', name, d, k, end - start)

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
    parser.add_argumnet('--k', type=int, default=15, help='Number of best matches to find')
	parser.add_argument('--start-over', action='store_true', help='Start from the beginning')
	parser.add_argument('--nquery', type=int, help='Number of random queries to generate')
	parser.add_argument('--dry-run', action='store_true', help='Only print the datasets and params to group')
	parser.add_argument('--email-addr', default=None, help='Email to send notification when finishing grouping')

	args = parser.parse_args()
	logging.info('Args: %s', pprint.pformat(args))

	with open(GROUPING_RECORDS, 'r') as f:
		ds_info = json.load(f)

    try:
		order = sorted([(ds_info[ds]['subsequence'], ds) 
                        for ds in ds_info if not ds.endswidth('_out')])
		order = zip(*order)[1]
		for ds in order:
			subseq = ds_info[ds]['subsequence']
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
			sent_notification(args.email_addr, 'Error occured. Brute force stopped', content)
	else:
		content = 'Touched xx/yy dataset(s)....'
		logging.info(content)
		if not args.dry_run:
			send_notification(args.email_addr, 'Brute force finished', content)
		
