#include <vermilion/vermilion.hpp>
#include <vector>

using namespace Vermilion::Core;

int main(int argc, char ** argv){
    try{

        // Create Vermilion instance
        VmInstance instance(nullptr, nullptr, nullptr, WindowCallbackFunctions{});

        // Load texture
        VmTexture texture;
        size_t width, height, channels;
        unsigned char * pixels = loadTextureData("../assets/texture1.jpg", &width, &height, &channels);
        instance.createTexture(texture, width, height, channels);
        texture.setData(pixels);
        freeTextureData(pixels);
        // Create texture sampler
        VmSampler sampler;
        instance.createSampler(sampler, texture);

        // Get default render target (screen)
        VmRenderTarget renderTarget;
        instance.getDefaultRenderTarget(renderTarget);

        // Create shaders
        VmShader vshader, fshader;
        VmShaderProgram program;
        instance.createShader(vshader, R"(
            #version 450
            #extension GL_ARB_separate_shader_objects : enable

            layout(location=0) in vec3 aPos;
            layout(location=1) in vec2 aUV;

            layout(location=0) out vec2 fUV;

            void main(){
                gl_Position = vec4(aPos, 1.0);
                fUV = aUV;
            }
        )", ShaderType::SHADER_TYPE_VERTEX);
        instance.createShader(fshader, R"(
            #version 450
            #extension GL_ARB_separate_shader_objects : enable

            layout(location=0) in vec2 aUV;

            layout(location=0) out vec4 outColor;

            layout(binding=0) uniform sampler2D uTex;

            void main(){
                outColor = texture(uTex, aUV);
            }
        )", ShaderType::SHADER_TYPE_FRAGMENT);
        instance.createShaderProgram(program, {&vshader, &fshader});

        // Create pipeline layout
        VmPipelineLayout layout;
        instance.createPipelineLayout(layout, {
            BufferLayoutElementFloat3("aPos"),
            BufferLayoutElementFloat2("aUV")
        }, {
            PipelineLayoutBinding(PipelineLayoutBindingType::PIPELINE_LAYOUT_BINDING_SAMPLER)
        }, {});
        // Create pipeline with layout
        VmPipeline pipeline;
        instance.createPipeline(pipeline, renderTarget, program, layout, PipelineSettings{
            PipelineSettingsDepthTest::PIPELINE_SETTINGS_DEPTH_TEST_DISABLED,
            PipelineSettingsCullMode::PIPELINE_SETTINGS_CULL_MODE_NONE,
            PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE
        });

        // Create triangle mesh
        VmBuffer vbuffer, ibuffer;
        VmRenderable mesh;
        std::vector<float> vertices = {
            -1.0,   -1.0,   0.0,        0.0,    0.0,
            0.0,    1.0,    0.0,        0.5,    1.0,
            1.0,    -1.0,   0.0,        1.0,    0.0,
        };
        std::vector<unsigned int> indices = {
            0, 1, 2,
        };
        instance.createBuffer(vbuffer, vertices.size()*sizeof(float), BufferType::BUFFER_TYPE_VERTEX, BufferUsage::BUFFER_USAGE_WRITE_ONLY, BufferDataUsage::BUFFER_DATA_USAGE_STATIC);
        instance.createBuffer(ibuffer, vertices.size()*sizeof(unsigned int), BufferType::BUFFER_TYPE_INDEX, BufferUsage::BUFFER_USAGE_WRITE_ONLY, BufferDataUsage::BUFFER_DATA_USAGE_STATIC);
        vbuffer.setData(vertices.data());
        ibuffer.setData(indices.data());
        instance.createRenderable(mesh, vbuffer, ibuffer, 0, 0, indices.size());

        // Create binding
        VmBinding binding;
        instance.createBinding(binding, {}, {&sampler});

        while(instance.shouldClose()){
            instance.startRender();

            renderTarget.start(0.2, 0.2, 0.2, 1.0);
            renderTarget.draw(pipeline, binding, mesh);
            renderTarget.end();

            instance.endRender({});
        }


    }catch(std::exception& ex){
    	return 1;
    }
    return 0;
}