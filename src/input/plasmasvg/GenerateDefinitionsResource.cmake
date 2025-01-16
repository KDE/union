# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
#
# This generates a QRC file for the definitions by listing all yml files in the definitions directory.
# This avoids having to maintain our own QRC file and related merge conflicts.
file(GLOB _definitions definitions/*.yml)
set(_qrc_content "<RCC>\n<qresource prefix=\"/org/kde/union/input/plasmasvg\">\n")
foreach(_file ${_definitions})
    get_filename_component(_name "${_file}" NAME)
    string(APPEND _qrc_content "<file alias=\"${_name}\">${_file}</file>\n")
endforeach()
string(APPEND _qrc_content "</qresource>\n</RCC>\n")
file(WRITE "${OUTPUT_DIR}/PlasmaSvgDefinitions.qrc" "${_qrc_content}")
