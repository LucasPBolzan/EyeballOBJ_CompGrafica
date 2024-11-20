#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

void main()
{
    // Obter o normal do normal map
    vec3 normal = texture(normalMap, fs_in.TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0); // Converter de [0,1] para [-1,1]

    // Direções de luz e visualização
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    // Componente difusa
    float diff = max(dot(normal, lightDir), 0.0);

    // Componente especular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * texture(specularMap, fs_in.TexCoord).rgb;

    // Cor final
    vec3 diffuse = diff * texture(diffuseMap, fs_in.TexCoord).rgb;
    vec3 ambient = 0.1 * diffuse;
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}
