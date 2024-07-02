//#pragma once
//#include "../LibCoreHeaders.h"
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <functional>
//#include <iostream>
//
//namespace libCore
//{
//    class MousePicker
//    {
//    public:
//        static MousePicker& getInstance() {
//            static MousePicker instance;
//            return instance;
//        }
//
//        void initialize(GLFWwindow* window) {
//            this->window = window;
//        }
//
//        void setupStencilBuffer() {
//            glEnable(GL_DEPTH_TEST);
//            glEnable(GL_STENCIL_TEST);
//            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//            glStencilMask(0xFF);
//            glClearStencil(no_hover_index);
//        }
//
//        void handleMouseClick(double xpos, double ypos) {
//            if (!window) {
//                std::cerr << "Window not initialized in MousePicker." << std::endl;
//                return;
//            }
//
//            int width, height;
//            glfwGetFramebufferSize(window, &width, &height);
//            int mouseX = static_cast<int>(xpos);
//            int mouseY = height - static_cast<int>(ypos);
//
//            glReadPixels(mouseX, mouseY, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &hovered_index);
//        }
//
//        void pickObject(const std::function<void()>& renderObjects) {
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//
//            glStencilFunc(GL_ALWAYS, 1, 0xFF);
//            glStencilMask(0xFF);
//            renderObjects();
//        }
//
//        int getHoveredIndex() const {
//            return hovered_index;
//        }
//
//    private:
//        MousePicker() : window(nullptr), hovered_index(no_hover_index) {} // Constructor privado
//        GLFWwindow* window;
//        int hovered_index;
//        const int no_hover_index = 0;
//    };
//}
