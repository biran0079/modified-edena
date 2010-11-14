#!/usr/bin/python
import sys
assert len(sys.argv)==3,"sequence file, and read file required"
s=file(sys.argv[1]).readline()
for l in file(sys.argv[2]):
	if l[0]=='#' or l[0]=='>':continue
	l=l[:-1]
	print "%d:\t%d" % (len(l),s.find(l))
