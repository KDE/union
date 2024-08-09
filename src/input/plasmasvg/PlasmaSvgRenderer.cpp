/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "PlasmaSvgRenderer.h"

#include <QBuffer>
#include <QFile>
#include <QTransform>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <KCompressionDevice>

#include "PlasmaSvgLoader.h"

#include "plasmasvg_logging.h"

using namespace Qt::StringLiterals;

std::size_t qHash(const PlasmaSvgRenderer::RendererId &renderer, std::size_t seed)
{
    return qHashMulti(seed, renderer.theme, renderer.path, renderer.colorGroup, renderer.colorSet);
}

struct ColorInfo {
    QString name;
    std::optional<KColorScheme::ForegroundRole> foregroundRole = std::nullopt;
    std::optional<KColorScheme::BackgroundRole> backgroundRole = std::nullopt;
    std::optional<KColorScheme::DecorationRole> decorationRole = std::nullopt;
};

PlasmaSvgRenderer::PlasmaSvgRenderer()
{
}

QPalette::ColorGroup PlasmaSvgRenderer::colorGroup() const
{
    return m_colorGroup;
}

void PlasmaSvgRenderer::setColorGroup(QPalette::ColorGroup group)
{
    if (group == m_colorGroup) {
        return;
    }

    m_colorGroup = group;
}

KColorScheme::ColorSet PlasmaSvgRenderer::colorSet() const
{
    return m_colorSet;
}

void PlasmaSvgRenderer::setColorSet(KColorScheme::ColorSet colorSet)
{
    if (colorSet == m_colorSet) {
        return;
    }

    m_colorSet = colorSet;
}

QString PlasmaSvgRenderer::path() const
{
    return m_path;
}

void PlasmaSvgRenderer::setPath(const QString &path)
{
    if (path == m_path) {
        return;
    }

    m_path = path;
}

bool PlasmaSvgRenderer::load()
{
    if (m_path.isEmpty()) {
        return false;
    }

    KCompressionDevice file(m_path, KCompressionDevice::GZip);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(UNION_PLASMASVG) << "Could not open SVG for rendering" << m_path;
        return false;
    }

    auto styleSheet = createStylesheet();

    auto data = file.readAll();
    auto styledData = QByteArray(data.size(), 0);

    QXmlStreamReader xmlReader(data);
    QBuffer buffer(&styledData);
    buffer.open(QIODevice::WriteOnly);
    QXmlStreamWriter xmlWriter(&buffer);

    while (!xmlReader.atEnd()) {
        auto next = xmlReader.readNext();

        if (next == QXmlStreamReader::Invalid) {
            continue;
        }

        auto guard = qScopeGuard([&]() {
            xmlWriter.writeCurrentToken(xmlReader);
        });

        if (next != QXmlStreamReader::StartElement) {
            continue;
        }

        if (xmlReader.qualifiedName() != u"style") {
            continue;
        }

        if (xmlReader.attributes().value(u"id") != u"current-color-scheme") {
            continue;
        }

        guard.dismiss();

        xmlWriter.writeStartElement(u"style");
        xmlWriter.writeAttributes(xmlReader.attributes());
        xmlWriter.writeCharacters(styleSheet);
        xmlWriter.writeEndElement();

        while (xmlReader.tokenType() != QXmlStreamReader::EndElement) {
            xmlReader.readNext();
        }
    }
    buffer.close();

    m_renderer = std::make_unique<QSvgRenderer>(styledData);

    return true;
}

bool PlasmaSvgRenderer::elementExists(const QString &elementName)
{
    if (!m_renderer) {
        return false;
    }

    return m_renderer->elementExists(elementName);
}

QRectF PlasmaSvgRenderer::elementRect(const QString &elementName)
{
    if (!elementExists(elementName)) {
        return QRectF{};
    }

    return m_renderer->transformForElement(elementName).map(m_renderer->boundsOnElement(elementName)).boundingRect();
}

