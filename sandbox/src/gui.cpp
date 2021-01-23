#include "gui.hpp"
#include <vermilion/glm/glm.hpp>
#include <vermilion/glm/gtc/matrix_transform.hpp>

struct UniformBufferType{
    glm::mat4 proj;
};

GUI::GUI(std::shared_ptr<VmInstance> instance, int width, int height){
    this->instance = instance;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.BackendPlatformName = "Vermilion";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    io.DisplaySize = ImVec2((float)width, (float)height);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    io.WantCaptureMouse = true;


    // Get texture
    unsigned char * pixels;
    int Twidth, Theight;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &Twidth, &Theight);

    instance->getDefaultRenderTarget(renderTarget);
    instance->createTexture(texture, Twidth, Theight, 4);
    texture.setData(pixels);
    instance->createSampler(sampler, texture);

    instance->createShader(vertexShader, R"(
			#version 450
			#extension GL_ARB_separate_shader_objects : enable
			layout(location = 0) in vec2 aPos;
			layout(location = 1) in vec2 aTexCoord;
			layout(location = 2) in vec4 aColor;
			
			layout(location = 0) out vec4 fColor;
			layout(location = 1) out vec2 fTexCoord;

			layout(binding=0) uniform UniformBufferObject{
				mat4 proj;
			} ubo;
			
			void main() {
				gl_Position = ubo.proj * vec4(aPos, 0.0, 1.0);

                #ifdef VULKAN
                    fColor = aColor;
                #else
				    fColor = aColor/256;
                #endif

				fTexCoord = aTexCoord;
			}
    )", Vermilion::Core::ShaderType::SHADER_TYPE_VERTEX);
    instance->createShader(fragmentShader, R"(
			#version 450
			#extension GL_ARB_separate_shader_objects : enable

			layout(location = 0) in vec4 fColor;
			layout(location = 1) in vec2 fTexCoord;

			layout(location = 0) out vec4 outColor;

			layout(binding = 1) uniform sampler2D s_tex;

			void main() {
				outColor = texture(s_tex, fTexCoord) * fColor;
			}
    )", Vermilion::Core::ShaderType::SHADER_TYPE_FRAGMENT);
    instance->createShaderProgram(shaderProgram, {&vertexShader, &fragmentShader});

    instance->createPipelineLayout(pipelineLayout, {
            Vermilion::Core::BufferLayoutElementFloat2("Position"),
            Vermilion::Core::BufferLayoutElementFloat2("UV"),
            Vermilion::Core::BufferLayoutElementByte4("Color"),
        }, {
            Vermilion::Core::PipelineLayoutBinding(Vermilion::Core::PipelineLayoutBindingType::PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER, 0, 0),
            Vermilion::Core::PipelineLayoutBinding(Vermilion::Core::PipelineLayoutBindingType::PIPELINE_LAYOUT_BINDING_SAMPLER)
        },{
    });

    instance->createPipeline(pipeline, renderTarget, shaderProgram, pipelineLayout, {
            Vermilion::Core::PipelineSettingsDepthTest::PIPELINE_SETTINGS_DEPTH_TEST_DISABLED,
            Vermilion::Core::PipelineSettingsCullMode::PIPELINE_SETTINGS_CULL_MODE_NONE,
            Vermilion::Core::PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE
    });

    instance->createBuffer(uniformBuffer, sizeof(UniformBufferType),
        Vermilion::Core::BufferType::BUFFER_TYPE_UNIFORM,
        Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY,
        Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_DYNAMIC
    );
    instance->createBuffer(vertexBuffer, 1024*1024, 
        Vermilion::Core::BufferType::BUFFER_TYPE_VERTEX,
        Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY,
        Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_DYNAMIC
    );
    instance->createBuffer(indexBuffer, 1024*1024,
        Vermilion::Core::BufferType::BUFFER_TYPE_INDEX,
        Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY,
        Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_DYNAMIC   
    );
    instance->createRenderable(renderable, vertexBuffer, indexBuffer, 0, 0, 0);

    instance->createBinding(binding, {&uniformBuffer}, {&sampler});

    m_time = std::chrono::high_resolution_clock::now();
    ImGui::NewFrame();
}

GUI::~GUI(){
    ImGui::DestroyContext();
}

void GUI::render(){
    ImGui::Render();

    ImDrawData * draw_data = ImGui::GetDrawData();

    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    ImVec2 clip_off = ImVec2(0,0);         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)
    
    // Set projection
    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
    UniformBufferType ubo;
    ubo.proj = glm::ortho(L, R, B, T);
    uniformBuffer.setData(&ubo);

    for(int i=0; i<draw_data->CmdListsCount; i++){
        const ImDrawList * cmd_list = draw_data->CmdLists[i];
        
        vertexBuffer.setData(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        indexBuffer.setData(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
    
        for(int j=0; j<cmd_list->CmdBuffer.Size; j++){
            const ImDrawCmd * pcmd = &cmd_list->CmdBuffer[j];
            if(pcmd->UserCallback!=NULL){
                // User callback registered
            }else{
                ImVec4 clip_rect;
                clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;
                if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f){
                    if (clip_rect.x < 0.0f)
                        clip_rect.x = 0.0f;
                    if (clip_rect.y < 0.0f)
                        clip_rect.y = 0.0f;
                    pipeline.setScissor(clip_rect.z-clip_rect.x, clip_rect.w-clip_rect.y, clip_rect.x, clip_rect.y);
                    renderable.indexOffset(pcmd->IdxOffset);
                    renderable.vertexOffset(pcmd->VtxOffset);
                    renderable.length(pcmd->ElemCount);
                    renderTarget.draw(pipeline, binding, renderable);
                }
            }
        }
    }

    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = current_time - m_time;
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = time_span.count();
    m_time = current_time;
    ImGui::NewFrame();
}

void GUI::resize(int width, int height){
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)width, (float)height);
    this->pipeline.setViewport(width, height, 0, 0);
    this->pipeline.setScissor(width, height, 0, 0);
}

void GUI::mouseButton(Vermilion::Core::WindowMouseButton btn, Vermilion::Core::WindowMouseAction act){
    ImGuiIO& io = ImGui::GetIO();
    int button = ImGuiMouseButton_Left;
    if(act==Vermilion::Core::WindowMouseAction::WINDOW_MOUSE_ACTION_PRESS)
        io.MouseDown[button] = true;
    else
        io.MouseDown[button] = false;
}

void GUI::mousePos(double x, double y){
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
}

void GUI::scroll(double x, double y){
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = y;
}