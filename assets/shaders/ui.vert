#version 450

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;

layout(location = 0) out vec4 vColor;

layout(push_constant) uniform Push {
    vec2 uScale;
    vec2 uTranslate;
}
pc;

void main() {
    vColor = aColor;
    gl_Position = vec4(aPos * pc.uScale + pc.uTranslate, 0.0, 1.0);
}
