/// All header files in glare engine must include this document first.
#pragma once

#include "glare/core/preference.h"
#include "glare/core/type.h"

#define STATIC
#define UNUSED(x) ((void)x)
#define NODISCARD [[nodiscard]]

// WINDOWS
#define NOMINMAX

namespace glare {
void _unimplemented_break();
};

#if defined(_DEBUG)
#define GLARE_DEBUG
#define UNIMPLEMENTED_BREAK {glare::_unimplemented_break();}
#else
#define UNIMPLEMENTED_BREAK {}
#endif

#pragma warning(disable:4201)
