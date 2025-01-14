#pragma once

#include <utility>

#include "base.hpp"

#include "gfx.hpp"
#include "maths/vector.hpp"
#include "util/bitflags.hpp"

namespace pom::gfx {

#ifndef DOXYGEN // Hide this because technically it doesn't matter, the namespace is only really here so that the
                // members of the enum do not pollute the namespace.
    namespace TextureUsageNamespace {
#endif
        enum TextureUsage {
            SAMPLED = 1 << 0,
            STORAGE = 1 << 1,
            TRANSFER_SRC = 1 << 2,
            TRANSFER_DST = 1 << 3,
        };

#ifndef DOXYGEN
    } // namespace TextureUsageNamespace

    using TextureUsage = TextureUsageNamespace::TextureUsage;
#endif

    enum class TextureType {
        IMAGE_1D,
        IMAGE_2D,
        IMAGE_3D,
    };

    enum class TextureViewType {
        VIEW_1D,
        VIEW_2D,
        VIEW_3D,
        CUBE,
        VIEW_1D_ARRAY,
        VIEW_2D_ARRAY,
        CUBE_ARRAY,
    };

    struct TextureCreateInfo {
        TextureType type;
        TextureUsage usage;
        Format format;
        u32 mipLevels = 1;
        u32 arrayLayers = 1;
        bool cubeCompatable = false;
    };

    struct TextureSubresourceRange {
        u32 baseArrayLayer = 0;
        u32 layerCount = 1;
        u32 baseMip = 0;
        u32 mipCount = 1;
        // TODO: the rest of this
    };

    struct TextureViewCreateInfo {
        TextureViewType type;
        Format format;
        TextureSubresourceRange subresourceRange;
        // TODO: component mapping
    };

    class POM_API Texture {
    public:
        /// Returns the GraphicsAPI associated with this texture.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        [[nodiscard]] static Ref<Texture> create(TextureCreateInfo createInfo,
                                                 u32 width,
                                                 u32 height = 1u,
                                                 u32 depth = 1u,
                                                 const void* initialData = nullptr,
                                                 usize initialDataOffset = 0,
                                                 usize initialDataSize = 0);

        [[nodiscard]] static Ref<Texture> createDirect(TextureCreateInfo createInfo,
                                                       u32 width,
                                                       u32 height,
                                                       u32 depth,
                                                       usize initialDataSize,
                                                       const void* initialData);

        [[nodiscard]] inline TextureUsage getUsage() const
        {
            return createInfo.usage;
        }
        [[nodiscard]] inline u32 getWidth() const
        {
            return extent.x;
        }
        [[nodiscard]] inline u32 getHeight() const
        {
            return extent.y;
        }

        [[nodiscard]] inline u32 getDepth() const
        {
            return extent.z;
        }

        [[nodiscard]] inline u32 getMipCount() const
        {
            return createInfo.mipLevels;
        }

        [[nodiscard]] const maths::uvec3& getExtent() const
        {
            return extent;
        }

        [[nodiscard]] virtual inline usize getSize() const = 0;

        virtual ~Texture() = default;

        [[nodiscard]] virtual void* map(usize offset = 0, usize size = 0) = 0;
        virtual void unmap() = 0;

    protected:
        POM_NOCOPY(Texture);

        Texture(TextureCreateInfo createInfo, u32 width, u32 height, u32 depth);

        maths::uvec3 extent;

        TextureCreateInfo createInfo;
    };

    class POM_API TextureView {
    public:
        /// Returns the GraphicsAPI associated with this texture view.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        [[nodiscard]] static Ref<TextureView> create(const Ref<Texture>& texture, TextureViewCreateInfo createInfo);

        virtual ~TextureView() = default;

        [[nodiscard]] inline Format getFormat() const
        {
            return createInfo.format;
        }

    protected:
        POM_NOCOPY(TextureView);

        TextureView(TextureViewCreateInfo createInfo) : createInfo(createInfo)
        {
        }

        TextureViewCreateInfo createInfo;
    };
} // namespace pom::gfx