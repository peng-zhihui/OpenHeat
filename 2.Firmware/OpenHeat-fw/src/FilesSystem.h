#ifndef FILESSYSTEM_H
#define FILESSYSTEM_H

#include "Type.h"
#include "FS.h"
#include "SPIFFS.h"

extern char SYS_SVAE_PATH[];

void FilesSystemInit(void);

void SYS_Save(void);
void SYS_Load(void);

void listDir(fs::FS &fs, const char* dirname, uint8_t levels);
void readFile(fs::FS &fs, const char* path);
void writeFile(fs::FS &fs, const char* path, const char* message);
void appendFile(fs::FS &fs, const char* path, const char* message);
void renameFile(fs::FS &fs, const char* path1, const char* path2);
void deleteFile(fs::FS &fs, const char* path);
void testFileIO(fs::FS &fs, const char* path);
#endif