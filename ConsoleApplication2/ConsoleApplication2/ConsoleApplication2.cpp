#define _USE_MATH_DEFINES  // для "пи"
#include <windows.h>
#include <glut.h>
#include <cmath>   // для мат. функций

LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
GLUquadricObj* FQobj;
float ambient_1[4], diffuse_1[4], specular_1[4];
float ambient_2[4], diffuse_2[4], specular_2[4];
float position_[4];
int main()
{
	//Получаем хендл приложения, потребуется при создании класса окна и самого окна.
	HINSTANCE histance = GetModuleHandleW(NULL);

	//Создаем класс окна.
	WNDCLASSEX wclass = { 0 };          //Обнуляем структуру с самого начала, так как заполнять будем не все поля.
	wclass.cbSize = sizeof(WNDCLASSEX);      //По размеру структуры Windows определит, какая версия API была использована.
	wclass.style = CS_HREDRAW | CS_VREDRAW;    //Говорим окну перерисовываться при изменении размеров окна.
	wclass.lpfnWndProc = WndProc;        //Указываем функцию обработки сообщений.
	wclass.hInstance = histance;        //Указываем хендл приложения.
	wclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);    //GetStockObject возвращает хендл на белую кисточку, для фона окна
	wclass.lpszClassName = L"MYCLASS";      //Имя данного класса, должно быть уникальным, иначе, если класс с таким именем уже зарегестрирован, то в регистрации будет отказано.
	//Регистрируем класс окна.
	RegisterClassEx(&wclass);

	HWND w;

	//Создаем окно.
	w = CreateWindowExW(
		0,
		L"MYCLASS",                //Имя класса.
		L"Title",                //Заголовок окна.
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, //WS_OVERLAPPEDWINDOW,          //Тип окна, влияет на отображение системного меню, кнопок в верхнем правом углу и т.п.
		50, 50,                  //Координаты окна.
		500, 500,                //Ширина окна.
		0,                    //Ссылка на родительское окно.
		0,                    //Хендл меню.
		histance,                //Хендл приложения, получаем его функцией GetModuleHandleW.
		0
	);

	//Показываем окно, если этого не сделать окно не будет отображено.
	ShowWindow(w, SW_SHOW);
	//Обновляем окно.
	UpdateWindow(w);

	//Запускаем цикл обработки сообщений окна.
	MSG msg = { 0 };
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);  //Преобразуем виртуальную клавишу в ASCII-код и посылаем сообщение WM_CHAR (тут не нужно.Необходимо, если надо работать с текстом, вводимым с клавиатуры)
		DispatchMessage(&msg);  //Передаем сообщения для обработки в "главную функцию обработки сообщений"
	}
	return 0;
}
void Axes2(GLUquadricObj* qobj, GLfloat size)
{
	// вычтем длину стрелки
	size = size - 0.075;
	// сфера в начале координат
	gluSphere(qobj, 0.015, 6, 6);
	// ось x
	glPushMatrix();
	gluCylinder(qobj, 0.005, 0.005, size, 8, 5);
	glTranslatef(0.0, 0.0, size);
	gluCylinder(qobj, 0.02, 0.0, 0.075, 8, 1);
	gluDisk(qobj, 0.0, 0.02, 8, 1);
	glPopMatrix();
	// ось z
	glPushMatrix();
	glRotatef(90.0, 0.0, 1.0, 0.0);
	gluCylinder(qobj, 0.005, 0.005, size, 6, 5);
	glTranslatef(0.0, 0.0, size);
	gluCylinder(qobj, 0.02, 0.0, 0.075, 8, 1);
	gluDisk(qobj, 0.0, 0.02, 8, 1);
	glPopMatrix();
	// ось y
	glPushMatrix();
	glRotatef(90.0, -1.0, 0.0, 0.0);
	gluCylinder(qobj, 0.005, 0.005, size, 6, 5);
	glTranslatef(0.0, 0.0, size);
	gluCylinder(qobj, 0.02, 0.0, 0.075, 8, 1);
	gluDisk(qobj, 0.0, 0.02, 8, 1);
	glPopMatrix();
}


