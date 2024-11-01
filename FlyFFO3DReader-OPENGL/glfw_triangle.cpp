#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <SOIL2/SOIL2.h> // Inclua a biblioteca SOIL2 para carregar texturas
#include "LoadedObject.hpp" // Inclua seu header LoadedObject

// Variáveis globais
HDC hdc;
HGLRC hglrc;
HWND hwnd;
float cubeRotationX = 0.0f;
float cubeRotationY = 0.0f;
float cubePositionX = 0.0f;
float cubePositionY = 0.0f;
float cubePositionZ = -5.0f;
bool mouseDown = false;
POINT lastMousePos;

LoadedObject loadedModel; // Instância do seu modelo carregado
GLuint textureID; // ID da textura

// Prototipos de funções
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitOpenGL();
void RenderScene();
void ResizeGLScene(int width, int height);
void Update(); // Adicione um protótipo para a função de atualização
GLuint LoadTexture(const char* filename); // Prototipo da função para carregar texturas

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// Criação da janela
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "OpenGLWindowClass";
	RegisterClass(&wc);

	hwnd = CreateWindowEx(0, wc.lpszClassName, "OpenGL Model Loader", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hwnd, nShowCmd);

	InitOpenGL();
	

	std::string dir = "E:\\Flyff\\Flyff Programming Area\\Flyff Mocomochi\\Source Flyff Mocomochi\\FlyFF_Mocomochi\\Model\\";
	std::string model = dir + "mvr_AiBatt.o3d";

	// Carrega o modelo 3D.
	if (loadedModel.LoadObject(model.c_str()))
	{
		std::string texture = dir + "\\Texture\\" + loadedModel.getTextureName();
		// Carregar a textura
		textureID = LoadTexture(texture.c_str());
		if (textureID == 0) {
			std::cerr << "Erro ao carregar a textura." << std::endl;
		}
	}
	else
	{
		std::cerr << "Erro ao carregar o modelo." << std::endl;
	}
	
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

void InitOpenGL() {
	hdc = GetDC(hwnd);
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32; 
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);
	hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);

	// Inicializa GLEW
	glewInit();

	// Habilita o teste de profundidade
	glEnable(GL_DEPTH_TEST);

	// Set the front face to counterclockwise
	glFrontFace(GL_CCW);

	// Enable face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // Cull back faces
}

