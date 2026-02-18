// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PluginFactory.h"

#include <Kirigami/Platform/StyleHints>

#include "../qtquick/plugin/StyleHints.h"

#include "PlatformTheme.h"
#include "Units.h"

// Avoid ambiguity between Kirigami StyleHints and Union StyleHints below.
using UnionStyleHints = ::StyleHints;

class KirigamiStyleHints : public Kirigami::Platform::StyleHints
{
public:
    KirigamiStyleHints(QObject *parent = nullptr)
        : Kirigami::Platform::StyleHints(parent)
    {
        m_unionStyleHints = static_cast<UnionStyleHints *>(qmlAttachedPropertiesObject<UnionStyleHints>(parent));
    }

    void propertyChanged(ChangedProperty change) override
    {
        if (!m_unionStyleHints) {
            return;
        }

        switch (change) {
        case ChangedProperty::UseAlternateBackgroundColor:
            m_unionStyleHints->setUseAlternatingColors(useAlternateBackgroundColor());
            break;
        case ChangedProperty::ShowFramedBackground:
            m_unionStyleHints->setShowFramedBackground(showFramedBackground());
            break;
        case ChangedProperty::TickMarkStepSize:
            m_unionStyleHints->setTickMarkStepSize(tickMarkStepSize());
            break;
        case ChangedProperty::IconName:
            m_unionStyleHints->setIconName(iconName());
            break;
        case ChangedProperty::IconSource:
            m_unionStyleHints->setIconSource(iconSource());
            break;
        }
    }

private:
    UnionStyleHints *m_unionStyleHints = nullptr;
};

PluginFactory::PluginFactory(QObject *parent)
    : Kirigami::Platform::PlatformPluginFactory(parent)
{
    Kirigami::Platform::StyleHints::setMakeStyleHintsFunction([](QObject *object) {
        return new KirigamiStyleHints(object);
    });
}

Kirigami::Platform::PlatformTheme *PluginFactory::createPlatformTheme(QObject *parent)
{
    return new PlatformTheme(parent);
}

Kirigami::Platform::Units *PluginFactory::createUnits(QObject *parent)
{
    return new Units(parent);
}
