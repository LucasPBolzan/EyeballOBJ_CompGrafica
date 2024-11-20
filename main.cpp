#include <GL/glew.h>             // Biblioteca GLEW para carregar extensões OpenGL
#include <GLFW/glfw3.h>          // Biblioteca GLFW para criar janelas e contextos OpenGL

#include <glm/glm.hpp>           // Biblioteca GLM para operações matemáticas com vetores e matrizes
#include <glm/gtc/matrix_transform.hpp> // Funções adicionais de transformação da GLM
#include <glm/gtc/type_ptr.hpp>  // Funções para converter vetores e matrizes em ponteiros

#include <assimp/Importer.hpp>   // Biblioteca Assimp para importar modelos 3D
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

// Variáveis globais para a câmera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);   // Posição inicial da câmera
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);  // Direção para onde a câmera está apontando
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);   // Vetor "up" da câmera

float deltaTime = 0.0f; // Tempo entre frames atuais e anteriores
float lastFrame = 0.0f; // Tempo do último frame

float yaw = -90.0f; // Ângulo de rotação em torno do eixo Y (horizontal)
float pitch = 0.0f;  // Ângulo de rotação em torno do eixo X (vertical)
bool firstMouse = true; // Flag para verificar se é a primeira vez que o mouse é usado

float lastX = 400, lastY = 300; // Últimas posições conhecidas do mouse

// Funções utilitárias

// Função para ler o código-fonte de um shader a partir de um arquivo
std::string readShaderSource(const std::string& filePath) {
    std::ifstream shaderFile(filePath);      // Abre o arquivo do shader
    std::stringstream shaderStream;

    if (shaderFile.is_open()) {
        shaderStream << shaderFile.rdbuf();  // Lê o conteúdo do arquivo
        shaderFile.close();
    }
    else {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }

    return shaderStream.str(); // Retorna o código do shader como uma string
}

// Função para compilar um shader (vertex ou fragment)
GLuint compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);         // Cria um objeto shader
    const char* src = source.c_str();             // Converte a string para um ponteiro de caracteres
    glShaderSource(shader, 1, &src, nullptr);     // Associa o código-fonte ao shader
    glCompileShader(shader);                      // Compila o shader

    // Verifica erros de compilação
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog); // Obtém o log de erros
        std::cerr << "Shader Compilation Error: " << infoLog << std::endl;
    }

    return shader; // Retorna o identificador do shader compilado
}

// Função para criar um programa de shader a partir dos arquivos de vertex e fragment shaders
GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    // Lê o código-fonte dos shaders
    std::string vertexCode = readShaderSource(vertexPath);
    if (vertexCode.empty()) {
        std::cerr << "Vertex shader source is empty!" << std::endl;
        return 0;
    }

    std::string fragmentCode = readShaderSource(fragmentPath);
    if (fragmentCode.empty()) {
        std::cerr << "Fragment shader source is empty!" << std::endl;
        return 0;
    }

    // Compila os shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    // Cria o programa de shader e anexa os shaders compilados
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); // Linka os shaders no programa

    // Verifica erros de linkage
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog); // Obtém o log de erros
        std::cerr << "Program Linking Error: " << infoLog << std::endl;
    }

    // Deleta os shaders individuais, pois já estão linkados no programa
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram; // Retorna o identificador do programa de shader
}

// Função para carregar um modelo 3D usando a biblioteca Assimp
void loadModel(const std::string& path, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
    Assimp::Importer importer;

    // Importa o modelo com triangulação e flip nas coordenadas de textura (se necessário)
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        // Verifica se houve erros na importação
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    // Obtém o primeiro mesh (malha) do modelo
    aiMesh* mesh = scene->mMeshes[0];
    if (!mesh) {
        std::cerr << "ERROR::ASSIMP::Mesh not found in file: " << path << std::endl;
        return;
    }

    // Carrega os vértices (posições)
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D pos = mesh->mVertices[i];
        vertices.push_back(pos.x);
        vertices.push_back(pos.y);
        vertices.push_back(pos.z);
    }

    // Carrega os índices das faces
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Imprime informações sobre o modelo carregado
    std::cout << "Model loaded: " << path << std::endl;
    std::cout << "Vertices count: " << vertices.size() / 3 << std::endl;
    std::cout << "Indices count: " << indices.size() << std::endl;
}

