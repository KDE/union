# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

ecm_add_test(TestControls.cpp LINK_LIBRARIES Qt6::Test Qt6::Quick Union::Union)
set_tests_properties(TestControls PROPERTIES ENVIRONMENT "UNION_DISABLE_CACHE=1")
