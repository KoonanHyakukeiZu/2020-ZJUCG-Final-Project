#ifndef SHADOW_H
#define SHADOW_H

#include <glad/glad.h>
#include <light.h>

namespace KooNan
{
	class Shadow_Frame_Buffer
	{
	private:
		unsigned int shadow_fbo;
		unsigned int shadow_map;
	public:
		const unsigned int SHADOW_WIDTH = 4096;
		const unsigned int SHADOW_HEIGHT = 4096;
		glm::mat4 lightProjection;
		glm::mat4 lightView;
	public:
		Shadow_Frame_Buffer()
		{
			FBOInit();
		}
		~Shadow_Frame_Buffer()
		{

		}
		void bindFrameBuffer()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadow_fbo);
		}
		void unbindFrameBuffer()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}
		unsigned int getShadowTexture()
		{
			return shadow_map;
		}
	private:
		void FBOInit()
		{
			glGenFramebuffers(1, &shadow_fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
			glGenTextures(1, &shadow_map);
			glBindTexture(GL_TEXTURE_2D, shadow_map);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	};

}


#endif // !SHADOW_H
