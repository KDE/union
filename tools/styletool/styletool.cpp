// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#include <condition_variable>
#include <mutex>

#include <QStandardPaths>

#include <StyleRegistry.h>

#include "Common.h"

int handleInstallCommand([[maybe_unused]] const QStringList &arguments)
{
    auto parser = parseCommand(u"install"_s, arguments, [](QCommandLineParser *parser) {
        parser->setApplicationDescription(u"Install a new style."_s);
        parser->addPositionalArgument(u"<path>"_s, u"The path to the style to install."_s);
    });

    if (parser->positionalArguments().size() != 1) {
        showCommandHelp(parser.get(), u"install"_s, u"Invalid arguments for command \"install\""_s, 1);
    }

    auto packageName = parser->positionalArguments().first();
    auto packagePath = fs::absolute(fs::path(packageName.toStdString()));

    auto package = Union::StylePackage{packagePath};
    if (!package.isValid()) {
        return printPackageError(package);
    }

    auto handler = Union::StyleRegistry::instance()->packageHandler();
    if (auto result = handler->install(package); result != Union::PackageHandler::Error::None) {
        return printHandlerError(package, result);
    }

    std::cout << "Installed style " << qPrintable(package.id()) << "\n";
    return 0;
}

int handleCreateCommand([[maybe_unused]] const QStringList &arguments)
{
    auto parser = parseCommand(u"create"_s, arguments, [](QCommandLineParser *parser) {
        parser->setApplicationDescription(u"Create a new style. Any missing information will be requested interactively."_s);
        parser->addOptions({
            {u"name"_s, u"The name of the style."_s, u"name"_s},
            {u"id"_s, u"The ID of the style."_s, u"id"_s},
            {u"author"_s, u"The author of the style."_s, u"author"_s},
            {u"version"_s, u"The version of the style."_s, u"version"_s},
            {u"license"_s, u"The license of the style."_s, u"license"_s},
            {u"output-path"_s, u"The path where the style will be created. If not supplied, the current working directory will be used."_s, u"output-path"_s},
            {u"input-type"_s, u"The Union input type of the style to create. Currently only supports CSS"_s, u"input-type"_s},
            {u"install"_s, u"Install the style after creating it."_s},
        });
    });

    Union::PackageHandler::CreateInfo info;

    info.inputType = parser->value(u"input-type"_s);
    if (info.inputType.isEmpty()) {
        info.inputType = u"css"_s;
    }

    info.name = parser->value(u"name"_s);
    readInput(info.name, u"Enter the name of the style:");

    const auto nonIdCharacters = QRegularExpression(u"[^a-zA-Z0-9-_]"_s);
    const auto suggestedId = info.name.toLower().replace(nonIdCharacters, u"-"_s);

    auto id = parser->value(u"id"_s);

    auto idValidationFunction = [&nonIdCharacters](const QString &input) {
        if (input.contains(nonIdCharacters)) {
            std::cout << "The ID can only contain the characters a-z A-Z 0-9 - _\n";
            return false;
        } else {
            return true;
        }
    };

    if (!idValidationFunction(id)) {
        id = QString{};
    }

    readInput(id, u"Enter the ID of the style:", idValidationFunction, suggestedId);

    info.description = parser->value(u"description"_s);
    readInput(info.description, u"Enter the description of the style:");

    auto author = parser->value(u"author"_s);
    readInput(author, u"Enter the name of the author of the style:", u"Unknown Author"_s);
    info.authors = {author};

    info.version = parser->value(u"version"_s);
    readInput(info.version, u"Enter the version of the style:", u"0.1"_s);

    info.license = parser->value(u"license"_s);
    readInput(info.license, u"Enter the license of the style:", u"BSD-2-Clause"_s);

    std::cout << "Creating style:\n";
    std::cout << "  Input Type:  " << qPrintable(info.inputType) << "\n";
    std::cout << "  ID:          " << qPrintable(id) << "\n";
    std::cout << "  Name:        " << qPrintable(info.name) << "\n";
    std::cout << "  Description: " << qPrintable(info.description) << "\n";
    std::cout << "  Author:      " << qPrintable(info.authors.join(u", ")) << "\n";
    std::cout << "  License:     " << qPrintable(info.license) << "\n";
    std::cout << "  Version:     " << qPrintable(info.version) << "\n";

    if (parser->isSet(u"output-path"_s)) {
        info.path = fs::path(parser->value(u"output-path"_s).toStdString());
    } else {
        info.path = fs::current_path() / id.toStdString();
    }

    Union::StylePackage package;
    auto handler = Union::StyleRegistry::instance()->packageHandler();
    if (auto result = handler->create(package, info); result != Union::PackageHandler::Error::None) {
        return printHandlerError(package, result);
    }

    std::cout << "Created style " << package.path() << "\n";

    if (parser->isSet(u"install"_s)) {
        return handleInstallCommand({QString::fromStdString(package.path())});
    }

    return 0;
}

