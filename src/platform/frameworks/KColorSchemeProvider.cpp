// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#include "KColorSchemeProvider.h"

#include <KColorUtils>
#include <QCoreApplication>
#include <QMetaEnum>
#include <Style.h>

#include "kcolorscheme_logging.h"

std::optional<QPalette::ColorGroup> colorGroupFromString(const QString &group)
{
    if (group.compare(u"active", Qt::CaseInsensitive) == 0) {
        return QPalette::Active;
    } else if (group.compare(u"inactive", Qt::CaseInsensitive) == 0) {
        return QPalette::Inactive;
    } else if (group.compare(u"disabled", Qt::CaseInsensitive) == 0) {
        return QPalette::Disabled;
    }

    return std::nullopt;
}

std::optional<KColorScheme::ColorSet> colorSetFromString(const QString &set)
{
    if (set.compare(u"window", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ColorSet::Window;
    } else if (set.compare(u"button", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ColorSet::Button;
    } else if (set.compare(u"view", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ColorSet::View;
    } else if (set.compare(u"header", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ColorSet::Header;
    } else if (set.compare(u"tooltip", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ColorSet::Tooltip;
    } else if (set.compare(u"selection", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ColorSet::Selection;
    } else if (set.compare(u"complementary", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ColorSet::Complementary;
    }

    return std::nullopt;
}

std::optional<KColorScheme::BackgroundRole> backgroundRoleFromString(const QString &role)
{
    if (role.compare(u"normal", Qt::CaseInsensitive) == 0) {
        return KColorScheme::BackgroundRole::NormalBackground;
    } else if (role.compare(u"alternate", Qt::CaseInsensitive) == 0) {
        return KColorScheme::BackgroundRole::AlternateBackground;
    } else if (role.compare(u"active", Qt::CaseInsensitive) == 0) {
        return KColorScheme::BackgroundRole::ActiveBackground;
    } else if (role.compare(u"positive", Qt::CaseInsensitive) == 0) {
        return KColorScheme::BackgroundRole::PositiveBackground;
    } else if (role.compare(u"neutral", Qt::CaseInsensitive) == 0) {
        return KColorScheme::BackgroundRole::NeutralBackground;
    } else if (role.compare(u"negative", Qt::CaseInsensitive) == 0) {
        return KColorScheme::BackgroundRole::NegativeBackground;
    } else if (role.compare(u"link", Qt::CaseInsensitive) == 0) {
        return KColorScheme::BackgroundRole::LinkBackground;
    } else if (role.compare(u"visited", Qt::CaseInsensitive) == 0) {
        return KColorScheme::BackgroundRole::VisitedBackground;
    }

    return std::nullopt;
}

std::optional<KColorScheme::ForegroundRole> foregroundRoleFromString(const QString &role)
{
    if (role.compare(u"text", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ForegroundRole::NormalText;
    } else if (role.compare(u"active", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ForegroundRole::ActiveText;
    } else if (role.compare(u"inactive", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ForegroundRole::InactiveText;
    } else if (role.compare(u"positive", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ForegroundRole::PositiveText;
    } else if (role.compare(u"neutral", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ForegroundRole::NeutralText;
    } else if (role.compare(u"negative", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ForegroundRole::NegativeText;
    } else if (role.compare(u"link", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ForegroundRole::LinkText;
    } else if (role.compare(u"visited", Qt::CaseInsensitive) == 0) {
        return KColorScheme::ForegroundRole::VisitedText;
    }

    return std::nullopt;
}

std::optional<KColorScheme::DecorationRole> decorationRoleFromString(const QString &role)
{
    if (role.compare(u"hover", Qt::CaseInsensitive) == 0) {
        return KColorScheme::DecorationRole::HoverColor;
    } else if (role.compare(u"focus", Qt::CaseInsensitive) == 0) {
        return KColorScheme::DecorationRole::FocusColor;
    }

    return std::nullopt;
}

KColorSchemeProvider::KColorSchemeProvider(QObject *parent)
    : ColorProvider(parent)
{
    qApp->installEventFilter(this);

    m_colorConfig = KSharedConfig::openConfig();
}

bool KColorSchemeProvider::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == qApp && event->type() == QEvent::ApplicationPaletteChange) {
        m_cache.clear();
        // Important: If we don't reparse the global configuration, we will not
        // get any new colors.
        m_colorConfig->reparseConfiguration();
    }
    return QObject::eventFilter(obj, event);
}

std::optional<Union::ColorProvider::Rgba> KColorSchemeProvider::color(const QStringList &arguments) const
{
    if (arguments.size() != 4) {
        qCWarning(UNION_KCOLORSCHEME) << "Invalid arguments for KColorScheme color provider, expected 4 got" << arguments.size();
        return std::nullopt;
    }

    auto argumentsHash = qHash(arguments, QHashSeed::globalSeed());
    if (m_cache.contains(argumentsHash)) {
        return m_cache.value(argumentsHash).value();
    }

    auto group = colorGroupFromString(arguments.at(0));
    if (!group) {
        qCWarning(UNION_KCOLORSCHEME) << "Invalid arguments for KColorScheme color provider, group" << arguments.at(0) << "does not exist";
        return std::nullopt;
    }

    auto colorSet = colorSetFromString(arguments.at(1));
    if (!colorSet) {
        qCWarning(UNION_KCOLORSCHEME) << "Invalid arguments for KColorScheme color provider, color set" << arguments.at(1) << "does not exist";
        return std::nullopt;
    }

    KColorScheme colorScheme(group.value(), colorSet.value(), m_colorConfig);

    auto rgbaFromQColor = [](const QColor &qcolor) {
        return Rgba{
            .r = uint8_t(qcolor.red()),
            .g = uint8_t(qcolor.green()),
            .b = uint8_t(qcolor.blue()),
            .a = uint8_t(qcolor.alpha()),
        };
    };

    std::optional<Rgba> value;
    auto roleType = arguments.at(2);
    if (roleType.compare(u"background", Qt::CaseInsensitive) == 0) {
        auto role = backgroundRoleFromString(arguments.at(3));
        if (role) {
            value = rgbaFromQColor(colorScheme.background(role.value()).color());
        } else {
            qCWarning(UNION_KCOLORSCHEME) << "Invalid arguments for KColorScheme color provider, background role" << arguments.at(3) << "does not exist";
        }
    } else if (roleType.compare(u"foreground", Qt::CaseInsensitive) == 0) {
        auto role = foregroundRoleFromString(arguments.at(3));
        if (role) {
            value = rgbaFromQColor(colorScheme.foreground(role.value()).color());
        } else {
            qCWarning(UNION_KCOLORSCHEME) << "Invalid arguments for KColorScheme color provider, foreground role" << arguments.at(3) << "does not exist";
        }
    } else if (roleType.compare(u"decoration", Qt::CaseInsensitive) == 0) {
        if (arguments.at(3) == u"frame") {
            QColor framecolor = KColorUtils::mix(colorScheme.background(KColorScheme::BackgroundRole::NormalBackground).color(),
                                                 colorScheme.foreground(KColorScheme::ForegroundRole::NormalText).color(),
                                                 KColorScheme::frameContrast(m_colorConfig));
            value = rgbaFromQColor(framecolor);
        } else {
            auto role = decorationRoleFromString(arguments.at(3));
            if (role) {
                value = rgbaFromQColor(colorScheme.decoration(role.value()).color());
            } else {
                qCWarning(UNION_KCOLORSCHEME) << "Invalid arguments for KColorScheme color provider, decoration role" << arguments.at(3) << "does not exist";
            }
        }
    } else {
        qCWarning(UNION_KCOLORSCHEME) << "Invalid arguments for KColorScheme color provider, role type" << roleType << "does not exist";
    }

    if (value) {
        m_cache.insert(argumentsHash, value.value());
    }

    return value;
}
