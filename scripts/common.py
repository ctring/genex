import os
import argparse
import json
import numpy as np
import pandas as pd
import smtplib
import pygenex as pg
from email.mime.text import MIMEText
import logging

DATASET_ROOT = '../datasets/UCR'
GROUPS_ROOT = '/work/ctnguyendinh/groups'
GROUPING_RECORDS = '/work/ctnguyendinh/groups/grouping_records.json'

EXPERIMENT_ROOT = '/work/ctnguyendinh/experiments'

SMTP_SERVER = 'smtp.wpi.edu'
FROM_ADDR = 'noreply@wpi.edu'

def send_notification(to_addr, subject, content):
	if to_addr is None:
		return
	msg = MIMEText(content)
	msg['Subject'] = subject
	msg['From'] = FROM_ADDR
	msg['To'] = to_addr

	s = smtplib.SMTP(SMTP_SERVER)
	s.sendmail(FROM_ADDR, [to_addr], msg.as_string())
	logging.info('Sent notification to %s', to_addr)
	s.quit()


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
	outquery = np.hstack((outquery, np.tile(1, (nout, 1))))
	df = pd.DataFrame(np.vstack((inquery, outquery)),
					  columns=['index', 'start', 'end', 'outside'])
	df.to_csv(query_path, index=False)
	logging.info('Generated %d queries for %s', df.shape[0], name)
	return df


def common_argparser(description):
	parser = argparse.ArgumentParser(description)
	parser.add_argument('--dist', nargs='+', default=['euclidean'],
						help='List of distances')
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
	return parser


def check_subseq_range(subseq, subseq_min, subseq_max):
	return (subseq_max < 0 and subseq_min < 0) or\
			   (subseq_min <= subseq <= subseq_max) or\
			   (subseq_min < 0 and subseq_max >= 0 and subseq <= subseq_max) or\
			   (subseq_max < 0 and subseq_min >= 0 and subseq >= subseq_min)


def get_dataset_order(ds_info):
	order = sorted([(ds_info[ds]['subsequence'], ds) 
								 for ds in ds_info if not ds.endswith('_out')])
	return zip(*order)[1]


def load_and_normalize(name):
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
	return name, name_out


def get_results_object(name):
	experiment_path = os.path.join(EXPERIMENT_ROOT, name + '.json')
	if os.path.exists(experiment_path):
		logging.info('Result file for %s exists', name)
		with open(experiment_path, 'r') as f:
			results = json.load(f)
	else:
		results = {}
	return results, experiment_path


def query_description(method, k, query, distance):
	return '%s[%d, %d, %d, %d, %d, %s]' % (method
										, k
										, query['index']
										, query['start']
										, query['end']
										, query['outside']
										, distance)


def compute_accuracy(dist1, dist2):
	a = np.array(dist1)
	b = np.array(dist2)
	return 1 - np.mean((a - b)/b)
