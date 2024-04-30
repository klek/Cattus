#version 450

layout(location = 0) in vec4 inPosition;

layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform Push {
    float width;
    float height;
    float frame;
} push;

//vec2 positions[3] = vec2[](
//    vec2(0.0, -0.5),
//    vec2(0.5, 0.5),
//    vec2(-0.5, 0.5)
//);

//vec3 colors[3] = vec3[3](
//    vec3(1.0, 0.0, 0.0),
//    vec3(0.0, 1.0, 0.0),
//    vec3(0.0, 0.0, 1.0)
//);

void main()
{
    // Old code for hardcoded vertices 
//    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
//    fragColor = colors[gl_VertexIndex];

    // New code for vertices that is sent to the GPU from the CPU
    gl_Position = vec4( inPosition.x, inPosition.y, inPosition.z, inPosition.w );
//    fragColor = inColor;
    fragColor = inPosition;
}