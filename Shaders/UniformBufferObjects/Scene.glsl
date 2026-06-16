struct SceneUBO {
    vec4 TIMERS;
};

layout(std140, binding = 0) uniform SceneUniformBufferObject {
    SceneUBO SCENE;
};

const float TotalTime      = SCENE.TIMERS.x;
const float GridResolution = SCENE.TIMERS.y;


/*
    When all is done, move GridResolution to another UBO.
*/