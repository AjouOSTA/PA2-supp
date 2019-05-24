pipeline:
	gcc -o pipeline pipeline.c -lpthread

fully_parallel:
	gcc -o fully_parallel fully_parallel.c -lpthread

condvar:
	gcc -o condvar condvar-llnl.c -lpthread
