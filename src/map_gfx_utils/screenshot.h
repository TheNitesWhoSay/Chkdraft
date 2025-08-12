#ifndef SCREENSHOT_H
#define SCREENSHOT_H
#include <optional>
#include <string>
#include <glad/glad.h>

std::optional<std::string> saveOpenGlViewportImage(GLint x, GLint y, GLint width, GLint height);

#endif