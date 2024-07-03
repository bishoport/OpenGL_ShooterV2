#pragma once

#include "../LibCoreHeaders.h"

namespace libCore
{

	class IBL {

    public:

        //IBL
        GLuint IBL_FBO = 0;
        GLuint IBL_RBO = 0;

        GLuint envCubemap = 0;
        GLuint irradianceMap = 0;
        GLuint prefilterMap = 0;
        GLuint brdfLUTTexture = 0;


        void prepareIBL(int screenWidth, int screenHeight, bool dynamic);

        void UpdateIBL();
        //----------------------------------------------------

        GLuint m_hdrTexture;

       

    private:

        int  m_screenWidth = 0;
        int  m_screenHeight = 0;
        bool m_dynamic = false;

        void configureStaticIBL();
        void configureDynamicIBL();

        GLuint cubeVAO = 0;
        GLuint cubeVBO = 0;
        GLuint quadVAO = 0;
        GLuint quadVBO = 0;

        void renderCube();
        void renderQuad();
    };
}