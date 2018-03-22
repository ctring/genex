import smtplib
from email.mime.text import MIMEText
import logging

STORAGE_ROOT = '/work/ctnguyendinh/groups'
DATASET_ROOT = '../datasets/UCR'
SMTP_SERVER = 'smtp.wpi.edu'
FROM_ADDR = 'noreply@wpi.edu'
GROUPING_RECORDS = '/work/ctnguyendinh/groups/grouping_records.json'
EXPERIMENT_RECORDS_ROOT = '/work/ctnguyendinh/experiments'

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
