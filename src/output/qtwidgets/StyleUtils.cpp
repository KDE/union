// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "StyleUtils.h"
#include "SharedNames.h"
#include <ElementQuery.h>
#include <StyleRegistry.h>

#include <QCheckBox>
#include <QListView>
#include <QPushButton>
#include <QRadioButton>
#include <QStyleOption>
#include <QStyleOptionFrame>
#include <QTableView>
#include <QTextOption>
#include <QTreeView>
#include <qstyleoption.h>

using namespace Qt::StringLiterals;
using namespace Union::Properties;

Qt::Alignment toQtVerticalAlignment(Union::Properties::Alignment alignment)
{
    switch (alignment) {
    case Union::Properties::Alignment::Unspecified:
    case Union::Properties::Alignment::Fill:
    case Union::Properties::Alignment::StackCenter:
    case Union::Properties::Alignment::StackFill:
    case Union::Properties::Alignment::Center:
        return Qt::AlignVCenter;
        break;
    case Union::Properties::Alignment::Start:
        return Qt::AlignTop;
        break;
    case Union::Properties::Alignment::End:
        return Qt::AlignBottom;
        break;
    }
    return Qt::AlignVCenter;
}

Qt::Alignment toQtHorizontalAlignment(Union::Properties::Alignment alignment)
{
    switch (alignment) {
    case Union::Properties::Alignment::Unspecified:
    case Union::Properties::Alignment::Start:
        return Qt::AlignLeft;
    case Union::Properties::Alignment::Center:
        return Qt::AlignHCenter;
    case Union::Properties::Alignment::End:
        return Qt::AlignRight;
    case Union::Properties::Alignment::Fill:
    case Union::Properties::Alignment::StackFill:
    case Union::Properties::Alignment::StackCenter:
        return Qt::AlignJustify;
    }
    return Qt::AlignHCenter;
}

Qt::TextElideMode toQtElideMode(Union::Properties::TextElide elideMode)
{
    Qt::TextElideMode elide;
    switch (elideMode) {
    case Union::Properties::TextElide::None:
        elide = Qt::TextElideMode::ElideNone;
        break;
    case Union::Properties::TextElide::Left:
        elide = Qt::TextElideMode::ElideLeft;
        break;
    case Union::Properties::TextElide::Middle:
        elide = Qt::TextElideMode::ElideMiddle;
        break;
    case Union::Properties::TextElide::Right:
        elide = Qt::TextElideMode::ElideRight;
        break;
    }
    return elide;
}
Qt::TextFlag toQtWrapMode(Union::Properties::TextWrapMode wrapMode)
{
    Qt::TextFlag wrap = Qt::TextFlag::TextDontClip;
    switch (wrapMode) {
    case Union::Properties::TextWrapMode::NoWrap:
    case Union::Properties::TextWrapMode::ManualWrap:
        break;
    case Union::Properties::TextWrapMode::WordWrap:
    case Union::Properties::TextWrapMode::WrapAtWordBoundaryOrAnywhere:
        wrap = Qt::TextFlag::TextWordWrap;
        break;
    case Union::Properties::TextWrapMode::WrapAnywhere:
        wrap = Qt::TextFlag::TextWrapAnywhere;
        break;
    }
    return wrap;
}

QRectF backgroundRectangle(const QStyleOption *option, const Union::Properties::StylePropertyGroup *properties)
{
    // Shrink the widget rect by the insets
    if (!option) {
        return QRectF();
    }
    QRectF rect = option->rect;
    if (const auto layout = properties->layout()) {
        if (layout->inset()) {
            rect = rect.marginsRemoved(layout->inset()->toMargins());
        }
    }
    return rect;
}

QString styleOptionToElementName(const QStyleOption *option)
{
    if (!option) {
        return ElementString::Widget; // Default items that have no styleoption.
    }
    switch ((QStyleOption::OptionType)option->type) {
    case QStyleOption::SO_Default:
        return ElementString::Widget;
    case QStyleOption::SO_FocusRect:
        return ElementString::FocusFrame;
    case QStyleOption::SO_Button:
        return ElementString::Button;
    case QStyleOption::SO_Tab:
        return ElementString::Tab;
    case QStyleOption::SO_MenuItem:
        return ElementString::MenuItem;
    case QStyleOption::SO_Frame:
    case QStyleOption::SO_TabWidgetFrame:
        return ElementString::Frame;
    case QStyleOption::SO_ProgressBar:
        return ElementString::ProgressBar;
    case QStyleOption::SO_ToolBox:
        return ElementString::ToolBox;
    case QStyleOption::SO_Header:
        return ElementString::Header;
    case QStyleOption::SO_DockWidget:
        return ElementString::DockWidget;
    case QStyleOption::SO_ViewItem:
        return ElementString::ItemViewItem;
    case QStyleOption::SO_TabBarBase:
        return ElementString::TabBar;
    case QStyleOption::SO_RubberBand:
        return ElementString::RubberBand;
    case QStyleOption::SO_ToolBar:
        return ElementString::ToolBar;
    case QStyleOption::SO_GraphicsItem:
        return ElementString::GraphicsItem;
    case QStyleOption::SO_Slider:
        return ElementString::Slider;
    case QStyleOption::SO_SpinBox:
        return ElementString::SpinBox;
    case QStyleOption::SO_ToolButton:
        return ElementString::ToolButton;
    case QStyleOption::SO_ComboBox:
        return ElementString::ComboBox;
    case QStyleOption::SO_TitleBar:
        return ElementString::TitleBar;
    case QStyleOption::SO_GroupBox:
        return ElementString::GroupBox;
    case QStyleOption::SO_SizeGrip:
        return ElementString::SizeGrip;
    case QStyleOption::SO_CustomBase:
        // Just return "Widget"
        break;
    // Handle complex cases by casting in case they resolve to one of these
    case QStyleOption::SO_Complex:
    case QStyleOption::SO_ComplexCustomBase: {
        if (qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            return ElementString::ComboBox;
        }
        if (qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            return ElementString::GroupBox;
        }
        if (qstyleoption_cast<const QStyleOptionSizeGrip *>(option)) {
            return ElementString::SizeGrip;
        }
        if (qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            return ElementString::Slider;
        }
        if (qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            return ElementString::SpinBox;
        }
        if (qstyleoption_cast<const QStyleOptionTitleBar *>(option)) {
            return ElementString::TitleBar;
        }
        if (qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            return ElementString::ToolButton;
        }
    } break;
    }
    return ElementString::Widget;
}

