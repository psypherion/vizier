enable_testing()

# SmokeTest from test_main.cpp
add_executable(run_tests test_main.cpp)
target_link_libraries(run_tests vizier_core)
add_test(NAME SmokeTest COMMAND run_tests)

# EncoderTest from test_encoder.cpp
add_executable(test_encoder test_encoder.cpp)
target_link_libraries(test_encoder vizier_core)
add_test(NAME EncoderTest COMMAND test_encoder)


# Perft test
add_executable(test_perft test_perft.cpp)
target_link_libraries(test_perft vizier_core)
add_test(NAME PerftTest COMMAND test_perft)
