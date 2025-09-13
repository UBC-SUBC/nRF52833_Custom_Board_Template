cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app-v_0_0)

target_sources(app PRIVATE src/app_spi_slave_demo.c
               )

target_include_directories(app PUBLIC 
               ${CMAKE_CURRENT_SOURCE_DIR}/src)


