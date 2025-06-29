/*
 * Copyright (c) 2025 YASUOKA Masahiko <yasuoka@yasuoka.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * cc -o adjust_clock_gettime_wrap.so -fPIC -shared adjust_clock_gettime.c
 */

#include <stdlib.h>
#include <time.h>
#include <dlfcn.h>
#include <limits.h>

#ifndef	nitems
#define nitems(_x)	(sizeof((_x)) / sizeof((_x)[0]))
#endif

static void		  clock_gettime_initialize(void);
static int		  clock_diff(clockid_t);
static int		(*libc_clock_gettime)(clockid_t, struct timespec *)
			    = NULL;
static int volatile	  clock_gettime_initialized = 0;

void
clock_gettime_initialize(void)
{
	void	*libc;

	if (clock_gettime_initialized != 0)
		return;
	clock_gettime_initialized = 1;
	if ((libc = dlopen("libc.so", RTLD_NOW)) == NULL)
		return;

	libc_clock_gettime = dlsym(libc, "clock_gettime");
	dlclose(libc);
}

int
clock_gettime(clockid_t clock, struct timespec *now)
{
	int	 ret, d;

	clock_gettime_initialize();
	ret = libc_clock_gettime(clock, now);
	if (ret == 0) {
		d = clock_diff(clock);
		now->tv_sec -= d;
	}

	return (ret);
}

int
clock_diff(clockid_t clock)
{
	int			 i, ret;
	const char		*str, *errstr;
	struct {
		clockid_t	 val;
		const char	*env;
	} adjusts[] = {
	    { CLOCK_REALTIME,		"ADJUST_REALTIME" },
	    { CLOCK_PROCESS_CPUTIME_ID,	"ADJUST_PROCESS_CPUTIME_ID" },
	    { CLOCK_MONOTONIC,		"ADJUST_MONOTONIC" },
	    { CLOCK_THREAD_CPUTIME_ID,	"ADJUST_THREAD_CPUTIME_ID" },
	    { CLOCK_UPTIME,		"ADJUST_UPTIME" },
	    { CLOCK_BOOTTIME,		"ADJUST_BOOTTIME" }
	};

	for (i = 0; i < nitems(adjusts); i++) {
		if (adjusts [i].val == clock)
			break;
	}
	if (i >= nitems(adjusts))
		return (0);
	if ((str = getenv(adjusts[i].env)) == NULL)
		return (0);
	ret = strtonum(str, INT_MIN, INT_MAX, &errstr);
	if (errstr != NULL)
		return (0);

	return (ret);
}
