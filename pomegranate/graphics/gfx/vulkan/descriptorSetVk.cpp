#include "pch.hpp"

#include "descriptorSetVk.hpp"

#include "bufferVk.hpp"
#include "gfxVk.hpp"
#include "pipelineLayoutVk.hpp"
#include "textureVk.hpp"

namespace pom::gfx {
    DescriptorSetVk::DescriptorSetVk(InstanceVk* instance, Ref<PipelineLayoutVk> pipelineLayout, u32 set) :
        instance(instance), layout(std::move(pipelineLayout))
    {
        POM_PROFILE_FUNCTION();
        // VkDescriptorSetLayout layouts[] = { layout->descriptorSetLayouts[set], layout->descriptorSetLayouts[set] };
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = layout->pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &layout->descriptorSetLayouts[set],
        };

        POM_CHECK_VK(vkAllocateDescriptorSets(instance->getVkDevice(), &descriptorSetAllocateInfo, &descriptorSet),
                     "Failed to allocate descriptor sets");
    }

    DescriptorSetVk::~DescriptorSetVk()
    {
        POM_PROFILE_FUNCTION();
        vkFreeDescriptorSets(instance->getVkDevice(), layout->pool, 1, &descriptorSet);
    }

    void DescriptorSetVk::setBuffer(u32 binding, const Ref<Buffer>& buffer, u32 offset, u32 size)
    {
        POM_PROFILE_FUNCTION();
        VkDescriptorBufferInfo bufferInfo = {
            .buffer = buffer.dynCast<BufferVk>()->getBuffer(),
            .offset = offset,
            .range = size == 0 ? buffer->getSize() : size,
        };

        VkWriteDescriptorSet descriptorSetWrite = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = descriptorSet,
            .dstBinding = binding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = nullptr,
            .pBufferInfo = &bufferInfo,
            .pTexelBufferView = nullptr,
        };

        vkUpdateDescriptorSets(instance->getVkDevice(), 1, &descriptorSetWrite, 0, nullptr);
    }

    void DescriptorSetVk::setTexture(u32 binding, const Ref<Texture>& texture)
    {
        POM_PROFILE_FUNCTION();
        Ref<TextureVk> tex = texture.dynCast<TextureVk>();

        VkDescriptorImageInfo imageInfo = {
            .sampler = tex->getVkSampler(),
            .imageView = tex->getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        VkWriteDescriptorSet descriptorSetWrite = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = descriptorSet,
            .dstBinding = binding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &imageInfo,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr,
        };

        vkUpdateDescriptorSets(instance->getVkDevice(), 1, &descriptorSetWrite, 0, nullptr);
    }
} // namespace pom::gfx