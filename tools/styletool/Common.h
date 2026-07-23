// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#include <filesystem>
#include <iostream>

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

#include <PackageHandler.h>
#include <StylePackage.h>

using namespace Qt::StringLiterals;

namespace fs = std::filesystem;

void showCommandHelp(QCommandLineParser *parser, const QString &command, const QString &errorMessage = {}, int exitCode = 0)
{
    auto help = parser->helpText();
    help.replace(u"union-styletool"_s, u"union-styletool " + command);

    if (!errorMessage.isEmpty()) {
        help.prepend(errorMessage + u"\n");
        parser->showMessageAndExit(QCommandLineParser::MessageType::Error, help, exitCode);
    } else {
        parser->showMessageAndExit(QCommandLineParser::MessageType::Information, help, exitCode);
    }
}

[[nodiscard]] std::unique_ptr<QCommandLineParser>
parseCommand(const QString &command, const QStringList &arguments, std::function<void(QCommandLineParser *)> customiseFunction)
{
    auto commandParser = std::make_unique<QCommandLineParser>();
    commandParser->addOption({u"help"_s, u"Displays help for the current command."_s});

    customiseFunction(commandParser.get());

    QStringList argumentsWithApplication = {QCoreApplication::applicationName()};
    argumentsWithApplication.append(arguments);

    if (!commandParser->parse(argumentsWithApplication)) {
        QString errorMessage;
        if (!commandParser->unknownOptionNames().isEmpty()) {
            errorMessage = u"Unknown option for command "_s + command + u": " + commandParser->unknownOptionNames().first();
        }

        showCommandHelp(commandParser.get(), command, errorMessage, 1);
    }

    if (commandParser->isSet(u"help"_s)) {
        showCommandHelp(commandParser.get(), command);
    }

    return commandParser;
}

void readInput(QString &output, QStringView message, std::function<bool(QString)> validationFunction, QString defaultValue = {})
{
    Q_ASSERT(!message.isEmpty());

    if (output.isEmpty()) {
        QTextStream inputStream(stdin);
        QString input;
        while (input.isEmpty()) {
            std::cout << message.toUtf8().data();
            if (!defaultValue.isEmpty()) {
                std::cout << " [Enter to use: " << qPrintable(defaultValue) << "]" << std::endl;
            } else {
                std::cout << std::endl;
            }

            inputStream.readLineInto(&input, 1024);

            if (input.isEmpty() && !defaultValue.isEmpty()) {
                input = defaultValue;
            }

            if (!validationFunction(input)) {
                input = QString{};
            }
        }

        output = input;
    }
}

void readInput(QString &output, QStringView message, QString defaultValue = {})
{
    readInput(
        output,
        message,
        [](const QString &) {
            return true;
        },
        defaultValue);
}

int printPackageError(const Union::StylePackage &package)
{
    auto path = package.path();
    switch (package.error()) {
    case Union::StylePackage::Error::NotFound:
        std::cerr << path << " does not exist.\n";
        return 1;
    case Union::StylePackage::Error::MissingFiles:
        std::cerr << path << " is an invalid style because it is missing required files.\n";
        return 2;
    case Union::StylePackage::Error::InvalidMetaData:
        std::cerr << path << " is an invalid style because its metadata could not be read.\n";
        return 3;
    case Union::StylePackage::Error::UnknownInputType:
        std::cerr << path << " is an invalid style because it uses an unknown input type.\n";
        return 4;
    case Union::StylePackage::Error::None:
        return 0;
    }

    return 0;
}

int printHandlerError(const Union::StylePackage &package, Union::PackageHandler::Error error)
{
    auto path = package.path();
    switch (error) {
    case Union::PackageHandler::Error::InvalidPackage:
        std::cerr << path << "is an invalid style:\n";
        printPackageError(package);
        return 5;
    case Union::PackageHandler::Error::AlreadyInstalled:
        std::cerr << "A style with ID " << qPrintable(package.id()) << " is already installed.\n";
        return 6;
    case Union::PackageHandler::Error::FilesystemError:
        std::cerr << "A filesystem error occurred.\n";
        return 7;
    case Union::PackageHandler::Error::NotInstalled:
        std::cerr << "The style " << qPrintable(package.id()) << " is not installed.\n";
        return 8;
    case Union::PackageHandler::Error::PackageExists:
        std::cerr << "A style already exists at " << package.path() << "\n";
        return 9;
    case Union::PackageHandler::Error::None:
        break;
    }

    return 0;
}