QString textFromOption(const QStyleOption *opt)
{
    if (const auto comboBoxOption = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
        return comboBoxOption->currentText;
    }

    switch ((QStyleOption::OptionType)opt->type) {
    case QStyleOption::SO_Button:
        if (const auto option = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            return option->text;
        }
        break;
    case QStyleOption::SO_ToolButton:
        if (const auto option = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            return option->text;
        }
        break;
    case QStyleOption::SO_DockWidget:
        if (const auto option = qstyleoption_cast<const QStyleOptionDockWidget *>(opt)) {
            return option->title;
        }
        break;
    case QStyleOption::SO_Header:
        if (const auto option = qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
            return option->text;
        }
        break;
    case QStyleOption::SO_MenuItem:
        if (const auto option = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
            return option->text;
        }
        break;
    case QStyleOption::SO_ProgressBar:
        if (const auto option = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
            return option->text;
        }
        break;
    case QStyleOption::SO_Tab:
        if (const auto option = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            return option->text;
        }
        break;
    case QStyleOption::SO_ToolBox:
        if (const auto option = qstyleoption_cast<const QStyleOptionToolBox *>(opt)) {
            return option->text;
        }
        break;
    case QStyleOption::SO_ViewItem:
        if (const auto option = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            return option->text;
        }
        break;
    case QStyleOption::SO_GroupBox:
        if (const auto option = qstyleoption_cast<const QStyleOptionGroupBox *>(opt)) {
            return option->text;
        }
        break;
    default:
        break;
    }
    return QString();
}

QSizeF iconSizeFromOption(const QStyleOption *opt)
{
    switch ((QStyleOption::OptionType)opt->type) {
    case QStyleOption::SO_Button:
        if (const auto option = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            return option->iconSize;
        }
        break;
    case QStyleOption::SO_ToolButton:
        if (const auto option = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            return option->iconSize;
        }
        break;
    case QStyleOption::SO_MenuItem:
        if (const auto option = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
            return QSizeF(option->maxIconWidth, option->maxIconWidth);
        }
        break;
    case QStyleOption::SO_Tab:
        if (const auto option = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            return option->iconSize;
        }
        break;
    case QStyleOption::SO_ViewItem:
        if (const auto option = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            if (!option->icon.isNull()) {
                return option->decorationSize;
            }
        }
        break;
    case QStyleOption::SO_ComboBox:
        if (const auto option = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
            return option->iconSize;
        }
        break;
    default:
        break;
    }
    return QSizeF(-1, -1);
}

int textFlagsFromProperties(Union::Properties::StylePropertyGroup *properties)
{
    int textFlags = Qt::AlignVCenter;
    const auto horizontalAlignment =
        properties->safePropertyLookup(Alignment::Start, &StylePropertyGroup::layout, &LayoutPropertyGroup::alignment, &AlignmentPropertyGroup::horizontal);
    const auto verticalAlignment =
        properties->safePropertyLookup(Alignment::Center, &StylePropertyGroup::layout, &LayoutPropertyGroup::alignment, &AlignmentPropertyGroup::vertical);
    const auto textAlign = toQtHorizontalAlignment(horizontalAlignment) | toQtVerticalAlignment(verticalAlignment);
    auto textWrap =
        toQtWrapMode(properties->safePropertyLookup(Union::Properties::TextWrapMode::NoWrap, &StylePropertyGroup::text, &TextPropertyGroup::wrapMode));
    textFlags |= textAlign;
    // Do not add wrap flags if we get DontClip
    // This could be done better
    if (textWrap != Qt::TextDontClip) {
        textFlags |= textWrap;
    }
    return textFlags;
}

QRectF centerRect(const QRectF &rect, int width, int height)
{
    return QRect(rect.left() + (rect.width() - width) / 2, rect.top() + (rect.height() - height) / 2, width, height);
}

QRectF unifiedRect(QMap<QString, LayoutItem> layoutMap)
{
    QRectF rect;
    for (const auto &m : layoutMap) {
        if (m.elementName != ElementString::Indicator) {
            rect = rect.united(m.rect.toRect());
        }
    }
    return rect;
}

QStringList frameHints(const QStyleOptionFrame *frameOption)
{
    QStringList hints;
    if (frameOption->features.testFlag(QStyleOptionFrame::Flat)) {
        hints.append(u"flat"_s);
    }
    if (frameOption->features.testFlag(QStyleOptionFrame::Rounded)) {
        hints.append(u"rounded"_s);
    }
    return hints;
}
