#version 430

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout (rgba32f, binding = 0) uniform image2D img;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

const vec3 sphereCenter = vec3(0.0, 1.0, 3.0);
const float sphereRadius = 1.0;

// Ray-Sphere Intersection Function
bool intersectSphere(vec3 rayOrigin, vec3 rayDir, out float t) {
    vec3 oc = rayOrigin - sphereCenter;
    float a = dot(rayDir, rayDir);
    float b = 2.0 * dot(oc, rayDir);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0) {
        return false; // No intersection
    }

    // Find the nearest positive intersection
    float sqrtD = sqrt(discriminant);
    float t0 = (-b - sqrtD) / (2.0 * a);
    float t1 = (-b + sqrtD) / (2.0 * a);

    t = (t0 > 0.0) ? t0 : t1;
    return t > 0.0;
}

void main() {
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 imgSize = imageSize(img);
    
    if (pixelCoords.x >= imgSize.x || pixelCoords.y >= imgSize.y) return;

    // Convert pixel coordinates to normalized device coordinates (NDC: -1 to 1)
    vec2 uv = (vec2(pixelCoords) / vec2(imgSize)) * 2.0 - 1.0;
    uv.x *= float(imgSize.x) / float(imgSize.y); // Maintain aspect ratio

    // Convert NDC to clip space
    vec4 clipSpacePos = vec4(uv, -1.0, 1.0); // NDC Z = -1 for near plane

    // Transform to view space (invert projection matrix)
    vec4 viewSpacePos = inverse(projMatrix) * clipSpacePos;
    viewSpacePos /= viewSpacePos.w; // Perspective divide

    // Transform to world space (invert view matrix)
    vec4 worldSpacePos = inverse(viewMatrix) * viewSpacePos;

    // Compute ray origin (camera position in world space)
    vec3 rayOrigin = vec3(inverse(viewMatrix)[3]); // Extract camera position

    // Compute ray direction
    vec3 rayDir = normalize(worldSpacePos.xyz - rayOrigin);

    // Perform ray-sphere intersection
    float t;


    if (!intersectSphere(rayOrigin, rayDir, t)) {
        return; // No intersection, keep previous framebuffer value
    }

    // Compute intersection point
    vec3 hitPoint = rayOrigin + t * rayDir;

    // Compute normal at hit point
    vec3 normal = normalize(hitPoint - sphereCenter);

    // Simple lighting (light at (1,1,0))
    vec3 lightDir = normalize(vec3(1.0, 1.0, 0.0));
    float brightness = max(dot(normal, lightDir), 0.0);

    // Shaded color
    vec4 newColor = vec4(vec3(1.0, 0.2, 0.2) * brightness, 1.0); // Red sphere with shading

    imageStore(img, pixelCoords, newColor);
}
