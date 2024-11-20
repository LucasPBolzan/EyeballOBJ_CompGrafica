# EyeballOBJ_CompGrafica

## Descrição

Este projeto é uma aplicação gráfica em C++ que utiliza OpenGL para renderizar um modelo 3D de um olho humano. O programa permite que o usuário visualize o modelo em 3D, mova a câmera ao redor e interaja com a cena.

## Pré-requisitos

Para executar este projeto, você precisará dos seguintes softwares instalados em seu sistema:

- **Compilador C++** compatível com C++11 ou superior (por exemplo, GCC, Clang, MSVC).
- **OpenGL** (versão 3.3 ou superior).
- **GLEW** (OpenGL Extension Wrangler Library).
- **GLFW** (biblioteca para criação de janelas e gerenciamento de entrada).
- **GLM** (OpenGL Mathematics) para operações matemáticas.
- **Assimp** (Open Asset Import Library) para carregar modelos 3D.

Certifique-se de que todas as bibliotecas estão corretamente instaladas e configuradas no seu ambiente de desenvolvimento.

## Como Compilar e Executar o Projeto

### Passo 1: Clonar o Repositório

Clone o repositório para o seu computador:

```bash
git clone https://github.com/LucasPBolzan/EyeballOBJ_CompGrafica.git
```

### Passo 2: Navegar até o Diretório do Projeto

```bash
cd EyeballOBJ_compGrafica
```

### Passo 3: Compilar o Código
Utilize o comando abaixo para compilar o código.Certifique-se de ajustar os caminhos de inclusão e de biblioteca conforme necessário para o seu sistema.
```bash
g++ main.cpp -o EyeballApp -lGL -lGLEW -lglfw -lassimp
```

### Passo 4: Executar o Programa
Após a compilação bem-sucedida, execute o programa:
```bash
./EyeballApp
```

### Como Interagir com o Programa
Movimentação da Câmera
- W: Move a câmera para frente.
- S: Move a câmera para trás.
- A: Move a câmera para a esquerda.
- D: Move a câmera para a direita.
- Q: Move a câmera para cima.
- E: Move a câmera para baixo.

### Orientação da Câmera:
Mova o mouse para girar a câmera em torno do modelo. O movimento do mouse ajusta os ângulos de visão (yaw e pitch).

### Fechar o Programa:
Pressione Esc ou feche a janela para sair do programa.

## Descrição dos Elementos do Cenário

### Modelo 3D do Olho Humano

- O programa carrega e renderiza um modelo 3D de um olho humano (`eyeball.obj`).
- O modelo é renderizado sem texturas, utilizando uma cor sólida definida no shader.

### Câmera Interativa

- Implementa um sistema de câmera em primeira pessoa, permitindo que o usuário explore a cena livremente.
- A câmera pode ser movida em todas as direções e rotacionada usando o mouse.

### Cenário Simples

- O foco do programa é a visualização do modelo do olho.
- Não há outros elementos no cenário além do modelo carregado.

## Estrutura do Projeto

- **main.cpp**: Arquivo principal contendo o código-fonte da aplicação.
- **vertex_shader.vert**: Shader de vértice usado para processar os vértices do modelo.
- **fragment_shader.frag**: Shader de fragmento usado para determinar a cor dos pixels renderizados.
- **eyeball.obj**: Arquivo de modelo 3D do olho humano.
- **.gitignore**: Arquivo para ignorar arquivos desnecessários no controle de versão.
- **README.md**: Este arquivo, contendo instruções e informações sobre o projeto.

## Dependências

- **OpenGL**: Biblioteca gráfica para renderização 2D e 3D.
- **GLEW**: Gerenciador de extensões OpenGL.
- **GLFW**: Biblioteca para criação de janelas e captura de eventos de entrada.
- **GLM**: Biblioteca matemática para gráficos.
- **Assimp**: Biblioteca para importação de modelos 3D.


## Licença

Este projeto é distribuído sob a licença MIT. Consulte o arquivo [LICENSE](./LICENSE) para mais informações.

## Créditos

- **Desenvolvedor**: Lucas P. Bolzan
- **Modelo 3D**: Certifique-se de que o modelo `eyeball.obj` está sendo utilizado de acordo com sua licença original.

## Contato

- **Email**: [lucaspbolzan@example.com](mailto:lucaspbolzan@example.com)
- **GitHub**: [LucasPBolzan](https://github.com/LucasPBolzan)

  