int handleInspectCommand([[maybe_unused]] const QStringList &arguments)
{
    auto parser = parseCommand(u"inspect"_s, arguments, [](QCommandLineParser *parser) {
        parser->setApplicationDescription(u"Inspect details of an installed Union style."_s);
        parser->addPositionalArgument(u"<path or id>"_s, u"The path of a style or the ID of an installed style to inspect."_s);
    });

    if (parser->positionalArguments().size() != 1) {
        showCommandHelp(parser.get(), u"inspect"_s, u"Invalid arguments for command \"inspect\""_s, 1);
    }

    auto pathOrId = parser->positionalArguments().first();

    auto handler = Union::StyleRegistry::instance()->packageHandler();

    auto package = handler->package(pathOrId);
    if (!package.isValid()) {
        package = Union::StylePackage{fs::absolute(fs::path(pathOrId.toStdString()))};
    }

    if (!package.isValid()) {
        return printPackageError(package);
    }

    std::cout << "Path:        " << package.path().string() << "\n";
    std::cout << "Input Type:  " << qPrintable(package.inputType()) << "\n";
    std::cout << "Name:        " << qPrintable(package.name()) << "\n";
    std::cout << "Description: " << qPrintable(package.description()) << "\n";
    std::cout << "Version:     " << qPrintable(package.version()) << "\n";
    std::cout << "License:     " << qPrintable(package.license()) << "\n";
    std::cout << "Authors:     " << qPrintable(package.authors().join(u", ")) << "\n";

    return 0;
}

int handleListCommand([[maybe_unused]] const QStringList &arguments)
{
    auto parser = parseCommand(u"list"_s, arguments, [](QCommandLineParser *parser) {
        parser->setApplicationDescription(u"List all the installed Union styles."_s);
        parser->addOption({u"hidden"_s, u"Include hidden packages."_s});
        parser->addOption({u"details"_s, u"Show details information about each package."_s});
    });

    auto handler = Union::StyleRegistry::instance()->packageHandler();
    auto packages = handler->allPackages();
    if (packages.isEmpty()) {
        std::cerr << "No styles could be found.\n";
        return 1;
    }

    std::ranges::stable_sort(packages, [](const Union::StylePackage &first, const Union::StylePackage &second) {
        return first.id() < second.name();
    });

    for (const auto &package : packages) {
        if (!parser->isSet(u"details"_s)) {
            std::cout << "- " << qPrintable(package.id()) << ": " << qPrintable(package.name()) << "\n";
        } else {
            std::cout << "- Path:        " << package.path().string() << "\n";
            std::cout << "  Input Type:  " << qPrintable(package.inputType()) << "\n";
            std::cout << "  Name:        " << qPrintable(package.name()) << "\n";
            std::cout << "  Description: " << qPrintable(package.description()) << "\n";
            std::cout << "  Version:     " << qPrintable(package.version()) << "\n";
            std::cout << "  License:     " << qPrintable(package.license()) << "\n";
            std::cout << "  Authors:     " << qPrintable(package.authors().join(u", ")) << "\n";
            std::cout << std::endl;
        }
    }

    return 0;
}

