struct GameUBO {
    vec4 Attributes;
};

layout(std140, binding = 2) uniform GameUniformBufferObject {
    GameUBO Game;
};

const float Timer      = Game.Attributes.x;
const float Resolution = Game.Attributes.y;
const float Unused_1   = Game.Attributes.z;
const float Unused_2   = Game.Attributes.w;