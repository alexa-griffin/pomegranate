#pragma once

#include "base.hpp"

#include "gfx.hpp"

namespace pom::gfx {

    class RenderPass;
    class Shader;

    /// @addtogroup gfx
    /// @{

    /// Determines the primitives topology for a graphics pipeline.
    enum class PrimitiveTopology {
        POINTS, ///< Vertices should be drawn as points in space.
        LINES, ///< Verticies should be drawn as separate lines.
        LINE_STRIP, ///< Vertices should be drawn as lines with consecutive lines sharing vertices.
        TRIANGLES, ///< Verticies should be drawn as separate triangles.
        TRIANGLE_STRIP, ///< Verticies should be drawn as triangles with consecutive triangles sharing edges.
        TRIANGLE_FAN, ///< Verticies should be drawn as triangles with all triangles sharing the first vertex.
    };

    /// @brief Determines the cull mode for a graphics pipeline.
    /// @note Front and back faces are determined by the winding order of the face.
    /// @see GraphicsPipelineState::frontIsClockwise
    enum class CullMode {
        NONE, ///< No faces should be culled.
        FRONT, ///< Front faces should be culled.
        BACK, ///< Back faces should be culled.
        BOTH, ///< Both front and back faces should be culled.
    };

    /// Determines the state for a graphics pipeline. Provides sensable defaults for all values.
    struct GraphicsPipelineState {
        /// Determines the primitive topology for a graphics pipeline. Defaults to `PrimitiveTopology::TRIANGLES`
        PrimitiveTopology topology = PrimitiveTopology::TRIANGLES;
        /// If set to true primitive restart will be enabled, the primitive restart value for an drawIndexed call with
        /// `IndexType::U16` is `0xFFFF`, for `IndexType::U32` it is `0xFFFFFFFF`. Defaults to off.
        bool primativeRestart = false;
        /// Determines the cull mode for a draw.
        CullMode cullMode = CullMode::BACK;
        /// Determines which face is the front face. Defaults to counter-clockwise being the front.
        bool frontIsClockwise = false;
        // TODO: blend modes, prob a dozen other things
    };

    /// A single vertex attribute.
    struct VertexAttribute {
        /// The location of this attribute.
        u32 location;
        /// The format of this attribute.
        Format format;
        /// The offset in bytes of this attribute from the beginning of a vertex. If not specified defaults to the sum
        /// of the size of previous `VertexAttribute::format`s in the VertexBinding.
        i32 offset = -1;
    };

    /// Used for the construction of graphics pipelines. Specifies data needed for a vertex binding.
    struct VertexBinding {
        /// The bindpoint of ths vertex binding.
        u32 binding;
        /// Specifies the stride in bytes of a vertex. If not specified defaults to the sum of the size of the
        /// `VertexAttribute::format`s in `attribs`.
        i32 stride = -1;
        /// A list of all vertex attributes belonging to this binding.
        std::vector<VertexAttribute> attribs;
    };

    // TODO: compute pipelines

    /// A resource containing all data for a given graphics pipeline.
    class POM_API Pipeline {
    public:
        /// Returns the GraphicsAPI associated with this pipeline.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        [[nodiscard]] static Pipeline* create(GraphicsPipelineState state,
                                              Shader* shader,
                                              RenderPass* renderPass,
                                              std::initializer_list<VertexBinding> vertexBindings,
                                              VkPipelineLayout pipelineLayout); // FIXME: api independence
        virtual ~Pipeline() = default;

    protected:
    };

    ///@}

} // namespace pom::gfx