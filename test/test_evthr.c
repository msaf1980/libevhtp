#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

#include <event2/event-config.h>

#include <evhtp/evthr.h>

#define CTEST_MAIN
#include <ctest.h>

static int _init_ = 0;
static int _shutdown_ = 0;

static void
_init_cb(evthr_t * thr, void * shared) {
    int n = __sync_sub_and_fetch(&_init_, 1);
    printf("INIT _init_cb (%u): %d\n", (unsigned int)pthread_self(), n);
}

static void
_exit_cb(evthr_t * thr, void * shared) {
    int n = __sync_add_and_fetch(&_shutdown_, 1);
    printf("SHUTDOWN _exit_cb (%u): %d\n", (unsigned int)pthread_self(), n);
}

static void
_test_cb_1(evthr_t * thr, void * cmdarg, void * shared) {
    printf("START _test_cb_1 (%u)\n", (unsigned int)pthread_self());
    sleep(1);
    printf("END   _test_cb_1 (%u)\n", (unsigned int)pthread_self());
}

CTEST(evthr, evthr_pool) {
    evthr_pool_t * pool = NULL;
    int            i    = 0;

    printf("\n");
/*
    evthread_use_pthreads();
    evthread_enable_lock_debuging();
*/
    pool = evthr_pool_wexit_new(8, _init_cb, _exit_cb, NULL);

    evthr_pool_start(pool);

    while (1) {
        if (i++ >= 5) {
            break;
        }

        printf("Iter %d\n", i);

        printf("%d\n", evthr_pool_defer(pool, _test_cb_1, "derp"));
        printf("%d\n", evthr_pool_defer(pool, _test_cb_1, "derp"));
        printf("%d\n", evthr_pool_defer(pool, _test_cb_1, "derp"));
        printf("%d\n", evthr_pool_defer(pool, _test_cb_1, "derp"));
        printf("%d\n", evthr_pool_defer(pool, _test_cb_1, "derp"));
        printf("%d\n", evthr_pool_defer(pool, _test_cb_1, "derp"));

        sleep(2);
    }

    evthr_pool_stop(pool);
    evthr_pool_free(pool);

    ASSERT_EQUAL_D(-8, _init_, "init");
    ASSERT_EQUAL_D(8, _shutdown_, "shutdown");
}

int main(int argc, const char *argv[])
{
    return ctest_main(argc, argv);
}
