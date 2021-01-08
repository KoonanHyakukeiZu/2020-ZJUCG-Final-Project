#ifndef PICKINGTEXTURE_H
#define PICKINGTEXTURE_H
#include <common.h>
#include <glad/glad.h>
namespace KooNan
{
	struct PixelInfo
	{
		float ObjID;
		float DrawID;
		float PriID;
	};

	class PickingTexture
	{
	public:
		
		PickingTexture() 
		{ 
			FBOInit();
		};
		bool FBOInit()
		{
			glGenFramebuffers(1, &m_fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
			glGenTextures(1, &m_pickingTexture);
			glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingTexture, 0);
			glGenTextures(1, &m_depthTexture);
			glBindTexture(GL_TEXTURE_2D, m_depthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
			glReadBuffer(GL_NONE);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (Status != GL_FRAMEBUFFER_COMPLETE) {
				printf("FB error, status: 0x%x\n", Status);
				return false;
			}
			//Restore to default
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return true;
		}
		void bindFrameBuffer()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
		}
		void unbindFrameBuffer()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}
		PixelInfo ReadPixel(unsigned int x, unsigned int y)
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT0);

			PixelInfo info;
			glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &info);

			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

			return info;
		}

	private:
		unsigned int m_fbo;
		unsigned int m_pickingTexture;
		unsigned int m_depthTexture;
	};

#endif // !PICKINGTEXTURE_H
}
