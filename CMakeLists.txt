# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Name:        tfp_wor/CMakeLists.txt
# Project:     wor: Web Output for The Family Pack Rec Library. 
# Author:      Nick Matthews
# Website:     http://thefamilypack.org
# Created:     11th January 2023
# Copyright:   Copyright (c) 2023, Nick Matthews.
# Licence:     GNU GPLv3
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

include_directories( include )
include_directories( ../tfp/include )

add_subdirectory( src/wor )

add_subdirectory( utils/wor )
