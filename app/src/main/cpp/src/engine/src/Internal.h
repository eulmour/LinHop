#ifndef ENGINE_INTERNAL
#define ENGINE_INTERNAL

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#ifdef __linux__
#include <unistd.h>

int engine_file_exists_(const char *path) {

    return access( path, F_OK ) == 0;
}

#elif defined(_WIN32) || defined(_WIN64)
#include "Windows.h"

int engine_file_exists_(const char* path)
{
	DWORD dwAttrib = GetFileAttributes(path);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#endif
#endif // ENGINE_INTERNAL