
#include "pch.h"

using namespace std;

void UpdateInputStateKeyboard(int, int, int, int);
void UpdateInputStateMouse(int, int, int);
void UpdateInputMousePosition(double, double);
void UpdateInputMouseWheel(double, double);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_REPEAT) return; 
    UpdateInputStateKeyboard(key, scancode, action, mods);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    UpdateInputStateMouse(button, action, mods);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    UpdateInputMousePosition(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    UpdateInputMouseWheel(xoffset, yoffset);
}


