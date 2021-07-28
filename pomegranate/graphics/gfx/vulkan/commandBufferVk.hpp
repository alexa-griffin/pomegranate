#pragma once

#include "base.hpp"

#include "../commandBuffer.hpp"

namespace pom::gfx {
    class InstanceVk;

    /// @addtogroup vulkan
    /// @{

    /// Vulkan CommandBuffer, should be created implicitly through `Context::createCommandBuffer`. Contains a list of
    /// vulkan command buffers and fences corresponding to the number of swapchain images.
    class POM_API CommandBufferVk final : public CommandBuffer {
    public:
        ~CommandBufferVk();

        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        void begin() final;

        void beginRenderPass(RenderPass* renderPass) final;

        void endRenderPass() final;

        void end() final;

        void setViewport(const maths::vec2& offset, const maths::vec2& extent, f32 mindepth, f32 maxdepth) final;

        void setScissor(const maths::ivec2& offset, const maths::uvec2& extent) final;

        void draw(u32 vertexCount, u32 vertexOffset = 0) final;

        void drawIndexed(u32 indexCount, u32 firstIndex = 0, i32 vertexOffset = 0) final;

        void bindVertexBuffer(Buffer* vertexBuffer, u32 bindPoint = 0, size_t offset = 0) final;

        void bindIndexBuffer(Buffer* indexBuffer, IndexType type, size_t offset = 0) final;

        [[nodiscard]] inline VkCommandBuffer& getCurrentCommandBuffer()
        {
            return commandBuffers[currentIndex];
        }

        [[nodiscard]] inline VkFence& getCurrentRecordingFence()
        {
            return recordingFences[currentIndex];
        }

    private:
        friend class ContextVk;

        CommandBufferVk(CommandBufferSpecialization specialization,
                        ContextVk* context,
                        InstanceVk* instance,
                        VkCommandPool pool,
                        u32 count);

        ContextVk* context;
        InstanceVk* instance;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkFence> recordingFences;
        u32 currentIndex;

        std::vector<VkBuffer> vertexBufferBindings;
    };

    /// @}
} // namespace pom::gfx