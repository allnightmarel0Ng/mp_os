cmake_minimum_required(VERSION 3.21)
project(mp_os_course_work)

add_library(
        mp_os_course_work
        src/common.cpp
        src/database.cpp
        src/date_time.cpp
        # src/tcp_storage_server.cpp
        # src/tcp_client.cpp
        src/command_interpreter.cpp)
target_include_directories(
        mp_os_course_work
        PUBLIC
        ./include)
target_link_libraries(
        mp_os_course_work
        PUBLIC
        mp_os_cmmn)
target_link_libraries(
        mp_os_course_work
        PUBLIC
        mp_os_lggr_lggr)
target_link_libraries(
        mp_os_course_work
        PUBLIC
        mp_os_allctr_allctr)
target_link_libraries(
        mp_os_course_work
        PUBLIC
        mp_os_assctv_cntnr_srch_tr)
target_link_libraries(
        mp_os_course_work
        PUBLIC
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr)
set_target_properties(
        mp_os_course_work PROPERTIES
        LANGUAGES CXX
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "course work")

# add_executable(
#         mp_os_course_work_server
#         server.cpp)
# target_link_libraries(
#         mp_os_course_work_server
#         PUBLIC
#         mp_os_course_work)
# target_link_libraries(
#         mp_os_course_work_server
#         PUBLIC
#         Boost::system)
# set_target_properties(
#         mp_os_course_work_server PROPERTIES
#         LANGUAGES CXX
#         LINKER_LANGUAGE CXX
#         CXX_STANDARD 17
#         CXX_STANDARD_REQUIRED ON
#         CXX_EXTENSIONS OFF
#         VERSION 1.0
#         DESCRIPTION "course work server executable")

# add_executable(
#         mp_os_course_work_client
#         client.cpp)
# target_link_libraries(
#         mp_os_course_work_client
#         PUBLIC
#         mp_os_course_work)
# target_link_libraries(
#         mp_os_course_work_client
#         PUBLIC
#         Boost::system)
# set_target_properties(
#         mp_os_course_work_client PROPERTIES
#         LANGUAGES CXX
#         LINKER_LANGUAGE CXX
#         CXX_STANDARD 17
#         CXX_STANDARD_REQUIRED ON
#         CXX_EXTENSIONS OFF
#         VERSION 1.0
#         DESCRIPTION "course work client executable")
        
add_executable(
        mp_os_course_work_main
        main.cpp)
target_link_libraries(
        mp_os_course_work_main
        PUBLIC
        mp_os_course_work)
set_target_properties(
        mp_os_course_work_main PROPERTIES
        LANGUAGES CXX
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "course work client executable")