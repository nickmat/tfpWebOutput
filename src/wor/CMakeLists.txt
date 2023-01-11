# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Name:        tfp_wor/src/wor/CMakeLists.txt
# Project:     wor: Web Output for The Family Pack Rec Library.
# Author:      Nick Matthews
# Website:     http://thefamilypack.org
# Created:     11th January 2023
# Copyright:   Copyright (c) 2023, Nick Matthews.
# Licence:     GNU GPLv3
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

set( WOR_SRC_WOR_FILES
    worFiles.cpp
    worExport.cpp
    worProcess.cpp
    worWrFamily.cpp
    worWrIndividual.cpp
    worWrNames.cpp
)

set( WOR_SRC_WOR_HEADERS
    ../../include/wor/worExport.h
    worWr.h
    worFiles.h
)

add_library( wor ${WOR_SRC_WOR_FILES} ${WOR_SRC_WOR_HEADERS} )

target_link_libraries( wor PUBLIC reccl )
