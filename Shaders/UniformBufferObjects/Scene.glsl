struct SceneUBO {
    vec4 TIMERS;
    //vec4 GridStuff; maybe instead of another UBO
};

layout(std140, binding = 0) uniform SceneUniformBufferObject {
    SceneUBO SCENE;
};

const float TotalTime      = SCENE.TIMERS.x;
const float GridResolution = SCENE.TIMERS.y;
