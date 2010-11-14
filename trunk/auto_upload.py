#!/usr/bin/python
import sys,os
import upload
host='www-appn.comp.nus.edu.sg/~cs5206/cgi-bin/2010/index.cgi?TN=1'
fields=[('TN','1'),('.submit','Upload')]
for i in range(2,6):	#2...5
	for j in range(1,7):	#1...6
		file_name='contigs%d0%d.fna' % (i,j)
		content=file(file_name).read()
		upload.post_multipart(host,'',fields,[('upload_file',file_name,content)])
