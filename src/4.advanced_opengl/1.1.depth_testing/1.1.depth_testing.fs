#version 330 core
out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;  // 深度值还原为 NDC [-1, 1]
    return (2.0 * near * far) / (far + near - z * (far - near));  // 逆变换，将非线性的NDC深度值线性化
}

vec3 BoardShading()
{
    vec3 board = vec3(0.0, 0.0, 0.0);
    return board;
}

void main()
{    
    //float depth = LinearizeDepth(gl_FragCoord.z) / far; 
    //FragColor = vec4(vec3(depth), 1.0);

    FragColor = vec4(BoardShading(), 1.0);
}