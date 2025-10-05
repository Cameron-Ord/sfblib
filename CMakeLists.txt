cmake_minimum_required(VERSION 3.10)
project(sfblib VERSION 1.0 LANGUAGES C)
set(SOURCES src/sfb.c)
add_library(sfb SHARED ${SOURCES})

