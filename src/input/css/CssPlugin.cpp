// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "CssPlugin.h"

#include <Style.h>

#include "CssLoader.h"

using namespace Qt::StringLiterals;

namespace fs = std::filesystem;

CssPlugin::CssPlugin(QObject *parent)
    : Union::InputPlugin(parent)
{
}

Union::StylePackage::Error CssPlugin::validatePackage(const Union::StylePackage &package)
{
    if (!fs::exists(package.path() / "contents" / "css" / "style.css")) {
        return StylePackage::Error::MissingFiles;
    }

    return StylePackage::Error::None;
}

std::shared_ptr<Union::Style> CssPlugin::createStyle(const QString &styleName) const
{
    return Union::Style::create(name(), styleName, std::make_unique<CssLoader>());
}

#include "moc_CssPlugin.cpp"
