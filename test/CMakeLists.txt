add_executable(test_evthr test_evthr.c)
target_link_libraries(test_evthr evhtp ${LIBEVHTP_EXTERNAL_LIBS} ${SYS_LIBS})
add_test(NAME test_evthr COMMAND test_evthr --exe $<TARGET_FILE:test_evthr>)

add_executable(bench_evthr bench_evthr.c)
target_link_libraries(bench_evthr evhtp ${LIBEVHTP_EXTERNAL_LIBS} ${SYS_LIBS})
