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

struct shared_data {
    int init;
    int shutdown;
};

static void
_init_cb(evthr_t * thr, void * shared) {
    struct shared_data *s = (struct shared_data *) shared;
    int n = __sync_sub_and_fetch(&s->init, 1);
    printf("INIT _init_cb (%u): %d\n", (unsigned int)pthread_self(), n);
}

static void
_exit_cb(evthr_t * thr, void * shared) {
    struct shared_data *s = (struct shared_data *) shared;
    int n = __sync_add_and_fetch(&s->shutdown, 1);
    printf("SHUTDOWN _exit_cb (%u): %d\n", (unsigned int)pthread_self(), n);
}

static void
_test_cb_1(evthr_t * thr, void * cmdarg, void * shared) {
    struct shared_data *s = (struct shared_data *) shared;
    const char *c = (const char *) cmdarg;
    printf("START _test_cb_1 (%u): %s, %d %d\n", (unsigned int)pthread_self(), c, s->init, s->shutdown);
    ASSERT_STR_D("derp", c, "cmdarg mismatch");
    ASSERT_EQUAL_D(0, s->shutdown, "shutdown count can't be set on this step");
    sleep(1);
    printf("END   _test_cb_1 (%u): %s, %d %d\n", (unsigned int)pthread_self(), c, s->init, s->shutdown);
}

CTEST(evthr, evthr_pool) {
    evthr_pool_t * pool = NULL;
    int            i    = 0;
    struct shared_data s;

    s.init = 0;
    s.shutdown = 0;

    printf("\n");
/*
    evthread_use_pthreads();
    evthread_enable_lock_debuging();
*/
    pool = evthr_pool_wexit_new(8, _init_cb, _exit_cb, &s);

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

    ASSERT_EQUAL_D(-8, s.init, "init");
    ASSERT_EQUAL_D(8, s.shutdown, "shutdown");
}

int main(int argc, const char *argv[])
{
    return ctest_main(argc, argv);
}