void PlasmaSvgRenderer::render(QPainter *painter, const QString &elementName, const QRectF &bounds)
{
    if (!m_renderer) {
        return;
    }

    m_renderer->render(painter, elementName, bounds);
}

std::shared_ptr<PlasmaSvgRenderer> PlasmaSvgRenderer::rendererForPath(const std::shared_ptr<Union::Theme> &theme,
                                                                      QAnyStringView path,
                                                                      QPalette::ColorGroup colorGroup,
                                                                      Union::Element::ColorSet colorSet)
{
    auto pathString = path.toString();

    if (colorSet == Union::Element::ColorSet::None) {
        colorSet = Union::Element::ColorSet::Window;
    }

    RendererId id{theme.get(), pathString, colorGroup, colorSet};

    if (s_rendererCache.contains(id)) {
        return s_rendererCache.value(id);
    }

    auto loader = static_cast<PlasmaSvgLoader *>(theme->loader());
    const auto fileName = loader->plasmaTheme()->imagePath(pathString);
    if (fileName.isEmpty()) {
        return nullptr;
    }

    auto renderer = std::make_shared<PlasmaSvgRenderer>();
    renderer->setPath(fileName);
    renderer->setColorGroup(colorGroup);
    renderer->setColorSet(static_cast<KColorScheme::ColorSet>(static_cast<int>(colorSet) - 1));

    if (!renderer->load()) {
        qCWarning(UNION_PLASMASVG) << "Renderer for path" << pathString << "failed to load";
        return nullptr;
    }

    s_rendererCache.insert(id, renderer);
    return renderer;
}

void PlasmaSvgRenderer::clearRenderers(const std::shared_ptr<Union::Theme> &theme)
{
    s_rendererCache.removeIf([&theme](auto itr) {
        return itr.key().theme == theme.get();
    });
}

