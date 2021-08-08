#include "pch.hpp"

#include "bufferVk.hpp"

#include "commandBufferVk.hpp"
#include "typesVk.hpp"

namespace pom::gfx {
    BufferVk::BufferVk(InstanceVk* instance,
                       BufferUsage usage,
                       BufferMemoryAccess access,
                       size_t size,
                       const void* initialData,
                       size_t initialDataOffset,
                       size_t initialDataSize) :
        Buffer(usage, access, size),
        instance(instance)
    {
        if (access == BufferMemoryAccess::GPU_ONLY) {
            usage |= BufferUsage::TRANSFER_DST;
        }

        VkBufferCreateInfo vertexBufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = size,
            .usage = toVkBufferUsageFlags(usage),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE, // FIXME: there is probably an error here esp when multithreading
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
        };

        POM_ASSERT(vkCreateBuffer(instance->device, &vertexBufferCreateInfo, nullptr, &buffer) == VK_SUCCESS,
                   "Failed to create buffer");

        VkMemoryRequirements memoryReqs;
        vkGetBufferMemoryRequirements(instance->device, buffer, &memoryReqs);

        // TODO: currently a block of memory is allocated per buffer. This is bad and I definitely need to write a
        // custom memory allocator for this prob the CPU side too tbh

        VkPhysicalDeviceMemoryProperties physicalMemoryProps;
        vkGetPhysicalDeviceMemoryProperties(instance->physicalDevice, &physicalMemoryProps);

        VkMemoryPropertyFlags props = access == BufferMemoryAccess::CPU_WRITE
                                          ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                                          : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        // VK_MAX_MEMORY_TYPES will always be an invalid index.
        u32 memoryTypeIndex = VK_MAX_MEMORY_TYPES;

        for (u32 i = 0; i < physicalMemoryProps.memoryTypeCount; i++) {
            if (memoryReqs.memoryTypeBits & (1 << i)
                && (physicalMemoryProps.memoryTypes[i].propertyFlags & props) == props) {
                memoryTypeIndex = i;
                break;
            }
        }

        POM_ASSERT(memoryTypeIndex != VK_MAX_MEMORY_TYPES, "failed to find suitable memory type.");

        VkMemoryAllocateInfo memoryAllocInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memoryReqs.size,
            .memoryTypeIndex = memoryTypeIndex,
        };

        POM_ASSERT(vkAllocateMemory(instance->device, &memoryAllocInfo, nullptr, &memory) == VK_SUCCESS,
                   "Failed to allocate vertex buffer memory.")

        vkBindBufferMemory(instance->device, buffer, memory, 0);

        if (access == BufferMemoryAccess::GPU_ONLY) {
            auto* transferBuffer = new BufferVk(instance,
                                                BufferUsage::TRANSFER_SRC,
                                                BufferMemoryAccess::CPU_WRITE,
                                                size,
                                                initialData,
                                                initialDataOffset,
                                                initialDataSize);

            auto* commandBuffer = new CommandBufferVk(instance, CommandBufferSpecialization::TRANSFER, 1);
            commandBuffer->begin();
            commandBuffer->copyBuffer(transferBuffer, this, size, 0, 0);
            commandBuffer->end();
            commandBuffer->submit();

            delete commandBuffer;
            delete transferBuffer;
        } else {
            if (initialData) {
                void* data = map(initialDataOffset, initialDataSize ? initialDataSize : size);
                memcpy(data, initialData, initialDataSize ? initialDataSize : size);
                unmap();
            }
        }
    }

    BufferVk::~BufferVk()
    {
        vkDestroyBuffer(instance->device, buffer, nullptr);
        vkFreeMemory(instance->device, memory, nullptr);
    }

    [[nodiscard]] void* BufferVk::map(size_t offset, size_t size)
    {
        void* data = nullptr;
        vkMapMemory(instance->device, memory, offset, size == 0 ? VK_WHOLE_SIZE : size, 0, &data);
        return data;
    }

    void BufferVk::unmap()
    {
        vkUnmapMemory(instance->device, memory);
    }

} // namespace pom::gfx