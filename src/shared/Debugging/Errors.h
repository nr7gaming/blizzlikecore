/*
 * BlizzLikeCore Copyright (C) 2013  BlizzLikeGroup
 * Integrated Files: CREDITS.md and LICENSE.md
 */

#ifndef BLIZZLIKECORE_ERRORS_H
#define BLIZZLIKECORE_ERRORS_H

#include "Define.h"

namespace BlizzLike
{

    DECLSPEC_NORETURN void Assert(char const* file, int line, char const* function, char const* message) ATTR_NORETURN;

    DECLSPEC_NORETURN void Fatal(char const* file, int line, char const* function, char const* message) ATTR_NORETURN;

    DECLSPEC_NORETURN void Error(char const* file, int line, char const* function, char const* message) ATTR_NORETURN;

    void Warning(char const* file, int line, char const* function, char const* message);

} // namespace BlizzLike

#define WPAssert(cond) do { if (!(cond)) BlizzLike::Assert(__FILE__, __LINE__, __FUNCTION__, #cond); } while (0)
#define WPFatal(cond, msg) do { if (!(cond)) BlizzLike::Fatal(__FILE__, __LINE__, __FUNCTION__, (msg)); } while (0)
#define WPError(cond, msg) do { if (!(cond)) BlizzLike::Error(__FILE__, __LINE__, __FUNCTION__, (msg)); } while (0)
#define WPWarning(cond, msg) do { if (!(cond)) BlizzLike::Warning(__FILE__, __LINE__, __FUNCTION__, (msg)); } while (0)

#define ASSERT WPAssert

#endif
