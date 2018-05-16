#ifndef NKRYPTO_GLOBAL_H
#define NKRYPTO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(NKRYPTO_LIBRARY)
#include "debug.h"
#  define DLLPORT Q_DECL_EXPORT
#else
#  define DLLPORT Q_DECL_IMPORT
#endif

#endif // NKRYPTO_GLOBAL_H
