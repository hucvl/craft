/*
* Copyright (c) 2006-2016 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include "Testbed/glad/glad.h"
#endif

#include "SimulationDefines.h"
#include "Testbed/imgui/imgui.h"
#include "Testbed/imgui/imgui_impl_glfw_gl3.h"
#include "Camera.hpp"
#include "Simulation.h"

#include "Testbed/glfw/glfw3.h"
#include <stdio.h>

#include "ControllerParser.h"
#include <time.h>

#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h> 
#endif

//
namespace
{
	GLFWwindow* mainWindow = NULL;
}

//
static void sCreateUI(GLFWwindow* window)
{
	// Init UI
	const char* fontPath = "Data/DroidSans.ttf";
	ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath, 15.f);

	if (ImGui_ImplGlfwGL3_Init(window, false) == false)
	{
		fprintf(stderr, "Could not init GUI renderer.\n");
		assert(false);
		return;
	}

	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = style.GrabRounding = style.ScrollbarRounding = 2.0f;
	style.FramePadding = ImVec2(4, 2);
	style.DisplayWindowPadding = ImVec2(0, 0);
	style.DisplaySafeAreaPadding = ImVec2(0, 0);
}

//
static void sResizeWindow(GLFWwindow*, int width, int height)
{
	g_camera.m_width = width;
	g_camera.m_height = height;
}

//
static void sSimulate(Simulation* simulation, SettingsBase* settings)
{
	glEnable(GL_DEPTH_TEST);
	simulation->Step(settings);
	glDisable(GL_DEPTH_TEST);
}

//

//
void glfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "GLFW error occured. Code: %d. Description: %s\n", error, description);
}

void renderLoop(Simulation* simulation, SettingsBase* settings)
{
	while (!glfwWindowShouldClose(mainWindow))
	{
		glViewport(0, 0, settings->bufferWidth, settings->bufferHeight);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)g_camera.m_width, (float)g_camera.m_height));
		ImGui::Begin("Overlay", NULL, ImVec2(0, 0), 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
		ImGui::SetCursorPos(ImVec2(5, (float)g_camera.m_height - 20));
		ImGui::End();

		sSimulate(simulation, settings);

		glfwSwapBuffers(mainWindow);

		glfwPollEvents();
	}
}

int main(int c, char** args)
{
#ifdef _MSC_VER
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	// To produce random numbers rather than getting same numbers on every run.
	srand(time(NULL) + 42);

	std::string controllerJSONPath = args[1];
	const svqa::SimulationBase::Ptr& simulation = svqa::parse(controllerJSONPath);
	const auto& settings = simulation->getSettings();

	glfwSetErrorCallback(glfwErrorCallback);

	g_camera.m_width = settings->bufferWidth;
	g_camera.m_height = settings->bufferHeight;
	//g_camera.m_zoom = 1.3f;


	if (glfwInit() == 0)
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	char title[64];
	sprintf(title, "Box2D Testbed Version %d.%d.%d", b2_version.major, b2_version.minor, b2_version.revision);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 16); 
	if (settings->offline) {
		// Do not show and focus window.
		glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	}

#ifdef _MSC_VER
	mainWindow = glfwCreateWindow(g_camera.m_width, g_camera.m_height, title, NULL, NULL);
#else 
	mainWindow = glfwCreateWindow(g_camera.m_width / 2, g_camera.m_height / 2, title, NULL, NULL);
#endif
	if (mainWindow == NULL)
	{
		fprintf(stderr, "Failed to open GLFW mainWindow.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(mainWindow);

#ifdef _MSC_VER
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to load OpenGL and its extensions" << std::endl;
		return EXIT_FAILURE;
	}
#endif


#ifndef _MSC_VER
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif

	g_debugDraw.Create();

	sCreateUI(mainWindow);

	// Control the frame rate. One draw per monitor refresh.
	glfwSwapInterval(1);

	glClearColor(1.0f, 1.0f, 1.0f, 1.f);
	renderLoop(simulation.get(), settings.get());

	g_debugDraw.Destroy();
	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate(); 

#ifdef _MSC_VER
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
