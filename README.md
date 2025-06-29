adjust\_clock\_gettime
======================

Tweak ``clock_gettime(3)`` for debug by ``LD_PRELOAD``.

To adjust -1 day the result of ``clock_gettime(CLOCK_MONOTIME,)``

```
$ cc -o adjust_clock_gettime_wrap.so -fPIC -shared adjust_clock_gettime.c
$ ADJUST_MONOTIME=-86400 LD_PRELOAD=adjust_clock_gettime_wrap.so PROGRAM
```
