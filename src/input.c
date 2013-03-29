//========================================================================
// GLFW - An OpenGL library
// Platform:    Any
// API version: 3.0
// WWW:         http://www.glfw.org/
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2010 Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "internal.h"


//////////////////////////////////////////////////////////////////////////
//////                         GLFW event API                       //////
//////////////////////////////////////////////////////////////////////////

void _glfwInputKey(_GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (window->callbacks.key)
        window->callbacks.key((GLFWwindow*) window, key, scancode, action, mods);
}

void _glfwInputChar(_GLFWwindow* window, unsigned int character)
{
    if (character == -1)
        return;

    if (character < 32 || (character > 126 && character < 160))
        return;

    if (window->callbacks.character)
        window->callbacks.character((GLFWwindow*) window, character);
}

void _glfwInputScroll(_GLFWwindow* window, double xoffset, double yoffset)
{
    if (window->callbacks.scroll)
        window->callbacks.scroll((GLFWwindow*) window, xoffset, yoffset);
}

void _glfwInputMouseClick(_GLFWwindow* window, int button, int action, int mods)
{
    if (window->callbacks.mouseButton)
        window->callbacks.mouseButton((GLFWwindow*) window, button, action, mods);
}

void _glfwInputCursorMotion(_GLFWwindow* window, double x, double y)
{
    if (window->cursorMode == GLFW_CURSOR_CAPTURED)
    {
        if (x == 0.0 && y == 0.0)
            return;

        window->cursorPosX += x;
        window->cursorPosY += y;
    }
    else
    {
        if (window->cursorPosX == x && window->cursorPosY == y)
            return;

        window->cursorPosX = x;
        window->cursorPosY = y;
    }

    if (window->callbacks.cursorPos)
    {
        window->callbacks.cursorPos((GLFWwindow*) window,
                                    window->cursorPosX,
                                    window->cursorPosY);
    }
}

void _glfwInputCursorEnter(_GLFWwindow* window, int entered)
{
    if (window->callbacks.cursorEnter)
        window->callbacks.cursorEnter((GLFWwindow*) window, entered);
}


//////////////////////////////////////////////////////////////////////////
//////                        GLFW public API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI int glfwGetCursorMode(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT_OR_RETURN(0);
    return window->cursorMode;
}

GLFWAPI void glfwSetCursorMode(GLFWwindow* handle, int mode)
{
    int width, height, oldMode, centerPosX, centerPosY;
    _GLFWwindow* window = (_GLFWwindow*) handle;

    _GLFW_REQUIRE_INIT();

    if (mode != GLFW_CURSOR_NORMAL &&
        mode != GLFW_CURSOR_HIDDEN &&
        mode != GLFW_CURSOR_CAPTURED)
    {
        _glfwInputError(GLFW_INVALID_ENUM, NULL);
        return;
    }

    oldMode = window->cursorMode;
    if (oldMode == mode)
        return;

    _glfwPlatformGetWindowSize(window, &width, &height);

    centerPosX = width / 2;
    centerPosY = height / 2;

    if (oldMode == GLFW_CURSOR_CAPTURED || mode == GLFW_CURSOR_CAPTURED)
        _glfwPlatformSetCursorPos(window, centerPosX, centerPosY);

    _glfwPlatformSetCursorMode(window, mode);
    window->cursorMode = mode;

    if (oldMode == GLFW_CURSOR_CAPTURED)
        _glfwInputCursorMotion(window, window->cursorPosX, window->cursorPosY);
}

GLFWAPI void glfwGetCursorPos(GLFWwindow* handle, double* xpos, double* ypos)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;

    _GLFW_REQUIRE_INIT();

    if (xpos)
        *xpos = window->cursorPosX;

    if (ypos)
        *ypos = window->cursorPosY;
}

GLFWAPI void glfwSetCursorPos(GLFWwindow* handle, double xpos, double ypos)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;

    _GLFW_REQUIRE_INIT();

    if (_glfw.focusedWindow != window)
        return;

    // Don't do anything if the cursor position did not change
    if (xpos == window->cursorPosX && ypos == window->cursorPosY)
        return;

    // Set GLFW cursor position
    window->cursorPosX = xpos;
    window->cursorPosY = ypos;

    // Do not move physical cursor in locked cursor mode
    if (window->cursorMode == GLFW_CURSOR_CAPTURED)
        return;

    // Update physical cursor position
    _glfwPlatformSetCursorPos(window, xpos, ypos);
}

GLFWAPI void glfwSetKeyCallback(GLFWwindow* handle, GLFWkeyfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT();
    window->callbacks.key = cbfun;
}

GLFWAPI void glfwSetCharCallback(GLFWwindow* handle, GLFWcharfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT();
    window->callbacks.character = cbfun;
}

GLFWAPI void glfwSetMouseButtonCallback(GLFWwindow* handle, GLFWmousebuttonfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT();
    window->callbacks.mouseButton = cbfun;
}

GLFWAPI void glfwSetCursorPosCallback(GLFWwindow* handle, GLFWcursorposfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT();
    window->callbacks.cursorPos = cbfun;
}

GLFWAPI void glfwSetCursorEnterCallback(GLFWwindow* handle, GLFWcursorenterfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT();
    window->callbacks.cursorEnter = cbfun;
}

GLFWAPI void glfwSetScrollCallback(GLFWwindow* handle, GLFWscrollfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT();
    window->callbacks.scroll = cbfun;
}

