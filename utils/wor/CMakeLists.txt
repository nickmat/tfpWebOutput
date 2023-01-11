# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Name:        tfp_wor/utils/wor/CMakeLists.txt
# Project:     runWor: Web Output for The Family Pack Rec Library.
# Author:      Nick Matthews
# Website:     http://thefamilypack.org
# Created:     11th January 2023
# Copyright:   Copyright (c) 2023, Nick Matthews.
# Licence:     GNU GPLv3
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

set( UTILS_RUNWOR_FILES
    runWor.cpp
)

set( UTILS_RUNWOR_HEADERS
    ../../include/wor/worExport.h
)

add_executable( runWor ${UTILS_RUNWOR_FILES} ${UTILS_RUNWOR_HEADERS} )

target_link_libraries( runWor PRIVATE wor )
