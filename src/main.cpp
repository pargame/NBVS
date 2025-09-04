#include <stdio.h>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "imnodes.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.3 Core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 800, "NBVS - Node Based Visual Scripting", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    // Init GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "GLEW error: %s\n", glewGetErrorString(err));
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // ImNodes
    ImNodes::CreateContext();

    // Simple demo graph data
    int start_id = 1;
    int end_id = 2;
    int value = 42;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // DockSpace fullscreen
        {
            static bool dockspace_open = true;
            static bool opt_fullscreen = true;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                const ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }

            ImGui::Begin("NBVS DockSpace", &dockspace_open, window_flags);
            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            ImGuiID dockspace_id = ImGui::GetID("NBVSDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            ImGui::End();
        }

        // Menu
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Quit", "Cmd+Q"))
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Node editor window
        ImGui::Begin("Graph");
    ImNodes::BeginNodeEditor();

        // Start Node
    ImNodes::BeginNode(start_id);
            ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("Start");
            ImNodes::EndNodeTitleBar();
            ImNodes::BeginOutputAttribute(start_id * 10 + 1);
                ImGui::Text("Out");
            ImNodes::EndOutputAttribute();
    ImNodes::EndNode();

        // Value Node
    ImNodes::BeginNode(end_id);
            ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("Print");
            ImNodes::EndNodeTitleBar();
            ImNodes::BeginInputAttribute(end_id * 10 + 1);
                ImGui::Text("In");
            ImNodes::EndInputAttribute();
    ImNodes::EndNode();

        // Link
    ImNodes::Link(100, start_id * 10 + 1, end_id * 10 + 1);

    ImNodes::EndNodeEditor();
        ImGui::End();

        // Example property panel
        ImGui::Begin("Inspector");
        ImGui::Text("Value: %d", value);
        ImGui::SliderInt("##value", &value, 0, 100);
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImNodes::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
