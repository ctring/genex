import smtplib
from email.mime.text import MIMEText
import logging

DATASET_ROOT = '../datasets/UCR'
GROUPS_ROOT = '/work/ctnguyendinh/groups'
GROUPING_RECORDS = '/work/ctnguyendinh/groups/grouping_records.json'

EXPERIMENT_ROOT = '/work/ctnguyendinh/experiments'
EXPERIMENT_RECORDS = '/work/ctnguyendinh/experiments/experiment_records.json'

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
