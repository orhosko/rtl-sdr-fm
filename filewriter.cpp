#include "filewriter.h"

FileWriter::FileWriter(FileSaveable &fs) {
    ui->addItem(fs.controlWidget().get());
}
