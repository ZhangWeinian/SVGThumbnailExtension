#pragma once

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(svgExtension)

#define debugLog qCDebug(svgExtension)
