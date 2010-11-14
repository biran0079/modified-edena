all:	edena1 error2 error_estimate
edena1:	edena1.cpp
	g++ edena1.cpp -o edena1 -O3
error2:	error2.cpp
	g++ error2.cpp -o error2 -O3
error_estimate:	error_estimate.cpp
	g++ error_estimate.cpp -o error_estimate -O3

clear:
	rm edena1 error2 error_estimate

