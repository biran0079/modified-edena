#!/usr/bin/python
from urllib import urlopen
import gzip,os
url="http://www.comp.nus.edu.sg/~cs5206/2010/DNSRA/sim/"
for i in range(2,6):	#2..5
	for j in range(1,7):	#1..6
		filename="reads%d0%d.fna.gz" % (i,j)
		f=file(filename,"wb")
		f.write(urlopen(url+filename).read());
		f.close();
		gf=gzip.open(filename,"rb")
		content=gf.read()
		read_file_name=filename[:-3]
		if os.path.exists(read_file_name):continue
		f=file(read_file_name,"wb")
		f.write(content)
		f.close()
