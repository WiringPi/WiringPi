#
# Copyright (c)2021- Alligator Descartes <http://www.hermitretro.com>
#
# This file is part of WiringPi
#
#     WiringPi is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
#
#     WiringPi is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with WiringPi.  If not, see <https://www.gnu.org/licenses/>.

include_directories(${PROJECT_SOURCE_DIR}/wiringPi)

add_compile_options(-D_FILE_OFFSET_BITS=64 -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast)

file(GLOB_RECURSE SOURCES "*.c")

link_directories(${PROJECT_SOURCE_DIR}/wiringPi ${PROJECT_SOURCE_DIR}/devLib)
add_executable(gpio ${SOURCES})
target_link_libraries(gpio wiringPi devLib m rt pthread crypt)
