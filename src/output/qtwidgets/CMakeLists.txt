# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

add_library(UnionWidgetsStyle MODULE)

ecm_qt_declare_logging_category(UnionWidgetsStyle
    HEADER qtwidgets_logging.h
    IDENTIFIER UNION_QTWIDGETS
    CATEGORY_NAME union.qtwidgets
    DESCRIPTION "Union - QtWidgets Output Plugin"
    EXPORT Union
)

target_sources(UnionWidgetsStyle PRIVATE
        BackgroundDrawing.cpp
        BackgroundDrawing.h
        SharedNames.h
        StyleUtils.cpp
        StyleUtils.h
        UnionStyle.cpp
        UnionStyle.h
        UnionStylePlugin.cpp
        UnionStylePlugin.h
        elements/AbstractElement.cpp
        elements/AbstractElement.h
        elements/ButtonElement.cpp
        elements/ButtonElement.h
        elements/CheckBoxElement.cpp
        elements/CheckBoxElement.h
        elements/ComboBoxElement.cpp
        elements/ComboBoxElement.h
        elements/DockWidgetElement.cpp
        elements/DockWidgetElement.h
        elements/MenuElement.cpp
        elements/MenuElement.h
        elements/MenuBarElement.cpp
        elements/MenuBarElement.h
        elements/MenuBarItemElement.cpp
        elements/MenuBarItemElement.h
        elements/FrameElement.cpp
        elements/FrameElement.h
        elements/FocusElement.cpp
        elements/FocusElement.h
        elements/GroupBoxElement.cpp
        elements/GroupBoxElement.h
        elements/HeaderElement.cpp
        elements/HeaderElement.h
        elements/ItemViewElement.cpp
        elements/ItemViewElement.h
        elements/LineEditElement.cpp
        elements/LineEditElement.h
        elements/MenuItemElement.cpp
        elements/MenuItemElement.h
        elements/ProgressBarElement.cpp
        elements/ProgressBarElement.h
        elements/RadioButtonElement.cpp
        elements/RadioButtonElement.h
        elements/RubberBandElement.cpp
        elements/RubberBandElement.h
        elements/ScrollBarElement.cpp
        elements/ScrollBarElement.h
        elements/ScrollAreaCornerElement.cpp
        elements/ScrollAreaCornerElement.h
        elements/SliderElement.cpp
        elements/SliderElement.h
        elements/SizeGripElement.cpp
        elements/SizeGripElement.h
        elements/SpinBoxElement.cpp
        elements/SpinBoxElement.h
        elements/SplitterElement.cpp
        elements/SplitterElement.h
        elements/StatusBarElement.cpp
        elements/StatusBarElement.h
        elements/TabElement.cpp
        elements/TabElement.h
        elements/TabCloseButtonElement.cpp
        elements/TabCloseButtonElement.h
        elements/TabBarElement.cpp
        elements/TabBarElement.h
        elements/TabWidgetElement.cpp
        elements/TabWidgetElement.h
        elements/TitleBarElement.cpp
        elements/TitleBarElement.h
        elements/ToolBarElement.cpp
        elements/ToolBarElement.h
        elements/ToolBoxTabElement.cpp
        elements/ToolBoxTabElement.h
        elements/ToolButtonElement.cpp
        elements/ToolButtonElement.h
        elements/ToolTipElement.cpp
        elements/ToolTipElement.h
        elements/WidgetElement.cpp
        elements/WidgetElement.h
        elements/IndicatorElement.cpp
        elements/IndicatorElement.h
        elements/TreeViewElement.cpp
        elements/TreeViewElement.h
)
target_link_libraries(UnionWidgetsStyle
        PUBLIC
        Qt::Widgets
        Union::Union
)

set_target_properties(UnionWidgetsStyle PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS UnionWidgetsStyle DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/styles/)
install(FILES union.themerc DESTINATION ${KDE_INSTALL_DATADIR}/kstyle/themes)

