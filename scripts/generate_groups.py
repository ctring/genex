'''
This script generates groups for different values of st and distances.
Information of datasets to be grouped is read from the info file generated
by the extract_info.py script.

It updates the info file with current progress so that it can ignore already
grouped datasets the next time it is run again.
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
import smtplib
import time
from datetime import datetime
from email.mime.text import MIMEText

# Change these parameters if running on another machine
STORAGE_ROOT = '/work/ctnguyendinh/groups'
DATASET_ROOT = '../datasets/UCR'
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


def group_dataset(name, from_st, to_st, dist, num_threads=15, dry_run=False,
			      exclude_callback=None, progress_callback=None):
	dataset_path = os.path.join(DATASET_ROOT, name + '_DATA')
	info = pg.loadDataset(name, dataset_path, ',', -1, 1)
	logging.info('Loaded dataset %s. Count = %d. Length = %d', 
				 name, info['count'], info['length'])
	records = []
	timestamp = datetime.now().strftime('%Y_%m_%d_%H_%M_%S') 
	records_name = name + '_records_' + timestamp + '.csv'
	records_path = os.path.join(STORAGE_ROOT, name, records_name)
	for d in dist:
		for st in np.arange(from_st, to_st, 0.1):
			if exclude_callback is not None and exclude_callback(name, d, st):
				logging.info('Ignore [%s, %s, %.1f]', name, d, st)
				continue

			logging.info('Grouping [%s, %s, %.1f] with %d threads', name, d, st, num_threads)
			if dry_run:
				records.append({})

			if not dry_run:
				start = time.time()
				group_count = pg.group(name, st, d, num_threads)
				end = time.time()

				logging.info('Finished [%s, %s, %.1f] after %f seconds', name, d, st, end - start)
				logging.info('[%s, %s, %.1f] generates %d groups', name, d, st, group_count)

				save_dir = os.path.join(STORAGE_ROOT, name, d)
				if not os.path.exists(save_dir):
					os.makedirs(save_dir)
				save_path = os.path.join(save_dir, 
										 name + '_GROUPS_' + '{:.1f}'.format(st))
				logging.info('Saving groups [%s, %s, %.1f] to %s', name, d, st, save_path)
				pg.saveGroups(name, save_path)
				
				size_save_path = os.path.join(save_dir, 
											  name + '_GROUP_SIZES_' + '{:.1f}'.format(st))

				logging.info('Saving groups size [%s, %s, %.1f] to %s', name, d, st, size_save_path)
				pg.saveGroupsSize(name, size_save_path)

				records.append({
					'dist_name': d,
					'st': st,
					'group_count': group_count,
					'path': save_path,
					'size_path': size_save_path,
					'duration': end - start
				})

				records_df = pd.DataFrame(records)
				records_df.to_csv(records_path, index=False)
				logging.info('Saved grouping record for %s to %s', name, records_path)

				if progress_callback is not None:
					progress_callback(name, d, st)

	pg.unloadDataset(name)
	logging.info('Unloaded %s', name)
	return records


if __name__=='__main__':
	logging.basicConfig(format='%(asctime)s [%(levelname)s] %(name)s: %(message)s', 
	                    level=logging.INFO)
	parser = argparse.ArgumentParser('Generate groups')
	parser.add_argument('dataset_info_file', help='File containing information of the datasets')
	parser.add_argument('--subseq-count-min', type=int, default=-1,
						help='Group datasets with total subsequences larger than this.'
							 'Set a negative number to group all. Default: -1')
	parser.add_argument('--subseq-count-max', type=int, default=-1,
						help='Group datasets with total subsequences smaller than this.'
							 'Set a negative number to group all. Default: -1')
	parser.add_argument('--from-st', type=float, default=0.1,
						help='Group from this similarity threshold. Default: 0.1')
	parser.add_argument('--to-st', type=float, default=0.6,
						help='Group to this similarity threshold. Default: 0.6')
	parser.add_argument('--dist', nargs='+', default=['euclidean'],
					    help='List of distances to group with')
	parser.add_argument('--start-over', action='store_true', help='Start from the beginning')
	parser.add_argument('--dry-run', action='store_true', help='Only print the datasets and params to group')
	parser.add_argument('--email-addr', default=None, help='Email to send notification when finishing grouping')

	args = parser.parse_args()
	logging.info('Args: %s', pprint.pformat(args))
	
	with open(args.dataset_info_file, 'r') as f:
		ds_info = json.load(f)

	def exclude(ds_name, dist, st):
		if 'progress' in ds_info[ds_name]:
			excluded = ds_info[ds_name]['progress']
			return '%s %s %.1f' % (ds_name, dist, st) in excluded
		return False

	def progress(ds_name, dist, st):
		if 'progress' not in ds_info[ds_name]:
			ds_info[ds_name]['progress'] = []

		progress = ds_info[ds_name]['progress']
		params = '%s %s %.1f' % (ds_name, dist, st)

		if not params in progress:
			progress.append(params)

		with open(args.dataset_info_file, 'w') as f:
			json.dump(ds_info, f)

	try:
		all_records = {}
		record_count = 0
		subseq_max = args.subseq_count_max
		subseq_min = args.subseq_count_min
		order = sorted([(ds_info[ds]['subsequence'], ds) for ds in ds_info])
		order = zip(*order)[1]
		for ds in order:
			subseq = ds_info[ds]['subsequence']
			if (subseq_max < 0 and subseq_min < 0) or\
			   (subseq_min <= subseq <= subseq_max) or\
			   (subseq_min < 0 and subseq_max >= 0 and subseq <= subseq_max) or\
			   (subseq_max < 0 and subseq_min >= 0 and subseq >= subseq_min):
				if args.start_over:
					logging.info('Start over flag is set. Reset progress for %s', ds)
					if 'progress' in ds_info[ds]:
						del ds_info[ds]['progress']
				
				logging.info('%s. Number of subsequences %d', ds, ds_info[ds]['subsequence'])
				records = group_dataset(ds.encode('ascii', 'ignore'),
									    args.from_st, args.to_st, args.dist,
										exclude_callback=exclude, progress_callback=progress,
										dry_run=args.dry_run)
				all_records[ds] = records
				record_count += len(records)

	except Exception as e:
		content = 'Grouping stopped - ' + repr(e)
		logging.error(content)
		if not args.dry_run:
			sent_notification(args.email_addr, 'Error occured. Grouping stopped', content)
	else:
		nonempty_records = [all_records[ds] for ds in all_records if len(all_records[ds]) > 0]
		content = 'Touched %d/%d dataset(s). %d grouping files generated.\n' % (len(nonempty_records),
																				len(all_records), 
																			    record_count)
		logging.info(content)
		if not args.dry_run:
			for ds in all_records:
				content += '\n%s\n' % ds
				for r in all_records[ds]:
					content += '[%s, %.1f] - Group count = %d. Elapsed time = %.2f sec.\n' % (r['dist_name'],
																				 		      r['st'],
																						      r['group_count'],
																						      r['duration'])
			send_notification(args.email_addr, 'Grouping finished', content)
		
