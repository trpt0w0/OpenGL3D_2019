/**
* @file GLFWEW.h
*/
#ifndef GLFWEW_H_INCLUDED
#define GLFWEW_H_INCLUDED
#include <GL/glew.h>
#include "GamePad.h"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

namespace GLFWEW {

	/**
	* GLFW��GLEW�̃��b�p�[�N���X.
	*/
	class Window
	{
	public:
		static Window& Instance();
		bool Init(int w, int h, const char* title);
		bool ShouldClose() const;
		void SwapBuffers() const;
		void Update();

		void InitTimer();
		void UpdateTimer();
		double DeltaTime() const;
		double Time() const;
		int Width() const { return width; }
		int Height() const { return height; }


        bool IsKeyDown(int key) const;


		bool IsKeyPressed(int key) const;
        glm::vec2 GetMousePosition() const;
        int GetMouseButton(int button) const;
		const GamePad& GetGamePad() const;
		void UpdateGamePad();
		void UpdateDeltaTime();

	private:
		Window();
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;


		bool isGLFWInitialized = false;
		bool isInitialized = false;
		GLFWwindow* window = nullptr;
		int width = 0;
		int height = 0;
        double previousTime = 0;
        double deltaTime = 0;
		GamePad gamepad;

        enum class KeyState : char {
          release,
          press1st,
          press,
        };

        KeyState keyState[GLFW_KEY_LAST + 1];
	};

} // namespace GLFWEW

#endif // GLFWEW_H_INCLUDED
