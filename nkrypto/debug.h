#ifndef DEBUG_H
#define DEBUG_H

#include <windows.h>
#include <cstdio>
#include <QDebug>

#ifndef _DEBUG
#define PRINT_BUF_SIZE 1024
#define dprintf qDebug
#else
#define dprintf(...)
#endif

#endif // DEBUG_H
