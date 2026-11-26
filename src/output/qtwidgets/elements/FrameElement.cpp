// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "FrameElement.h"

#include <QAbstractScrollArea>
#include <QComboBox>
#include <QDockWidget>
#include <QLineEdit>
#include <QMenu>
#include <QStackedLayout>
#include <QTextBrowser>

#include "ShadowRenderer.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

static OptionToElementRegistration<FrameElement, QStyleOption::SO_Frame> frameOptionRegistration;
static WidgetToElementRegistration<FrameElement, QFrame> frameWidgetRegistration;

FrameElement::FrameElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
    // if (m_elementProperties && m_frameOption) {
    //     m_shadowWidget = qobject_cast<QWidget *>(m_frameOption->styleObject);
    //     if (m_shadowWidget) {
    //         m_shadowRenderer = std::make_unique<ShadowRenderer>(m_elementProperties);
    //     }
    // }
}

void FrameElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    e->setType({ElementString::Frame});

    auto frameOption = qstyleoption_cast<const QStyleOptionFrame *>(option);
    if (!frameOption) {
        return AbstractElement::update(option);
    }

    m_frameShape = frameOption->frameShape;

    e->setHints(frameHintsFor(frameOption));

    auto w = widget();
    if (w) {
        if (w->inherits("KDEPrivate::KPageListView") || w->inherits("KDEPrivate::KPageTreeView")) {
            const bool reverseLayout(direction() == Qt::RightToLeft);
            if (reverseLayout) {
                e->setHint(u"panel-left"_s, false);
                e->setHint(u"panel-right"_s, true);
            } else {
                e->setHint(u"panel-left"_s, true);
                e->setHint(u"panel-right"_s, false);
            }
        }

        if (w->parentWidget()) {
            if (const auto dock = qobject_cast<const QDockWidget *>(w->parentWidget())) {
                // For some reason the dock->dockLocation segfaults if there are no dockWidgetFeatures set
                // so check for that.
                if (!dock->features().testFlag(QDockWidget::NoDockWidgetFeatures)) {
                    // switch (dock->dockLocation()) {
                    //     case Qt::LeftDockWidgetArea:
                    //         e->setHint(u"dock-left"_s, true);
                    //         break;
                    //     case Qt::RightDockWidgetArea:
                    //         hints.append(u"dock-right"_s);
                    //         break;
                    //     case Qt::TopDockWidgetArea:
                    //         hints.append(u"dock-top"_s);
                    //         break;
                    //     case Qt::BottomDockWidgetArea:
                    //         hints.append(u"dock-bottom"_s);
                    //         break;
                    //     case Qt::DockWidgetArea_Mask:
                    //     case Qt::NoDockWidgetArea:
                    //         break;
                    // }
                }
            }
        }
    }

    e->setAttributes(frameAttributesFor(frameOption, widget()));

    AbstractElement::update(option);
}

void FrameElement::draw(QPainter *painter, DrawVariant what) const
{
    // Application has requested to disable frames for this element
    if (widget()) {
        const auto forceFrame = widget()->property("_breeze_force_frame");
        if (forceFrame.isValid() && !forceFrame.toBool()) {
            return;
        }
    }

    std::visit(overloaded{//
                          [](std::nullopt_t) { },
                          [](QStyle::ComplexControl) { },
                          [&](QStyle::ControlElement controlElement) {
                              if (controlElement != QStyle::CE_FocusFrame && controlElement != QStyle::CE_ShapedFrame) {
                                  return;
                              }

                              switch (m_frameShape) {
                              // Do nothing
                              case QFrame::NoFrame:
                                  break;
                              // We always allow HLine/VLine
                              case QFrame::HLine:
                              case QFrame::VLine:
                                  drawBackground(painter);
                                  break;
                              case QFrame::Box:
                              case QFrame::Panel:
                              case QFrame::WinPanel:
                                  drawBackground(painter);
                                  // drawShadow();
                                  break;
                              // Skip drawing frames for items that have framewidth of 0
                              case QFrame::StyledPanel:
                                  if (pixelMetric(nullptr, QStyle::PM_DefaultFrameWidth) != 0) {
                                      drawBackground(painter);
                                  }
                                  // drawShadow();
                                  break;
                              }
                          },
                          [&](QStyle::PrimitiveElement primitive) {
                              switch (primitive) {
                              case QStyle::PE_Frame:
                                  drawBackground(painter, UnionPainter::Parts::FrameOnly);
                                  break;
                              case QStyle::PE_FrameWindow:
                              case QStyle::PE_FrameDockWidget:
                                  drawBackground(painter, UnionPainter::Parts::FrameOnly);
                                  // drawShadow();
                                  break;
                              default:
                                  break;
                              }
                          }},
               what);
}

// void FrameElement::drawShadow() const
// {
//     if (m_shadowRenderer && m_shadowWidget) {
//         m_shadowRenderer->drawShadow(m_shadowWidget);
//     }
// }
//
// void FrameElement::drawFrame(QPainter *painter) const
// {
//     // Follow the expectations: If frame has panel shape,
//     // we need to draw the background too. In HLine/VLine cases
//     // we can just skip it
//     if (m_frameOption) {
//         switch (m_frameOption->frameShape) {
//         case QFrame::NoFrame:
//             // Only draw panel if we have autoFillBackground set
//             if (widget() && widget()->autoFillBackground()) {
//                 drawPanel(painter);
//             }
//             return;
//         case QFrame::Box:
//         case QFrame::Panel:
//         case QFrame::WinPanel:
//         case QFrame::StyledPanel:
//             drawBackground(painter);
//             break;
//         case QFrame::HLine:
//         case QFrame::VLine:
//             AbstractElement::drawBackground(painter);
//             break;
//         }
//     }
// }

