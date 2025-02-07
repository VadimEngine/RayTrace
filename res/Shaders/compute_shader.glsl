#version 430

layout(local_size_x = 256) in; // Each workgroup processes 256 elements

layout(std430, binding = 0) buffer Data {
    int data[];
};

void main() {
    uint id = gl_GlobalInvocationID.x; // Unique thread ID
    data[id] += 1; // Increment each element
}
