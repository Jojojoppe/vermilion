#pragma once

#include <vermilion/vermilion.hpp>
#include "imgui/imgui.h"
#include <memory>
#include <chrono>

class GUI{
    public:

        std::shared_ptr<VmInstance> instance;

        VmTexture texture;
        VmSampler sampler;

        VmBuffer vertexBuffer;
        VmBuffer indexBuffer;
        VmRenderable renderable;
        
        VmBuffer uniformBuffer;

        VmShader vertexShader;
        VmShader fragmentShader;
        VmShaderProgram shaderProgram;

        VmPipelineLayout pipelineLayout;
        VmPipeline pipeline;
        VmBinding binding;

        VmRenderTarget renderTarget;

        std::chrono::high_resolution_clock::time_point m_time;

        GUI(std::shared_ptr<VmInstance> instance, int width, int height);
        ~GUI();

        void render();
        void resize(int width, int height);
        void mouseButton(Vermilion::Core::WindowMouseButton btn, Vermilion::Core::WindowMouseAction act);
        void mousePos(double x, double y);

};