#version 450 core
in vec3 FragPos;
in vec3 Color;
out vec4 fFragColor;
uniform vec3 uLightPos;
uniform vec3 uViewPos;

void main() {
    vec3 normal = normalize(cross(dFdx(FragPos), dFdy(FragPos)));
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 ambient = 0.1 * Color;
    vec3 diffuse = diff * Color;
    vec3 specular = 0.2 * spec * vec3(1.0, 1.0, 1.0);
    fFragColor = vec4(ambient + diffuse + specular, 1.0);
}