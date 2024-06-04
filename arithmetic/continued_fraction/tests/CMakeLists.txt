cmake_minimum_required(VERSION 3.21)
project(mp_os_arthmtc_cntnd_frctn_tests)

add_executable(
        mp_os_arthmtc_cntnd_frctn_tests
        continued_fraction_tests.cpp)
target_link_libraries(
        mp_os_arthmtc_cntnd_frctn_tests
        PUBLIC
        mp_os_arthmtc_cntnd_frctn)
set_target_properties(
        mp_os_arthmtc_cntnd_frctn_tests PROPERTIES
        LANGUAGES CXX
        LINKER_LANGUAGE CXX
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "continued fraction tests")