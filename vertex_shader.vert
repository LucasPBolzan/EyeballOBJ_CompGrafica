#version 330 core
layout(location = 0) in vec3 aPos;      // Posição
layout(location = 1) in vec3 aNormal;   // Normal
layout(location = 2) in vec2 aTexCoord; // Coordenada de textura
layout(location = 3) in vec3 aTangent;  // Tangente
layout(location = 4) in vec3 aBitangent; // Bitangente

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    fs_in.FragPos = vec3(model * vec4(aPos, 1.0));
    fs_in.TexCoord = aTexCoord;

    // Calculando as matrizes de transformação para o espaço tangente
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBitangent);
    vec3 N = normalize(normalMatrix * aNormal);
    mat3 TBN = transpose(mat3(T, B, N));

    fs_in.TangentLightPos = TBN * lightPos;
    fs_in.TangentViewPos = TBN * viewPos;
    fs_in.TangentFragPos = TBN * fs_in.FragPos;

    gl_Position = projection * view * vec4(fs_in.FragPos, 1.0);
}
