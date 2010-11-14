COMPILE:
type "make" in the directory containg source code

RUN:
Please run on LINUX system with at least 2Gb memory.

edena1:	the modified edena
--------------------------------
Usage:
./edena1 <file name> [-m <MIN_OVERLAP_SIZE>(22 by default)]
        [-c <MIN_CONTIG_SIZE>(100 by default)]
        [-d <MIN_DEAD_END_LEN>(10 by default)]
        [-n (enable non-strict node)]
        [-N (enable very-non-strict node)]
        [-o <output file name>]

error2:	the error correction program
--------------------------------
Usage: ./error2 read_file_name
        [-s <RANGE_ST>(14 by default)]
        [-e <RANGE_ED>(27 by default)]
        [-d <STEP_SIZE>(2 by default)]
        [-o <output file name>]

error_estimate:	estimate errors in a read file
--------------------------------
Usgae:	./error_estimate <correct sequence file> <reads file>

