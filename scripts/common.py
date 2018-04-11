import argparse
import smtplib
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
	outquery = np.hstack((outquery, np.tile(1, (nin, 1))))
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