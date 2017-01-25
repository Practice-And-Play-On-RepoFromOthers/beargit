beargit: main.c beargit.c beargit.h util.h
	gcc -std=c99 -g main.c beargit.c -o beargit

clean:
	rm -rf beargit autotest test

check::
	python2.7 tester.pyc beargit.c
