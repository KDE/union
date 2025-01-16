# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

add_executable(union-ruleinspector main.cpp)
target_link_libraries(union-ruleinspector Qt6::Core Qt6::Gui Union::Union)
