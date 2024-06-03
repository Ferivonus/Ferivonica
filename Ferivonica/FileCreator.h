#ifndef FILECREATOR_H
#define FILECREATOR_H

#include <string>
#include <windows.h>

class FileCreator {
public:
    static unsigned __stdcall createFiles(void* arg);
    static void startFileCreation();

private:
    void createTextFileWithRandomData(const std::string& filename);
    void createFilesInDirectory(const std::string& directoryPath);
};

#endif // FILECREATOR_H