QLayout *findParentLayout(const QWidget *widget)
{
    if (widget) {
        if (!widget->parentWidget()) {
            return nullptr;
        }

        auto layout = widget->parentWidget()->layout();
        if (!layout) {
            return nullptr;
        }

        if (layout->indexOf(const_cast<QWidget *>(widget)) > -1) {
            return layout;
        }

        QList<QObject *> children = layout->children();

        while (!children.isEmpty()) {
            layout = qobject_cast<QLayout *>(children.takeFirst());
            if (!layout) {
                continue;
            }

            if (layout->indexOf(const_cast<QWidget *>(widget)) > -1) {
                return layout;
            }
            children += layout->children();
        }
    }

    return nullptr;
}

std::optional<qreal> FrameElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    // Use same heuristics for drawing frames as Breeze does.
    // TODO: We may want to expose this to the user somehow.
    if (pixelMetric == QStyle::PM_DefaultFrameWidth) {
        auto w = widget();
        if (!w) {
            return std::nullopt;
        }
        const auto frameWidth = propertyWrapper().averageBorderSize();

        if (qobject_cast<const QMenu *>(w)) {
            return frameWidth;
        } else if (qobject_cast<const QLineEdit *>(w)) {
            return frameWidth;
        } else if (qobject_cast<const QComboBox *>(w)) {
            return frameWidth;
        }

        const auto forceFrame = w->property("_breeze_force_frame");
        if (forceFrame.isValid() && !forceFrame.toBool()) {
            return 0;
        }
        if ((forceFrame.isValid() && forceFrame.toBool()) || w->property("_breeze_borders_sides").isValid()) {
            return frameWidth;
        }

        if (qobject_cast<const QAbstractScrollArea *>(w)) {
            auto layout = findParentLayout(w);

            if (!layout) {
                if (w->parentWidget() && w->parentWidget()->layout()) {
                    layout = w->parentWidget()->layout();
                }
            }

            if (layout) {
                if (layout->inherits("QDockWidgetLayout") || layout->inherits("QMainWindowLayout") || qobject_cast<const QStackedLayout *>(layout)) {
                    return 0;
                }

                if (auto grid = qobject_cast<const QGridLayout *>(layout)) {
                    if (grid->horizontalSpacing() > 0 || grid->verticalSpacing() > 0) {
                        return frameWidth;
                    }
                }

                // Add frame when scroll area is in a layout with more than an item and the
                // layout has some spacing.
                if (layout->spacing() > 0 && layout->count() > 1) {
                    return frameWidth;
                }
            }
        }

        if (qobject_cast<const QTabWidget *>(w) || qobject_cast<const QTextBrowser *>(w)) {
            return frameWidth;
        }

        if (w->inherits("QComboBoxPrivateContainer") || w->inherits("KCompletionBox")) {
            return frameWidth;
        }

        // fallback
        return std::nullopt;
    }

    return std::nullopt;
}

std::optional<QRectF> FrameElement::subElementRect(const QStyleOption *option, QStyle::SubElement element) const
{
    if (element == QStyle::SE_FrameContents) {
        const auto r = style()->QCommonStyle::subElementRect(element, option, widget());
        return UnionStyle::visualRect(direction(), rect(), r.marginsRemoved(propertyWrapper().borderSize().toMargins()));
    }

    return std::nullopt;
}

Union::IdentifierVariantHash FrameElement::frameAttributesFor(const QStyleOptionFrame *frameOption, const QWidget *widget)
{
    Union::IdentifierVariantHash map;

    // For some reason both frame widget and styleoption can have frameshape,
    // and they can also differ. If we do not have a styleoption, rely on widget
    // information.
    QFrame::Shape shape;
    if (!frameOption) {
        if (const auto frame = qobject_cast<const QFrame *>(widget)) {
            shape = frame->frameShape();
        } else {
            return map;
        }
    } else {
        shape = frameOption->frameShape;
    }

    switch (shape) {
    case QFrame::NoFrame:
        map[u"shape"_s] = u"no-frame"_s;
        break;
    case QFrame::Box:
        map[u"shape"_s] = u"box"_s;
        break;
    case QFrame::Panel:
    case QFrame::WinPanel:
    case QFrame::StyledPanel:
        map[u"shape"_s] = u"panel"_s;
        break;
    case QFrame::HLine:
        map[u"shape"_s] = u"horizontal-line"_s;
        break;
    case QFrame::VLine:
        map[u"shape"_s] = u"vertical-line"_s;
        break;
    default:
        break;
    }

    return map;
}

Union::IdentifierList FrameElement::frameHintsFor(const QStyleOptionFrame *frameOption)
{
    Union::IdentifierList hints;
    if (frameOption) {
        if (frameOption->features.testFlag(QStyleOptionFrame::Flat)) {
            hints.append(u"flat"_s);
        }
        if (frameOption->features.testFlag(QStyleOptionFrame::Rounded)) {
            hints.append(u"rounded"_s);
        }
        // Some applications use the "_breeze_borders_sides" as a way to drive the Breeze application style,
        // keep it for backwards compatibility.
        if (frameOption->styleObject && frameOption->styleObject->property("_breeze_borders_sides").isValid()) {
            const auto edges = frameOption->styleObject->property("_breeze_borders_sides").value<Qt::Edges>();
            if (edges & Qt::LeftEdge) {
                hints.append(u"left-edge"_s);
            }
            if (edges & Qt::RightEdge) {
                hints.append(u"right-edge"_s);
            }
            if (edges & Qt::TopEdge) {
                hints.append(u"top-edge"_s);
            }
            if (edges & Qt::BottomEdge) {
                hints.append(u"bottom-edge"_s);
            }
        }
    }
    return hints;
}
