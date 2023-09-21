#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "tests/TestClearColor.h"

/* For debug */
int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(960, 540, "OpenGL Practice", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); // Vsync

	if (glewInit() != GLEW_OK) {
		std::cout << "Error!" << std::endl;
	}

	std::cout << glGetString(GL_VENDOR) << std::endl;
	{
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		


		Renderer renderer;

		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();
		
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		test::Test* currentTest = nullptr;
		test::TestMenu* testmenu = new test::TestMenu(currentTest);
		currentTest = testmenu;

		testmenu->RegisterTest<test::TestClearColor>("Clear Color");

		while (!glfwWindowShouldClose(window))
		{
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			renderer.Clear();		
					
			ImGui_ImplGlfwGL3_NewFrame();

			if (currentTest) {
				currentTest->OnUpdate(0.0f);
				currentTest->OnRender();
				ImGui::Begin("Test");
				if (currentTest != testmenu && ImGui::Button("<-"))
				{
					delete currentTest;
					currentTest = testmenu;
				}
				currentTest->OnImGuiRender();
				ImGui::End();
			}

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::Text("%s", glGetString(GL_VENDOR));
			ImGui::NewLine();
			ImGui::Button("Update Drive");
			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("New")) {
					// Do sth
					if (ImGui::MenuItem("Create")) {
						// Do sth
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit")) {
					// Do sth
					if (ImGui::MenuItem("Copy")) {
						// Do sth
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("View")) {
					// Do sth
					if (ImGui::MenuItem("Open")) {
						// Do sth
					}
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);

			glfwPollEvents();
		}
		delete currentTest;
		if (currentTest != testmenu) {
			delete testmenu;
		}
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}
