// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "CssPlugin.h"

#include <fstream>

#include <QFile>

#include <Style.h>
#include <StylePackage.h>

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

Union::PackageHandler::Error CssPlugin::createPackage(const Union::StylePackage &package)
{
    auto path = package.path();

    if (!fs::create_directories(path / "contents" / "css")) {
        return Union::PackageHandler::Error::FilesystemError;
    }

    auto year = QDate::currentDate().year();
    auto author = package.authors().isEmpty() ? u"Unknown"_s : package.authors().first();

    {
        std::ofstream stream{path / "contents" / "css" / "style.css", std::ios::out};
        // REUSE-IgnoreStart
        stream << "/* SPDX-License-Identifier: " << (package.license().isEmpty() ? "Unknown" : qPrintable(package.license())) << " */\n";
        stream << "/* SPDX-FileCopyrightText: " << year << " " << qPrintable(author) << " */\n";
        stream << "\n";
        stream << "/* This is the main CSS file for the Union style " << qPrintable(package.name()) << " */\n";
        stream << "\n";
        stream << "/* It is recommended to not place any style rules in this file, but instead\n";
        stream << "   create separate files for different categories of rules and use @import\n";
        stream << "   here to include them in the style. */\n";
        // REUSE-IgnoreEnd

        if (stream.fail()) {
            return Union::PackageHandler::Error::FilesystemError;
        }
    }

    if (!fs::create_directories(path / "contents" / "images")) {
        return Union::PackageHandler::Error::FilesystemError;
    }

    {
        std::ofstream stream{path / "contents" / "images" / "README", std::ios::out};
        stream << "This directory should contain any images your style uses.\n";

        if (stream.fail()) {
            return Union::PackageHandler::Error::FilesystemError;
        }
    }

    return Union::PackageHandler::Error::None;
}

std::shared_ptr<Union::Style> CssPlugin::createStyle(const Union::StylePackage &package) const
{
    return Union::Style::create(package.path(), std::make_unique<CssLoader>());
}

#include "moc_CssPlugin.cpp"