QString PlasmaSvgRenderer::createStylesheet()
{
    QString stylesheet;

    static const auto colors = {
        ColorInfo{.name = u"Text"_s, .foregroundRole = KColorScheme::NormalText},
        ColorInfo{.name = u"Background"_s, .backgroundRole = KColorScheme::NormalBackground},
        ColorInfo{.name = u"Highlight"_s, .decorationRole = KColorScheme::HoverColor},
        ColorInfo{.name = u"HighlightedText"_s, .foregroundRole = KColorScheme::ActiveText},
        ColorInfo{.name = u"PositiveText"_s, .foregroundRole = KColorScheme::PositiveText},
        ColorInfo{.name = u"NeutralText"_s, .foregroundRole = KColorScheme::NeutralText},
        ColorInfo{.name = u"NegativeText"_s, .foregroundRole = KColorScheme::NegativeText},

        ColorInfo{.name = u"ButtonText"_s, .foregroundRole = KColorScheme::NormalText},
        ColorInfo{.name = u"ButtonBackground"_s, .backgroundRole = KColorScheme::NormalBackground},
        ColorInfo{.name = u"ButtonHover"_s, .decorationRole = KColorScheme::HoverColor},
        ColorInfo{.name = u"ButtonFocus"_s, .decorationRole = KColorScheme::FocusColor},
        ColorInfo{.name = u"ButtonHighlightedText"_s, .foregroundRole = KColorScheme::ActiveText},
        ColorInfo{.name = u"ButtonPositiveText"_s, .foregroundRole = KColorScheme::PositiveText},
        ColorInfo{.name = u"ButtonNeutralText"_s, .foregroundRole = KColorScheme::NeutralText},
        ColorInfo{.name = u"ButtonNegativeText"_s, .foregroundRole = KColorScheme::NegativeText},

        ColorInfo{.name = u"ViewText"_s, .foregroundRole = KColorScheme::NormalText},
        ColorInfo{.name = u"ViewBackground"_s, .backgroundRole = KColorScheme::NormalBackground},
        ColorInfo{.name = u"ViewHover"_s, .decorationRole = KColorScheme::HoverColor},
        ColorInfo{.name = u"ViewFocus"_s, .decorationRole = KColorScheme::FocusColor},
        ColorInfo{.name = u"ViewHighlightedText"_s, .foregroundRole = KColorScheme::ActiveText},
        ColorInfo{.name = u"ViewPositiveText"_s, .foregroundRole = KColorScheme::PositiveText},
        ColorInfo{.name = u"ViewNeutralText"_s, .foregroundRole = KColorScheme::NeutralText},
        ColorInfo{.name = u"ViewNegativeText"_s, .foregroundRole = KColorScheme::NegativeText},

        ColorInfo{.name = u"TooltipText"_s, .foregroundRole = KColorScheme::NormalText},
        ColorInfo{.name = u"TooltipBackground"_s, .backgroundRole = KColorScheme::NormalBackground},
        ColorInfo{.name = u"TooltipHover"_s, .decorationRole = KColorScheme::HoverColor},
        ColorInfo{.name = u"TooltipFocus"_s, .decorationRole = KColorScheme::FocusColor},
        ColorInfo{.name = u"TooltipHighlightedText"_s, .foregroundRole = KColorScheme::ActiveText},
        ColorInfo{.name = u"TooltipPositiveText"_s, .foregroundRole = KColorScheme::PositiveText},
        ColorInfo{.name = u"TooltipNeutralText"_s, .foregroundRole = KColorScheme::NeutralText},
        ColorInfo{.name = u"TooltipNegativeText"_s, .foregroundRole = KColorScheme::NegativeText},

        ColorInfo{.name = u"ComplementaryText"_s, .foregroundRole = KColorScheme::NormalText},
        ColorInfo{.name = u"ComplementaryBackground"_s, .backgroundRole = KColorScheme::NormalBackground},
        ColorInfo{.name = u"ComplementaryHover"_s, .decorationRole = KColorScheme::HoverColor},
        ColorInfo{.name = u"ComplementaryFocus"_s, .decorationRole = KColorScheme::FocusColor},
        ColorInfo{.name = u"ComplementaryHighlightedText"_s, .foregroundRole = KColorScheme::ActiveText},
        ColorInfo{.name = u"ComplementaryPositiveText"_s, .foregroundRole = KColorScheme::PositiveText},
        ColorInfo{.name = u"ComplementaryNeutralText"_s, .foregroundRole = KColorScheme::NeutralText},
        ColorInfo{.name = u"ComplementaryNegativeText"_s, .foregroundRole = KColorScheme::NegativeText},

        ColorInfo{.name = u"HeaderText"_s, .foregroundRole = KColorScheme::NormalText},
        ColorInfo{.name = u"HeaderBackground"_s, .backgroundRole = KColorScheme::NormalBackground},
        ColorInfo{.name = u"HeaderHover"_s, .decorationRole = KColorScheme::HoverColor},
        ColorInfo{.name = u"HeaderFocus"_s, .decorationRole = KColorScheme::FocusColor},
        ColorInfo{.name = u"HeaderHighlightedText"_s, .foregroundRole = KColorScheme::ActiveText},
        ColorInfo{.name = u"HeaderPositiveText"_s, .foregroundRole = KColorScheme::PositiveText},
        ColorInfo{.name = u"HeaderNeutralText"_s, .foregroundRole = KColorScheme::NeutralText},
        ColorInfo{.name = u"HeaderNegativeText"_s, .foregroundRole = KColorScheme::NegativeText},
    };

    const QString styleTemplate = QStringLiteral(".ColorScheme-%1{color:%2;}");

    KColorScheme colorScheme{m_colorGroup, m_colorSet};
    QColor color;

    for (const auto &info : colors) {
        if (info.foregroundRole.has_value()) {
            color = colorScheme.foreground(info.foregroundRole.value()).color();
        } else if (info.backgroundRole.has_value()) {
            color = colorScheme.background(info.backgroundRole.value()).color();
        } else if (info.decorationRole.has_value()) {
            color = colorScheme.decoration(info.decorationRole.value()).color();
        } else {
            color = Qt::magenta;
        }

        stylesheet += styleTemplate.arg(info.name, color.name());
    }

    return stylesheet;
}
