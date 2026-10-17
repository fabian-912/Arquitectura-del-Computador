#pragma once

#include <string>
#include <cstddef>

struct FileSystemProfile {
    std::string name;
    std::size_t blockSizeBytes; // tamaño de bloque nativo del sistema de archivos
};
