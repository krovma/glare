#include "glare/core/common.h"
#include <intrin.h>

namespace glare {
void _unimplemented_break()
{
	__debugbreak();
}
};