// Função para processar a entrada do teclado
void processKeyboardInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime; // Ajusta a velocidade da câmera com base no tempo entre frames

    // Movimenta a câmera em diferentes direções com base nas teclas pressionadas
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront; // Para frente
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront; // Para trás
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; // Para a esquerda
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; // Para a direita
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp; // Para cima
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp; // Para baixo
}

// Função de callback para o movimento do mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouse = true;

    if (firstMouse) {
        // Inicializa as últimas posições do mouse na primeira vez
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Calcula o deslocamento do mouse
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Invertido porque as coordenadas Y aumentam para baixo
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // Sensibilidade do mouse
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;   // Atualiza o ângulo yaw
    pitch += yoffset; // Atualiza o ângulo pitch

    // Limita o ângulo pitch para evitar que a câmera inverta
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Calcula o novo vetor de direção da câmera
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front); // Normaliza o vetor de direção
}

// Função de callback para redimensionamento da janela
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height); // Ajusta o viewport OpenGL para as novas dimensões da janela
}

int main() {
    // Inicialização do GLFW
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar o GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Especifica a versão maior do OpenGL (3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Especifica a versão menor do OpenGL (3.3)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Utiliza o perfil core do OpenGL

    // Criação da janela GLFW
    GLFWwindow* window = glfwCreateWindow(800, 600, "Cubo com OBJ", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // Finaliza o GLFW em caso de falha
        return -1;
    }
    glfwMakeContextCurrent(window); // Torna o contexto OpenGL atual

    // Configurações de callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Callback para redimensionamento
    glfwSetCursorPosCallback(window, mouse_callback); // Callback para movimento do mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Oculta o cursor e captura o mouse

    // Inicialização do GLEW
    glewExperimental = GL_TRUE; // Ativa recursos experimentais
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Habilitar teste de profundidade
    glEnable(GL_DEPTH_TEST);

    // Compilar e vincular shaders
    GLuint shaderProgram = createShaderProgram("vertex_shader.vert", "fragment_shader.frag");
    if (shaderProgram == 0) {
        return -1;
    }

    // Matrizes de transformação
    glm::mat4 model = glm::mat4(1.0f); // Matriz modelo (identidade)
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f); // Matriz de projeção

    // Carregar modelo
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    loadModel("eyeball.obj", vertices, indices);

    if (vertices.empty() || indices.empty()) {
        return -1; // Encerra o programa se o modelo não foi carregado corretamente
    }

    // Configuração dos buffers e atributos de vértice
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO); // Gera um Vertex Array Object
    glGenBuffers(1, &VBO);      // Gera um Vertex Buffer Object
    glGenBuffers(1, &EBO);      // Gera um Element Buffer Object

    glBindVertexArray(VAO); // Liga o VAO

    // Carregar dados para o VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Liga o VBO
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW); // Envia os dados dos vértices

    // Carregar índices para o EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Liga o EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW); // Envia os índices

    // Especificar os ponteiros de atributo de vértice
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); // Atributo posição
    glEnableVertexAttribArray(0); // Habilita o atributo posição

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Desliga o VBO
    glBindVertexArray(0); // Desliga o VAO

    // Loop de renderização
    while (!glfwWindowShouldClose(window)) {
        // Calcula o tempo entre frames
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Processa a entrada do usuário
        processKeyboardInput(window);

        // Renderização
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa o buffer de cor e profundidade
        glUseProgram(shaderProgram); // Usa o programa de shader

        // Atualiza a matriz de visualização (view)
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Obter locais das uniforms
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

        // Definir as uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));       // Matriz modelo
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));         // Matriz de visualização
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));   // Matriz de projeção

        // Desenhar o modelo
        glBindVertexArray(VAO); // Liga o VAO
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Desenha os elementos
        glBindVertexArray(0); // Desliga o VAO

        glfwSwapBuffers(window); // Troca os buffers (duplo buffering)
        glfwPollEvents();        // Processa eventos (input)
    }

    // Limpar recursos alocados
    glDeleteVertexArrays(1, &VAO); // Deleta o VAO
    glDeleteBuffers(1, &VBO);      // Deleta o VBO
    glDeleteBuffers(1, &EBO);      // Deleta o EBO

    glfwTerminate(); // Encerra o GLFW
    return 0;
}
