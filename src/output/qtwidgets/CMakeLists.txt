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
        BoxShadowRenderer.cpp
        DragEventFilter.cpp
        Layout.cpp
        ShadowRenderer.cpp
        StyleGroupWrapper.cpp
        StyleUtils.cpp
        TypeMapping.cpp
        UnionStyle.cpp
        UnionStylePlugin.cpp
        UnionGraphicsEffect.cpp
        UnionPainter.cpp

        elements/AbstractElement.cpp
        elements/ButtonElement.cpp
        elements/CheckBoxElement.cpp
        elements/ComboBoxElement.cpp
        elements/DockWidgetElement.cpp
        # elements/MenuElement.cpp
        # elements/MenuBarItemElement.cpp
        elements/FrameElement.cpp
        elements/FocusElement.cpp
        elements/GroupBoxElement.cpp
        # elements/HeaderViewElement.cpp
        elements/IconSubElement.cpp
        elements/IndicatorSubElement.cpp
        # elements/ItemViewElement.cpp
        elements/LineEditElement.cpp
        elements/MenuBarElement.cpp
        # elements/MenuItemElement.cpp
        # elements/ProgressBarElement.cpp
        # elements/RadioButtonElement.cpp
        # elements/RubberBandElement.cpp
        # elements/ScrollBarElement.cpp
        # elements/ScrollAreaCornerElement.cpp
        # elements/SliderElement.cpp
        # elements/SizeGripElement.cpp
        # elements/SpinBoxElement.cpp
        # elements/SplitterElement.cpp
        # elements/StatusBarElement.cpp
        # elements/TabElement.cpp
        # elements/TabCloseButtonElement.cpp
        # elements/TabBarElement.cpp
        # elements/TabWidgetElement.cpp
        # elements/TitleBarElement.cpp
        # elements/ToolBarElement.cpp
        # elements/ToolBoxTabElement.cpp
        elements/ToolButtonElement.cpp
        # elements/ToolTipElement.cpp
        # elements/WidgetElement.cpp
        # elements/IndicatorElement.cpp
        # elements/TreeViewElement.cpp

        elements/TextSubElement.cpp
)

target_sources(UnionWidgetsStyle PUBLIC
        FILE_SET HEADERS
        FILES
        BoxShadowRenderer.h
        DragEventFilter.h
        Layout.h
        ShadowRenderer.h
        SharedNames.h
        StyleGroupWrapper.h
        StyleUtils.h
        TypeMapping.h
        UnionStyle.h
        UnionStylePlugin.h
        UnionGraphicsEffect.h
        UnionPainter.h

        elements/AbstractElement.h
        elements/ButtonElement.h
        elements/CheckBoxElement.h
        elements/ComboBoxElement.h
        elements/DockWidgetElement.h
        # elements/MenuElement.h
        # elements/MenuBarItemElement.h
        elements/FrameElement.h
        elements/FocusElement.h
        elements/GroupBoxElement.h
        # elements/HeaderViewElement.h
        elements/IconSubElement.h
        elements/IndicatorSubElement.h
        # elements/ItemViewElement.h
        elements/LineEditElement.h
        elements/MenuBarElement.h
        # elements/MenuItemElement.h
        # elements/ProgressBarElement.h
        # elements/RadioButtonElement.h
        # elements/RubberBandElement.h
        # elements/ScrollBarElement.h
        # elements/ScrollAreaCornerElement.h
        # elements/SliderElement.h
        # elements/SizeGripElement.h
        # elements/SpinBoxElement.h
        # elements/SplitterElement.h
        # elements/StatusBarElement.h
        # elements/TabElement.h
        # elements/TabCloseButtonElement.h
        # elements/TabBarElement.h
        # elements/TabWidgetElement.h
        # elements/TitleBarElement.h
        # elements/ToolBarElement.h
        # elements/ToolBoxTabElement.h
        elements/ToolButtonElement.h
        # elements/ToolTipElement.h
        # elements/WidgetElement.h
        # elements/IndicatorElement.h
        # elements/TreeViewElement.h

        elements/TextSubElement.h
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

