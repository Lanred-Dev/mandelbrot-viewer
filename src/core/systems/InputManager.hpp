#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include "core/Signal.h"

namespace Core
{
    namespace Systems
    {
        class InputManager
        {
        public:
            Signal<int, int> onKeyPressed;
            Signal<int, int> onKeyReleased;
            Signal<int, int> onKeyHeld;
            
            void Initialize(GLFWwindow *window);
            bool IsKeyPressed(int key);

        private:
            std::unordered_map<int, bool> pressedKeys;
            std::unordered_map<int, bool> heldKeys;
            std::unordered_map<int, float> lastKeyHoldTimes;

            static constexpr float KEY_HOLD_DEBOUNCE = 0.05f;

            static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
        };
    };
};