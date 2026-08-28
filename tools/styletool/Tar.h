// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#pragma once

#include <cstdint>
#include <filesystem>
#include <format>
#include <iostream>

#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QList>

extern "C" {
#include <zlib.h>
}

/*
 * This is a very simple and naive implementation of creating an in-memory tar
 * archive that is written to disk with gzip compression. This avoids having to
 * drag in a complete
 */

static constexpr uint8_t regularFileType = '0';
static constexpr uint8_t directoryType = '5';

// Header entry for an USTar format tar file
struct USTarHeader {
    char name[100] = {'\0'};
    char mode[8] = {'\0'};
    uint8_t uid[8] = {0};
    uint8_t gid[8] = {0};
    uint8_t size[12] = {0};
    uint8_t mtime[12] = {0};
    char checksum[8] = {0};
    uint8_t type = {0};
    uint8_t linkName[100] = {0};
    uint8_t ustar[8] = "ustar  ";
    uint8_t userName[32] = {0};
    uint8_t groupName[32] = {0};
    uint8_t deviceMajor[8] = {0};
    uint8_t deviceMinor[8] = {0};
    uint8_t namePrefix[155] = {0};
    uint8_t padding[12] = {0};
};

struct TarEntry {
    USTarHeader header;
    QByteArray data;
    QByteArray padding;
};

void tarDirectoryContents(const std::filesystem::path &directory, const std::filesystem::path &baseDirectory, QList<TarEntry> &output)
{
    for (auto entry : fs::directory_iterator(directory)) {
        auto pathString = fs::relative(entry.path(), baseDirectory).string();
        if (pathString.size() >= 100) {
            std::cout << "Skipping entry " << pathString << " because the file name is too long.\n";
            continue;
        }

        TarEntry tarEntry;

        auto &header = tarEntry.header;
        std::format_to_n(header.name, sizeof(header.name), "{}", pathString);
        std::format_to_n(header.mode, sizeof(header.mode), "{:07o}", static_cast<int>(entry.status().permissions()));
        std::format_to_n(header.size, sizeof(header.size), "{:011o}", entry.is_directory() ? 0 : entry.file_size());

        QFileInfo fileInfo(entry.path());

        std::format_to_n(header.uid, sizeof(header.uid), "{:07o}", fileInfo.ownerId());
        std::format_to_n(header.gid, sizeof(header.uid), "{:07o}", fileInfo.groupId());
        std::format_to_n(header.mtime, sizeof(header.mtime), "{:011o}", fileInfo.lastModified().toSecsSinceEpoch());

        std::format_to_n(header.userName, sizeof(header.userName), "{}", fileInfo.owner().toStdString());
        std::format_to_n(header.groupName, sizeof(header.groupName), "{}", fileInfo.group().toStdString());

        if (entry.is_directory()) {
            header.name[pathString.size()] = '/';
            header.type = directoryType;
        } else {
            header.type = regularFileType;

            QFile file(entry.path());
            if (!file.open(QIODevice::ReadOnly)) {
                std::cout << "Skipping entry " << pathString << " because it could not be opened.\n";
                continue;
            }

            tarEntry.data = file.readAll();
            tarEntry.padding = QByteArray{512 - tarEntry.data.size() % 512, '\0'};

            file.close();
        }

        strncpy(header.checksum, "        ", sizeof(header.checksum));

        auto headerData = reinterpret_cast<uint8_t *>(&header);
        uint32_t checksum = 0;
        for (int i = 0; i < 512; ++i) {
            checksum += headerData[i];
        }

        std::format_to_n(header.checksum, sizeof(header.checksum), "{:06o}", checksum);
        header.checksum[6] = '\0';
        header.checksum[7] = ' ';

        output.append(tarEntry);

        if (entry.is_directory()) {
            tarDirectoryContents(entry.path(), baseDirectory, output);
        }
    }
}

bool tarDirectory(const std::filesystem::path &directory, const std::filesystem::path &destination, std::error_code &errorCode)
{
    namespace fs = std::filesystem;

    if (!fs::exists(directory)) {
        errorCode = std::make_error_code(std::errc::no_such_file_or_directory);
        return false;
    }

    if (!fs::is_directory(directory)) {
        errorCode = std::make_error_code(std::errc::not_a_directory);
        return false;
    }

    if (fs::exists(destination)) {
        errorCode = std::make_error_code(std::errc::file_exists);
        return false;
    }

    QList<TarEntry> entries;
    tarDirectoryContents(directory, directory.parent_path(), entries);

    QByteArray data;
    for (const auto &entry : std::as_const(entries)) {
        data.append(reinterpret_cast<const char *>(&entry.header), sizeof(USTarHeader));
        data.append(entry.data);
        data.append(entry.padding);
    }

    data.append(QByteArray{1024, '\0'});

    auto outfile = gzopen(destination.string().data(), "wb");
    if (!outfile) {
        errorCode = std::error_code(errno, std::system_category());
        return false;
    }

    gzwrite(outfile, data.data(), data.size());
    gzclose(outfile);

    return true;
}