// главная функция обработки сообщений
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		gluDeleteQuadric(FQobj); 
		wglMakeCurrent(0, 0);
		wglDeleteContext(wglGetCurrentContext());
		ReleaseDC(hWnd, wglGetCurrentDC());
		PostQuitMessage(0);
		break;


	case WM_CREATE:
		HGLRC RC;
		HDC   DC;
		PIXELFORMATDESCRIPTOR pfd, * ppfd;

		wglMakeCurrent(0, 0);
		int pixelformat;
		ppfd = &pfd;
		ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
		ppfd->nVersion = 1;
		ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		ppfd->dwLayerMask = PFD_MAIN_PLANE;
		ppfd->iPixelType = PFD_TYPE_RGBA;
		ppfd->cColorBits = 16;
		ppfd->cDepthBits = 16;
		ppfd->cAccumBits = 0;
		ppfd->cStencilBits = 0;

		DC = GetDC(hWnd);	 	//1

		pixelformat = ChoosePixelFormat(DC, ppfd); //2
		SetPixelFormat(DC, pixelformat, ppfd);				//3
		RC = wglCreateContext(DC); //4
		wglMakeCurrent(DC, RC); 	//5

		// устанавливаем цвет очистки буфера кадра - темно-синий
		glClearColor(0.0, 0.0, 0.25, 0.0);
	
		// создаем quadric-объект для рисования
		FQobj = gluNewQuadric();
		// устанавливаем параметры смещения полигонов
		glPolygonOffset(1.0, 1.0);
		// разрешаем тест глубины
		glClearDepth(1.0);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);

		break;

	case WM_SIZE:
		WORD w, h;
		w = LOWORD(lParam);
		h = HIWORD(lParam);
		glViewport(0, 0, w, h);
		break;

	case WM_PAINT:


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		position_[0] = 0.9;
		position_[1] = 0.9;
		position_[2] = -0.5;
		position_[3] = 1;
		glLightfv(GL_LIGHT0, GL_POSITION, position_);

		ambient_1[0] = 0.135;
		ambient_1[1] = 0.2225;
		ambient_1[2] = 0.1575;
		ambient_1[3] = 0.8;

		diffuse_1[0] = 0.54;
		diffuse_1[1] = 0.89;
		diffuse_1[2] = 0.63;
		diffuse_1[3] = 0.8;

		specular_1[0] = 0.316228;
		specular_1[1] = 0.316228;
		specular_1[2] = 0.316228;
		specular_1[3] = 0.8;


		ambient_2[0] = 0.25;
		ambient_2[1] = 0.20725;
		ambient_2[2] = 0.20725;
		ambient_2[3] = 0.922;

		diffuse_2[0] = 1.0;
		diffuse_2[1] = 0.829;
		diffuse_2[2] = 0.829;
		diffuse_2[3] = 0.922;

		specular_2[0] = 0.296648;
		specular_2[1] = 0.296648;
		specular_2[2] = 0.296648;
		specular_2[3] = 0.922;

		glShadeModel(GL_SMOOTH);

		glLoadIdentity();
		
		glRotatef(20, 1.0, 0.0, 0.0);
		glRotatef(120, 0.0, 1.0, 0.0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//glColor3f(0.0, 0.95, 0.0);

		glMaterialfv(GL_FRONT_AND_BACK,
			GL_AMBIENT_AND_DIFFUSE, ambient_1);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse_1);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_1);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 11.264);

		Axes2(FQobj, 0.8);
		//glColor3f(0.95, 0.0, 0.95);		
		glMaterialfv(GL_FRONT_AND_BACK,
			GL_AMBIENT_AND_DIFFUSE, ambient_2);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse_2);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_2);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 76.8);
		gluSphere(FQobj, 0.4, 25, 25);



		// после завершения рисования необходимо выполнить обмен буферов
		// для показа изображения пользователю
		SwapBuffers(wglGetCurrentDC());

		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);//обрабатываем все остальные сообщения обработчиком "по умолчанию"
}