void ResizeGLScene(int width, int height) {
	if (height == 0) height = 1;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

GLuint LoadTexture(const char* filename) {
	GLuint textureID = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (textureID == 0) {
		std::cerr << "Failed to load texture: " << SOIL_last_result() << std::endl;
		return 0; // Retorna 0 em caso de erro
	}

	// Define os parâmetros da textura
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

class CUSTOM_VERTEX2
{
public:
	glm::vec3				    m_v3Pos;
	glm::vec2				    m_v2Tx;

	enum {
		FVF = (0x002 | 0x100)
	};
};

CUSTOM_VERTEX2			m_Flat[4];

struct FVF_3DVERTEX
{
	glm::vec3 vPos;
	DWORD		dwColor;
};

FVF_3DVERTEX m_vGrids[100];

void RenderScene() {

	m_Flat[0].m_v3Pos = glm::vec3(-10, 0, -10);
	m_Flat[1].m_v3Pos = glm::vec3(-10, 0, 10);
	m_Flat[2].m_v3Pos = glm::vec3(10, 0, 10);
	m_Flat[3].m_v3Pos = glm::vec3(10, 0, -10);

	int m_nGridCount = 0;
	for (int i = -10; i < 11; i++) {
		if (i == 0) continue;
		m_vGrids[m_nGridCount++].vPos = glm::vec3(-10.0f, .0f, float(i));
		m_vGrids[m_nGridCount++].vPos = glm::vec3(10.0f, .0f, float(i));
	}
	for (int i = -10; i < 11; i++) {
		if (i == 0) continue;
		m_vGrids[m_nGridCount++].vPos = glm::vec3(float(i), .0f, -10.0f);
		m_vGrids[m_nGridCount++].vPos = glm::vec3(float(i), .0f, 10.0f);
	}
	m_vGrids[m_nGridCount++].vPos = glm::vec3(.0f, .0f, -10.0f);
	m_vGrids[m_nGridCount++].vPos = glm::vec3(.0f, .0f, 10.0f);
	m_vGrids[m_nGridCount++].vPos = glm::vec3(-10.0f, .0f, .0f);
	m_vGrids[m_nGridCount++].vPos = glm::vec3(10.0f, .0f, .0f);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa os buffers de cor e profundidade
	glLoadIdentity();

	glTranslatef(cubePositionX, cubePositionY, cubePositionZ);
	glRotatef(cubeRotationX, 1.0f, 0.0f, 0.0f);
	glRotatef(cubeRotationY, 0.0f, 1.0f, 0.0f);

	//if (m_bBG)
	{
		glColor4f(0.25f, 0.25f, 0.25f, 1.0f);
		glBegin(GL_LINES);
		for (int i = 0; i < m_nGridCount / 2 - 2; ++i)
		{
			// Assuming m_vGrids contains the vertices for the grid
			glVertex3f(m_vGrids[i].vPos.x, m_vGrids[i].vPos.y, m_vGrids[i].vPos.z);
		}
		glEnd();
	}

	// Draw flat
	//if (m_bFlat)
	{
		glBegin(GL_TRIANGLE_FAN);
		// Assuming m_Flat is a vertex structure with data for the flat shape
		glVertex3f(m_Flat->m_v3Pos.x, m_Flat->m_v3Pos.y, m_Flat->m_v3Pos.z);
		glEnd();
	}
	glEnd();

	/*
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa os buffers de cor e profundidade
    glLoadIdentity();

    glTranslatef(cubePositionX, cubePositionY, cubePositionZ);
    glRotatef(cubeRotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(cubeRotationY, 0.0f, 1.0f, 0.0f);

    // Habilita a textura
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID); // Vínculo da textura

    // Renderize o modelo carregado
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, loadedModel.getVertexData()); // Defina os dados do vértice

    // Se as coordenadas de textura estiverem disponíveis, ative e defina
   if (loadedModel.getVertexUVCount() > 0) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, loadedModel.getVertexUVData()); // Use getUvs para obter os dados das coordenadas UV
    }

    glDrawElements(GL_TRIANGLES, loadedModel.getIndexCount(), GL_UNSIGNED_INT, loadedModel.getIndexData()); // Desenhe os elementos

    glDisableClientState(GL_VERTEX_ARRAY);
    if (!loadedModel.getUvs().empty()) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY); // Desative as coordenadas de textura apenas se estiverem habilitadas
    }

    glDisable(GL_TEXTURE_2D); // Desabilita a textura
	*/

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(-0.5f, -0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.5f, -0.5f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(0.0f, 0.5f);
	glEnd();

    SwapBuffers(hdc);
}

void Update() {
	// Atualize a cena se necessário (ex.: animações)
	InvalidateRect(hwnd, nullptr, TRUE); // Força a atualização da janela
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_SIZE:
		ResizeGLScene(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_PAINT:
		RenderScene();
		Update(); // Chame a função de atualização aqui
		break;
	case WM_LBUTTONDOWN:
		mouseDown = true;
		GetCursorPos(&lastMousePos);
		SetCapture(hwnd);
		break;
	case WM_LBUTTONUP:
		mouseDown = false;
		ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		if (mouseDown) {
			POINT currentMousePos;
			GetCursorPos(&currentMousePos);
			cubeRotationX += (currentMousePos.y - lastMousePos.y) * 0.5f;
			cubeRotationY += (currentMousePos.x - lastMousePos.x) * 0.5f;
			lastMousePos = currentMousePos;
			InvalidateRect(hwnd, nullptr, FALSE);
		}
		break;
	case WM_DESTROY:
		wglMakeCurrent(hdc, nullptr);
		wglDeleteContext(hglrc);
		ReleaseDC(hwnd, hdc);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}
