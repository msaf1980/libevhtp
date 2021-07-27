/*
 * Try to run thpool with a non-zero heap and stack
 */
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <event2/event-config.h>

#include <evhtp/evthr.h>

size_t LOOP_COUNT = 1000000;

size_t v = 0;
int ret = 0;


static void
_test_cb_1(evthr_t * thr, void * cmdarg, void * shared) {
    size_t *n = (size_t *) cmdarg;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"	
	__atomic_add_fetch(n, 1, __ATOMIC_RELAXED);
#pragma GCC diagnostic pop
}

static uint64_t getCurrentTime(void) {
    struct timeval now;
    uint64_t now64;
    gettimeofday(&now, NULL);
    now64 = (uint64_t) now.tv_sec;
    now64 *= 1000000;
    now64 += ((uint64_t) now.tv_usec);
    return now64;
}

void bench(size_t loop_count, int threads) {
	size_t i;
	uint64_t start, end;
	evthr_pool_t * pool;
	v = 0;
	pool = evthr_pool_wexit_new(threads, NULL, NULL, NULL);
	evthr_pool_start(pool);

	start = getCurrentTime();
	
	for (i = 0; i < loop_count; i++) {
		evthr_pool_defer(pool, _test_cb_1, (void *)&v);
	};
	while (__atomic_add_fetch(&v, 0, __ATOMIC_RELAXED) < loop_count) {
		usleep(10);
	}

	end = getCurrentTime();

	evthr_pool_stop(pool);
    evthr_pool_free(pool);
	if (v != loop_count) {
		fprintf(stderr, "%d threads (%lu iterations) [ERR]\n",
			threads, (unsigned long) v);
		ret = 1;	
	} else {
		printf("%d threads (%f ms, %lu iterations, %llu ns/op) [OK]\n",
			threads,
			((double) end - (double) start) / 1000,
			(unsigned long) loop_count,
			(unsigned long long) (end - start) * 1000 / loop_count);
	}
}

int main() {
	char *COUNT_STR = getenv("LOOP_COUNT");
	if (COUNT_STR) {
		unsigned long c = strtoul(COUNT_STR, NULL, 10);
		if (c > 0) {
			LOOP_COUNT = c;
		}
	}
	bench(LOOP_COUNT, 4);
	return ret;
}
