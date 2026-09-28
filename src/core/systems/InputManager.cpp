#include "InputManager.hpp"

namespace Core
{
    namespace Systems
    {
        void InputManager::Initialize(GLFWwindow *window)
        {
            glfwSetWindowUserPointer(window, this);
            glfwSetKeyCallback(window, KeyCallback);
        }

        bool InputManager::IsKeyPressed(int key)
        {
            return pressedKeys[key];
        }

        void InputManager::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
        {
            InputManager *inputManager = static_cast<InputManager *>(glfwGetWindowUserPointer(window));
            float now = static_cast<float>(glfwGetTime());

            if (action == GLFW_PRESS)
            {
                inputManager->pressedKeys[key] = true;
                inputManager->heldKeys[key] = true;
                inputManager->lastKeyHoldTimes[key] = now;
                inputManager->onKeyPressed.Emit(key);
                inputManager->onKeyHeld.Emit(key);
            }
            else if (action == GLFW_REPEAT)
            {
                for (auto &[heldKey, isHeld] : inputManager->heldKeys)
                {
                    if (isHeld && now - inputManager->lastKeyHoldTimes[heldKey] > inputManager->KEY_HOLD_DEBOUNCE)
                    {
                        inputManager->lastKeyHoldTimes[heldKey] = now;
                        inputManager->onKeyHeld.Emit(heldKey);
                    }
                }
            }
            else if (action == GLFW_RELEASE)
            {
                inputManager->pressedKeys[key] = false;
                inputManager->heldKeys[key] = false;
                inputManager->lastKeyHoldTimes[key] = 0.0f;
                inputManager->onKeyReleased.Emit(key);
            }
        }
    }
}