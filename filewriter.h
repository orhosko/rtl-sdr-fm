#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "qboxlayout.h"
#include "qlayout.h"
#include <memory>

class FileSaveable
{
public:
    virtual ~FileSaveable() = default;

    virtual void toggleSave() = 0;
    virtual auto controlWidget() -> std::unique_ptr<QLayout> = 0;
};

class FileWriter
{
public:
    FileWriter(FileSaveable &fs);
private:
    std::unique_ptr<QVBoxLayout> ui{};
};

#endif // FILEWRITER_H
