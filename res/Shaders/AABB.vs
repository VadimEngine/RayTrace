#version 460
layout(location = 0) in vec3 localPosition;  // Cube vertex positions

struct AABB {
    vec3 min;
    vec3 max;
};

layout(std430, binding = 0) buffer AABBBuffer {
    AABB aabbs[];
};

uniform mat4 view;
uniform mat4 projection;

void main() {
    AABB box = aabbs[gl_InstanceID];

    // Convert unit cube to AABB size
    vec3 worldPos = mix(box.min, box.max, localPosition);

    gl_Position = projection * view * vec4(worldPos, 1.0);
}