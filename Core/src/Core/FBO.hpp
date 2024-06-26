#pragma once

#include "../LibCoreHeaders.h"
#include <map>
#include <string>
#include <iostream>

namespace libCore {

    class FBO {
    public:

        std::string nameFBO = "";

        FBO() : framebuffer(0), fboWidth(0), fboHeight(0), internalFormat(GL_RGB8) {}

        ~FBO() {
            cleanup();
        }

        void init(int width, int height, GLenum internalFormat, std::string name) {
            nameFBO = name;
            fboWidth = width;
            fboHeight = height;
            this->internalFormat = internalFormat;
        }

        void addAttachment(const std::string& name, GLenum internalFormat, GLenum format, GLenum type) {
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, fboWidth, fboHeight, 0, format, type, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            attachments[name] = texture;
        }

        void closeSetup()
        {
            setupFBO();
            checkFBOStatus();
        }

        void bindFBO() const {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        }

        void unbindFBO() const {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void bindTexture(const std::string& attachmentType, int textureSlot) const {
            glActiveTexture(GL_TEXTURE0 + textureSlot);
            glBindTexture(GL_TEXTURE_2D, attachments.at(attachmentType));
        }

        GLuint getTexture(const std::string& attachmentType) const {
            return attachments.at(attachmentType);
        }

        GLuint getFramebuffer() const {
            return framebuffer;
        }

        int getWidth() const {
            return fboWidth;
        }

        int getHeight() const {
            return fboHeight;
        }

        void resize(int newWidth, int newHeight) {
            fboWidth = newWidth;
            fboHeight = newHeight;

            bindFBO();
            // Resize color attachment
            glBindTexture(GL_TEXTURE_2D, attachments["color"]);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, fboWidth, fboHeight, 0, GL_RGB, GL_FLOAT, nullptr);

            // Resize depth attachment
            glBindTexture(GL_TEXTURE_2D, attachments["depth"]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fboWidth, fboHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

            unbindFBO();
        }

    private:
        GLuint framebuffer;
        std::map<std::string, GLuint> attachments;
        int fboWidth, fboHeight;
        GLenum internalFormat;

        void setupFBO() {
            glGenFramebuffers(1, &framebuffer);
            bindFBO();
            setupAttachments();
        }

        void setupAttachments() {
            // Color attachment
            GLuint colorAttachment;
            glGenTextures(1, &colorAttachment);
            glBindTexture(GL_TEXTURE_2D, colorAttachment);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, fboWidth, fboHeight, 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);
            attachments["color"] = colorAttachment;

            // Depth attachment
            GLuint depthAttachment;
            glGenTextures(1, &depthAttachment);
            glBindTexture(GL_TEXTURE_2D, depthAttachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, fboWidth, fboHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);
            attachments["depth"] = depthAttachment;

            // Set draw buffers
            GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, drawBuffers);

            unbindFBO();
        }

        void cleanup() {
            for (auto& attachment : attachments) {
                glDeleteTextures(1, &attachment.second);
            }
            glDeleteFramebuffers(1, &framebuffer);
        }

        void checkFBOStatus() const {
            bindFBO();
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            }
            unbindFBO();
        }
    };
}
