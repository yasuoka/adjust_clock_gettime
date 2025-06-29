adjust_clock_gettime.so: adjust_clock_gettime.c
	$(CC) -shared -o $@ -Wall adjust_clock_gettime.c

clean:
	rm -f clock_gettime_wrap.so
