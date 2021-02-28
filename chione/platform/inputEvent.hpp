#pragma once

#include "maths/maths.hpp"

#include "input.hpp"

namespace ce {
    class Window;

    /// @addtogroup platform
    /// @{

    /// @addtogroup input_events Input Events
    /// @{

    /// The type of a given InputEvent, each of these is an event that can be emitted by a window.
    enum class InputEventType : u8 {
        NONE = 0, ///< None event, this is not used.

        // TODO: differentiate between close and close requested?
        WINDOW_CLOSE, ///< Fires when a window is closed.
        WINDOW_MOVE, ///< Fires when a window is moved, only makes sense on platforms that can do that.
        WINDOW_RESIZE, ///< Fires when a window is resized, only makes sense on platforms that can do that.
        WINDOW_FOCUS, ///< Fires when a window is focused, that is clicked onto.
        WINDOW_BLUR, ///< Fires when a window loses focus, that is clicked away from.

        MOUSE_MOVE, ///< Fires when the mouse moves over the window, x/y are from the top-left corner.
        MOUSE_DOWN, ///< Fires when a mouse button is pressed down.
        MOUSE_UP, ///< Fires when a pressed mouse button is released.
        MOUSE_SCROLL, ///< Fires when the user scrolls, contains both x and y information.

        KEY_DOWN, ///< Fires when the user presses a key down. Is only propagated to the focused window.
        KEY_UP, ///< Fires when the user releases a held key. Is only propagated to the focused window.

        // TODO: key type, drag/drop events, touch events, etc.
    };

    /// @brief Contains all data for an input event. This includes the type of the event, and a pointer to the source
    /// window, and the event data.
    /// @warning This uses the tagged union based on `type`. Attempting to access the wrong event data is undefined
    /// behavior in c++. It is recommended to use the helper functions which do checking. However, if you do not want
    /// that checking, then the felids are public.
    struct InputEvent {

        /// @private
        struct None {
        };

        /// Data concerning the window close event. An empty type. Not intended to be used alone.
        struct WindowClose {
        };

        /// Data concerning the window move event. Not intended to be used alone.
        struct WindowMove {
            /// The new position of the window.
            maths::ivec2 position;
        };

        /// Data concerning the window resize event. Not intended to be used alone.
        struct WindowResize {
            /// The new size of the window.
            maths::ivec2 size;
        };

        /// Data concerning the window focus event. An empty type. Not intended to be used alone.
        struct WindowFocus {
        };

        /// Data concerning the window blur event. An empty type. Not intended to be used alone.
        struct WindowBlur {
        };

        /// Data concerning the mouse move event. Not intended to be used alone.
        struct MouseMove {
            /// The new position of the mouse from the top left corner of the window (not including the titlebar).
            maths::ivec2 position;
        };

        /// Data concerning the mouse down event. Not intended to be used alone.
        struct MouseDown {
            /// mouse button pressed.
            MouseButton button;
            /// Repeat count for the event, this occurs when a user holds the button down.
            u8 repeatCount;
        };

        /// Data concerning the mouse up event. Not intended to be used alone.
        struct MouseUp {
            /// mouse button pressed.
            MouseButton button;
            /// Repeat count for the event, this occurs when a user holds the button down.
            u8 repeatCount;
        };

        /// Data concerning the mouse scroll event. Not intended to be used alone.
        struct MouseScroll {
            /// The amount scrolled in a given direction. Up and left are positive, down and right are negative,
            maths::ivec2 delta;
        };

        /// Data concerning the key down event. Not intended to be used alone
        struct KeyDown {
            /// Hardware ID of the key pressed.
            KeyHid hid;

            /// Keycode of the key pressed
            Keycode code;

            /// True if the key is repeated.
            bool repeated;
        };

        /// Data concerning the key up event. Not intended to be used alone
        struct KeyUp {
            /// Hardware ID of the key pressed.
            KeyHid hid;

            /// Keycode of the key pressed
            Keycode code;

            /// True if the key is repeated.
            bool repeated;
        };

        /// The type of the input event. This determines the tagged union's correct value.
        const InputEventType type;

        /// The origin Window of this event.
        Window* sourceWindow;

        // FIXME: Doxyfile:942 excludes this union, this is dumb and probably will break at some point.
        union {
            WindowClose windowCloseData;
            WindowMove windowMoveData;
            WindowResize windowResizeData;
            WindowFocus windowFocusData;
            WindowBlur windowBlurData;
            MouseMove mouseMoveData;
            MouseDown mouseDownData;
            MouseUp mouseUpData;
            MouseScroll mouseScrollData;
            KeyDown keyDownData;
            KeyUp keyUpData;
        };

        /// Prints a trace message of an event, and its associated data.
        void debugPrint() const;

        /// Returns position data, only works when the event type is `WINDOW_MOVE`. If the type is not valid
        /// this will assert.
        const maths::ivec2& getPosition() const;

        /// Returns position data, only works when the event type is `WINDOW_RESIZE`. If the type is not valid
        /// this will assert.
        const maths::ivec2& getSize() const;
    };
    /// @}
    /// @}

    std::ostream& operator<<(std::ostream& os, const InputEventType& type);

} // namespace ce