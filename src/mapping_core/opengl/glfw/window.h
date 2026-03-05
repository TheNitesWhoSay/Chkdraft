#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <type_traits>

namespace glfw
{
    struct Window
    {
        GLFWwindow* window = nullptr;

        void setVersionHint(int major, int minor)
        {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
        }

        void setCoreProfileHint()
        {
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }

        bool create(int width, int height, const char* title)
        {
            this->window = glfwCreateWindow(width, height, title, NULL, NULL);
            return this->window != NULL;
        }

        void makeContextCurrent()
        {
            glfwMakeContextCurrent(window);
        }

        void setInputMode(int mode, int value)
        {
            glfwSetInputMode(window, mode, value);
        }

        void setCursorEnabledInputMode()
        {
            setInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        void setCursorDisabledInputMode()
        {
            setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        template <typename T>
        void enableForwarding(T* t)
        {
            glfwSetWindowUserPointer(window, static_cast<void*>(t));
        }

        template <auto F, typename ... Ts>
        static inline void forward(GLFWwindow* window, Ts && ... ts)
        {
            using Instance = typename decltype([]<class T>(auto T::*) { return std::type_identity<T>{}; }(F))::type;
            Instance* instance = static_cast<Instance*>(glfwGetWindowUserPointer(window));
            (instance->*(F))(std::forward<Ts>(ts)...);
        }

        template <auto F>
        inline void setCursorPosCallback()
        {
            if constexpr ( std::is_member_pointer_v<decltype(F)> )
            {
                glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
                    forward<F>(window, xPos, yPos);
                });
            }
            else
                glfwSetCursorPosCallback(window, F);
        }

        template <auto F>
        inline void setScrollCallback()
        {
            if constexpr ( std::is_member_pointer_v<decltype(F)> )
            {
                glfwSetScrollCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
                    forward<F>(window, xPos, yPos);
                });
            }
            else
                glfwSetCursorPosCallback(window, F);
        }

        template <auto F>
        inline void setFrameBufferSizeCallback()
        {
            if constexpr ( std::is_member_pointer_v<decltype(F)> )
            {
                glfwSetFrambufferSizeCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
                    forward<F>(window, xPos, yPos);
                });
            }
            else
                glfwSetFramebufferSizeCallback(window, F);
        }

        inline void show()
        {
            glfwShowWindow(window);
        }

        inline bool shouldClose()
        {
            return glfwWindowShouldClose(window) != 0;
        }

        inline void swapBuffers()
        {
            return glfwSwapBuffers(window);
        }

        inline int getKey(int key)
        {
            return glfwGetKey(window, key);
        }

        inline void setWindowShouldClose(bool shouldClose)
        {
            glfwSetWindowShouldClose(window, int(shouldClose));
        }

        inline void pollEvents()
        {
            glfwPollEvents();
        }
    };
}
