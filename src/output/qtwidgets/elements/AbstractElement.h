// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "BackgroundDrawing.h"
#include "StyleUtils.h"
#include "qtwidgets_logging.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>

class UnionStyle;

enum class PaddingDirection {
    Inward,
    Outward
};

struct DrawEnums {
    int ControlElement = -1;
    int ComplexControl = -1;
    int PrimitiveElement = -1;
};

class AbstractElement : public QObject
{
    Q_OBJECT

public:
    AbstractElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~AbstractElement() override;

    QIcon icon() const;
    void setIcon(const QIcon &icon);
    bool hasIcon() const;

    QString text() const;
    void setText(const QString &text);
    bool hasText() const;

    QIcon indicator() const;
    void setIndicator(const QIcon &indicator);
    bool hasIndicator() const;

    /*!
     * \brief Returns the validity status. If the element has no properties loaded,
     * it is not valid and can not be drawn.
     */
    bool isValid() const;

    /*!
     * \brief Draw the whole element, including text, icon, background and indicator.
     */
    virtual void draw(QPainter *painter, DrawEnums enums) const;

    /*!
     * \brief Prepare the layoutMap of the element, and create the required properties.
     * By default this creates proeprties for background and content.
     */
    virtual void layout();

    /*!
     * \brief Return the contents size of the element with padding applied by default.
     */
    virtual QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const;

    /*!
     * \brief Return a subelement rectangle. If not found, empty QRect() is returned instead.
     */
    virtual QRectF subElementRect(QStyle::SubElement element) const;

    /*!
     * \brief Return a subcontrol rectangle. If not found, empty QRect() is returned instead.
     */
    virtual QRectF subControlRect(QStyle::SubControl subControl) const;

    /*!
     * \brief Return the given pixelmetric value. If not found, returns 0
     */
    virtual qreal pixelMetric(QStyle::PixelMetric pixelMetric) const;

    /*!
     * \brief Updates the properties of the element, such as text and layouting
     */
    virtual void update();

protected:
    /*!
     * \brief Draw text of the element.
     */
    virtual void drawText(QPainter *painter) const;

    /*!
     * \brief Draw icon of the element.
     */
    virtual void drawIcon(QPainter *painter) const;

    /*!
     * \brief Draw whole background of the element.
     */
    virtual void drawBackground(QPainter *painter) const;

    /*!
     * \brief Draw only the background frame of the element.
     */
    virtual void drawFrame(QPainter *painter) const;

    /*!
     * \brief Draw only the background panel of the element.
     */
    virtual void drawPanel(QPainter *painter) const;

    /*!
     * \brief Draw the indicator of the element. This can vary from secondary icon, such as drop-down
     * arrow icon, to a checkbox, depending on the element.
     */
    virtual void drawIndicator(QPainter *painter) const;

    /*!
     * \brief Translate styleoption values into attributes for this element.
     */
    virtual QVariantMap elementAttributes() const;

    /*!
     * \brief Translate styleoption values into attributes for this element.
     */
    virtual QStringList elementHints() const;

    /*!
     * \brief Translate states of styleoption into Union states.
     */
    virtual Union::Element::States elementStates() const;

    /*!
     * \brief Updates the m_subElementList with any values that are used when fetching a layout, so
     * that the element gets a proper hierarchy.
     */
    virtual void updateSubElementList();

    /*!
     * \brief Utilizes the background property to apply a padding to the given size.
     */
    QSizeF applyPaddingToSize(QSizeF oldSize,
                              PaddingDirection direction = PaddingDirection::Outward,
                              Union::Properties::StylePropertyGroup *properties = nullptr) const;

    /*!
     * \brief Query the size of an element based on its hierarchy. Useful for one-off calculations.
     */
    virtual QSizeF querySize(QStringList targetHierarchy) const;

    /*!
     * \brief Prepares elements for a widget. Sometimes we cannot decipher the specific item from widget alone, such as itemviews.
     * In those cases you may need to manually choose a target hierarchy, such as {"ItemViewItem"}
     */
    virtual Union::ElementList prepareElements(const QStyleOption *opt, const QWidget *widget, QStringList targetHierarchy = {}) const;

    /*!
     * \brief Queries the properties from list of elements. The properties match to the last element in the list,
     * inheriting anything it needs from its parents.
     */
    virtual Union::Properties::StylePropertyGroup *queryProperties(const Union::ElementList &elements) const;

    /*!
     * \brief Layouts list of elements, then returns a map of LayoutItems that contain information such as rectangles.
     * It will take list of elements, such as Button and any potential parents it has.
     * Then it uses the subElements stringlist to construct a layout: For example button is the container, then
     * Text, Icon and Indicator are the items to be layouted within the button container.
     * Currently only one container is used, which is the rectangle of the parent of the subElements.
     */
    virtual QMap<QString, LayoutItem> layoutMap(const Union::ElementList &elements, const QStyleOption *opt, const QStringList &subElements) const;

    virtual QMarginsF padding() const;

    virtual QMarginsF borderSize() const;

    virtual qreal height() const;

    virtual qreal width() const;

    virtual qreal spacing() const;

    virtual QSizeF indicatorSize() const;

    virtual QSizeF iconSize() const;

    qreal averagePadding() const;

    qreal averageBorderSize() const;

    qreal averageVPadding() const;

    qreal averageHPadding() const;

    QString elidedText(const QString &text, const QRectF &textRect, Union::Properties::StylePropertyGroup *properties) const;

    /*!
     * \brief Draw given icon at the rectangle. Handles the enabled status.
     */
    void drawIconAtRect(QPainter *painter, const QIcon &icon, const QRectF rect) const;

    /*!
     * \brief Draw text at the given rectangle. Handles coloring, elision and enabled status.
     */
    void drawTextAtRect(QPainter *painter, const QString &text, const QRectF &rect, Union::Properties::StylePropertyGroup *properties) const;

    const QStyleOption *m_styleOption;
    const UnionStyle *m_style;
    const QWidget *m_widget;
    QIcon m_icon = QIcon();
    QString m_text = QString();
    QIcon m_indicator = QIcon();
    Union::ElementList m_backgroundElementList;
    Union::ElementList m_contentElementList;
    Union::ElementList m_indicatorElementList;
    // Holds the properties for the background:
    // This is the top-level properties of the item by default.
    Union::Properties::StylePropertyGroup *m_backgroundProperties;
    // Holds the properties for any contents, such as text and icon.
    // This can vary a lot depending on the element.
    Union::Properties::StylePropertyGroup *m_contentProperties;
    // Holds the properties for any indicators, such as dropdown arrows.
    // This can vary a lot depending on the element.
    Union::Properties::StylePropertyGroup *m_indicatorProperties;
    QMap<QString, LayoutItem> m_layoutMap;
    QStringList m_subElementList;
    // Used to check if we have all elements properly prepared
    bool m_isValid = false;

private:
    Union::Element::Ptr createElement(const QString &name) const;
};
