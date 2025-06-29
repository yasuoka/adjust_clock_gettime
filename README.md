adjust\_clock\_gettime
======================

Adjust the result of clock\_gettime(3) by LD\_PRELOAD.

```
$ cc -o adjust_clock_gettime_wrap.so -fPIC -shared adjust_clock_gettime.c
$ ADJUST_MONOTIME=-86400 LD_PRELOAD=adjust_clock_gettime_wrap.so PROGRAM
```