int handleUpdateCommand([[maybe_unused]] const QStringList &arguments)
{
    auto parser = parseCommand(u"update"_s, arguments, [](QCommandLineParser *parser) {
        parser->setApplicationDescription(u"Update a style."_s);
        parser->addPositionalArgument(u"<path>"_s, u"The path to the style to update."_s);
    });

    if (parser->positionalArguments().size() != 1) {
        showCommandHelp(parser.get(), u"update"_s, u"Invalid arguments for command \"update\""_s, 1);
    }

    auto packageName = parser->positionalArguments().first();
    auto package = Union::StylePackage{fs::absolute(fs::path(packageName.toStdString()))};

    if (!package.isValid()) {
        return printPackageError(package);
    }

    auto handler = Union::StyleRegistry::instance()->packageHandler();
    if (auto result = handler->update(package); result != Union::PackageHandler::Error::None) {
        return printHandlerError(package, result);
    }

    std::cout << "Updated style " << qPrintable(package.id()) << " to version " << qPrintable(package.version());
    return 0;
}

int handleUninstallCommand([[maybe_unused]] const QStringList &arguments)
{
    auto parser = parseCommand(u"uninstall"_s, arguments, [](QCommandLineParser *parser) {
        parser->setApplicationDescription(u"Uninstall am installed style."_s);
        parser->addPositionalArgument(u"<style>"_s, u"The style to uninstall."_s);
    });

    if (parser->positionalArguments().size() != 1) {
        showCommandHelp(parser.get(), u"uninstall"_s, u"Invalid arguments for command \"uninstall\""_s, 1);
    }

    auto style = parser->positionalArguments().first();

    auto handler = Union::StyleRegistry::instance()->packageHandler();

    auto package = handler->package(style);
    if (!package.isValid()) {
        std::cerr << qPrintable(style) << " was not found.\n";
        return 1;
    }

    if (auto result = handler->uninstall(package); result != Union::PackageHandler::Error::None) {
        return printHandlerError(package, result);
    }

    std::cout << "Uninstalled style " << qPrintable(style) << "\n";
    return 0;
}

int main(int argc, char **argv)
{
    QCoreApplication application(argc, argv);
    QCoreApplication::setApplicationVersion(u"1.0"_s);

    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    auto versionOption = parser.addVersionOption();
    auto helpOption = parser.addHelpOption();

    auto commandHelp = u"The command to execute.\nAvailable commands:\n"_s;
    commandHelp += u"   create     Create a new style.\n"_s;
    commandHelp += u"   inspect    Inspect details of an installed style.\n"_s;
    commandHelp += u"   install    Install a new style.\n"_s;
    commandHelp += u"   list       List installed styles.\n"_s;
    commandHelp += u"   update     Update an installed style.\n"_s;
    commandHelp += u"   uninstall  Uninstall an installed style.\n"_s;

    parser.addPositionalArgument(u"<command>"_s, commandHelp);

    auto result = parser.parse(application.arguments());
    if (!result && parser.unknownOptionNames().isEmpty()) {
        parser.showHelp(1);
    }

    if (parser.isSet(versionOption)) {
        parser.showVersion();
    }

    if (parser.isSet(helpOption)) {
        parser.showHelp(0);
    }

    if (parser.positionalArguments().isEmpty()) {
        parser.showHelp(1);
    }

    auto remaining = parser.positionalArguments();
    auto command = remaining.takeFirst().toLower();

    int commandResult = 0;

    if (command == u"create") {
        commandResult = handleCreateCommand(remaining);
    } else if (command == u"inspect") {
        commandResult = handleInspectCommand(remaining);
    } else if (command == u"install") {
        commandResult = handleInstallCommand(remaining);
    } else if (command == u"list") {
        commandResult = handleListCommand(remaining);
    } else if (command == u"update") {
        commandResult = handleUpdateCommand(remaining);
    } else if (command == u"uninstall") {
        commandResult = handleUninstallCommand(remaining);
    } else {
        parser.showMessageAndExit(QCommandLineParser::MessageType::Error, u"Unknown command: "_s + command, 1);
    }

    return commandResult;
}
