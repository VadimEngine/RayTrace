#version 430

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout (rgba32f, binding = 0) uniform image2D img;

uniform mat4 invProjMatrix;
uniform mat4 invViewMatrix;

// Sphere struct
struct Sphere {
    vec3 center;
    float radius;
    vec3 color;
    float reflectivity; // Controls reflection intensity
};

// Shader Storage Buffer Object (SSBO) for spheres
layout(std430, binding = 1) buffer SphereBuffer {
    Sphere spheres[];
};
uniform int numSpheres;

// Computes Fresnel reflection factor using Schlick’s approximation
float fresnelSchlick(float cosTheta, float F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Ray-Sphere Intersection Function
bool intersectSphere(vec3 rayOrigin, vec3 rayDir, Sphere sphere, out float t) {
    vec3 oc = rayOrigin - sphere.center;
    float a = dot(rayDir, rayDir);
    float b = 2.0 * dot(oc, rayDir);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0) return false; // No intersection

    float sqrtD = sqrt(discriminant);
    float t0 = (-b - sqrtD) / (2.0 * a);
    float t1 = (-b + sqrtD) / (2.0 * a);

    t = (t0 > 0.0) ? t0 : t1;
    return t > 0.0;
}

// Trace the scene with reflections (without recursion)
void main() {
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 imgSize = imageSize(img);

    if (pixelCoords.x >= imgSize.x || pixelCoords.y >= imgSize.y) return;

    // Convert pixel coordinates to normalized device coordinates (NDC: -1 to 1)
    vec2 uv = (vec2(pixelCoords) / vec2(imgSize)) * 2.0 - 1.0;

    // Transform NDC to clip space
    vec4 clipSpacePos = vec4(uv, -1.0, 1.0);

    // Convert clip space to view space using precomputed inverse projection
    vec4 viewSpacePos = invProjMatrix * clipSpacePos;
    viewSpacePos /= viewSpacePos.w; // Perspective divide

    // Compute ray origin (camera position in world space)
    vec3 rayOrigin = vec3(invViewMatrix[3]);

    // Compute ray direction using precomputed inverse view matrix
    vec3 rayDir = normalize((invViewMatrix * viewSpacePos).xyz - rayOrigin);

    const int maxBounces = 8; // Number of reflections allowed
    vec3 accumulatedColor = vec3(0.0);
    vec3 currentAttenuation = vec3(1.0); // How much color carries through reflections

    // Iterative ray tracing instead of recursion
    for (int bounce = 0; bounce <= maxBounces; ++bounce) {
        float minT = 1e20;
        int closestSphereIndex = -1;

        // Find the closest intersection
        for (int i = 0; i < numSpheres; ++i) {
            float t;
            if (intersectSphere(rayOrigin, rayDir, spheres[i], t) && t < minT) {
                minT = t;
                closestSphereIndex = i;
            }
        }

        // If no intersection, blend with background color
        if (closestSphereIndex == -1) {
            accumulatedColor += currentAttenuation * vec3(0.1, 0.1, 0.2); // Dark blue background
            break;
        }

        // Get the closest hit sphere
        Sphere hitSphere = spheres[closestSphereIndex];

        // Compute intersection point and normal
        vec3 hitPoint = rayOrigin + minT * rayDir;
        vec3 normal = normalize(hitPoint - hitSphere.center);

        // Simple lighting (light at (1,1,0))
        vec3 lightDir = normalize(vec3(1.0, 1.0, 0.0));
        float brightness = max(dot(normal, lightDir), 0.0);
        vec3 baseColor = hitSphere.color * brightness;

        // Fresnel reflection factor (based on view angle)
        float viewDotNormal = max(dot(-rayDir, normal), 0.0);
        float reflectFactor = fresnelSchlick(viewDotNormal, hitSphere.reflectivity);

        // Adjust reflection direction based on sphere curvature
        vec3 reflectDir = normalize(reflect(rayDir, normal));

        // Slightly distort the reflection direction based on normal
        reflectDir = normalize(mix(reflectDir, normal, 0.2)); // 20% blend with normal for curvature

        // Update the ray direction and origin for the next bounce
        rayOrigin = hitPoint + reflectDir * 0.001; // Offset to avoid self-intersection
        rayDir = reflectDir;

        // Accumulate color with reflection factor
        accumulatedColor += currentAttenuation * mix(baseColor, accumulatedColor, reflectFactor);

        // Reduce color strength for next bounce
        currentAttenuation *= hitSphere.reflectivity;
    }

    // Store the final color in the framebuffer
    imageStore(img, pixelCoords, vec4(accumulatedColor, 1.0));
}
