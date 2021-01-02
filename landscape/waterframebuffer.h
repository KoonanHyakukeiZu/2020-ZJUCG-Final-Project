#ifndef WATER_FRAME_BUFFER_H
#define WATER_FRAME_BUFFER_H

#include <glad/glad.h>

#include <GameController.h>


namespace KooNan
{
	class Water_Frame_Buffer
	{
	private:
		const int REFLECTION_WIDTH = 1280;
		const int REFLECTION_HEIGHT = 720;
		const int REFRACTION_WIDTH = 1280;
		const int REFRACTION_HEIGHT = 720;
		unsigned int reflectionFrameBuffer;
		unsigned int reflectionTexture;
		unsigned int reflectionDepthBuffer;

		unsigned int refractionFrameBuffer;
		unsigned int refractionTexture;
		unsigned int refractionDepthTexture;

	public:
		Water_Frame_Buffer()
		{
			initialiseReflectionFrameBuffer();
			initialiseRefractionFrameBuffer();
		}
		void bindReflectionFrameBuffer() {//call before rendering to this FBO
			bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
		}

		void bindRefractionFrameBuffer() {//call before rendering to this FBO
			bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
		}

		void unbindCurrentFrameBuffer() {//call to switch to default frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, (float)GameController::SCR_WIDTH, (float)GameController::SCR_HEIGHT);
		}
		unsigned int getReflectionTexture() {//get the resulting texture
			return reflectionTexture;
		}

		unsigned int getRefractionTexture() {//get the resulting texture
			return refractionTexture;
		}

		unsigned int getRefractionDepthTexture() {//get the resulting depth texture
			return refractionDepthTexture;
		}

		void initialiseReflectionFrameBuffer() {
			reflectionFrameBuffer = createFrameBuffer();
			reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
			reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
			unbindCurrentFrameBuffer();
		}

		void initialiseRefractionFrameBuffer() {
			refractionFrameBuffer = createFrameBuffer();
			refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
			refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
			unbindCurrentFrameBuffer();
		}

		void cleanUp() {
			glDeleteFramebuffers(1, &reflectionFrameBuffer);
			glDeleteTextures(1, &reflectionTexture);
			glDeleteRenderbuffers(1, &reflectionDepthBuffer);
			glDeleteFramebuffers(1, &refractionFrameBuffer);
			glDeleteTextures(1, &refractionTexture);
			glDeleteTextures(1, &refractionDepthTexture);
		}

	private:
		void bindFrameBuffer(int frameBuffer, int width, int height) {
			glBindTexture(GL_TEXTURE_2D, 0);//To make sure the texture isn't bound
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glViewport(0, 0, width, height);
		}

		int createFrameBuffer()
		{
			unsigned int frameBuffer;
			glGenFramebuffers(1, &frameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			return frameBuffer;
		}
		unsigned int createTextureAttachment(int width, int height) {
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

			return texture;
		}

		unsigned int createDepthTextureAttachment(int width, int height) {
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, (void*)nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

			return texture;
		}

		unsigned int createDepthBufferAttachment(int width, int height) {
			unsigned int depthBuffer;
			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
			return depthBuffer;
		}
	};
}
#endif