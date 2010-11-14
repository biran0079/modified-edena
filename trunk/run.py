#!/usr/bin/python
import os,sys

def cmd_with_best_param(i,j):
	file_name="reads%d0%d.fna" % (i,j)
	if i>=2 and j>=2:
		file_name="new_"+file_name
	best_score=0
	best_cmd=""
	flags=["","-n","-N"]

	for m in range(15,28):
		for flag in flags:
			cmd="./edena1 "
			cmd+=file_name
			cmd+=" -m "+str(m)
			cmd+=" -o contigs%d0%d.fna"%(i,j)
			cmd+=" "+flag;
			cmd+=" -q"
			os.system(cmd)
			os.system("./eval.py %d0%d" % (i,j))
			score=float(file("%d0%d.eval"%(i,j)).read().split("\n")[4])
			print score
			if score > best_score:
				best_score=score
				best_cmd=cmd
	print best_cmd,best_score
	
	f=file("edena1_cmds","a")
	f.write(best_cmd+"\n")
	f.close()
	return best_cmd
	
flag=0;
for i in range(0,6):
	for j in range(1,7):
		if i<=1 and j==6:continue
		os.system(cmd_with_best_param(i,j))
