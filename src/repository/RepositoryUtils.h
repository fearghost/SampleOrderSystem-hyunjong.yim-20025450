#pragma once
#include <filesystem>
#include <string>

namespace RepositoryUtils {
    inline void ensureDirectoryExists(const std::string& filePath) {
        const auto dir = std::filesystem::path(filePath).parent_path();
        if (!dir.empty()) std::filesystem::create_directories(dir);
    }
}
