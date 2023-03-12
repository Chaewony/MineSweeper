#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <conio.h> // console io
#include <cstring> // string.h
#include <cstdlib> // stdlib.h
#include <string>
#include <string> // c++ string class
#include <Windows.h>
#include "Utils.h"

// ���� ���� Ÿ�� ������
enum Shape {
	None = 0, //������
	UnClicked, //Ŭ�� �ȵ� ��
	ClickedNotMine, //Ŭ�� �ư� ���ڰ� �ƴ� 
	ClickedAndMine, //Ŭ���ư� ������
	zero,
	one,
	two,
	three,
	Flag, //���
	NewLine, //\n
	End //������
};

// ���� ���� Ÿ�Կ� �´� ���� ����
char* ToShape(Shape es)
{
	char* shape = new char;

	switch (es)
	{
	case UnClicked:
	{
		strncpy(shape, "��", sizeof("��"));
		break;
	}
	case ClickedNotMine:
	{
		strncpy(shape, "��", sizeof("��"));
		break;
	}
	case ClickedAndMine:
	{
		strncpy(shape, "��", sizeof("��"));
		break;
	}
	case zero:
	{
		strncpy(shape, "0 ", sizeof("��"));
		break;
	}
	case one:
	{
		strncpy(shape, "1 ", sizeof("��"));
		break;
	}
	case two:
	{
		strncpy(shape, "2 ", sizeof("��"));
		break;
	}
	case three:
	{
		strncpy(shape, "3 ", sizeof("��"));
		break;
	}
	case Flag:
	{
		strncpy(shape, "��", sizeof("��"));
		break;
	}
	case NewLine:
	{
		strncpy(shape, "\n", sizeof("\n"));
		break;
	}
	case End:
	{
		strncpy(shape, "\0", sizeof("\0"));
		break;
	}
	default:
	{
		strncpy(shape, " ", sizeof(" "));
		break;
	}
	}

	return shape;
}

class Screen;
class Input;

class Screen {
private:
	int		width; // visible width
	int		height;
	int		size;
	Cell** cell; // ĭ �ϳ��� ĵ���� ��ü�� �ϳ��� ��ҷ� ���� ���� �� �ϳ��ϳ��� ���ڴ� ���̵��
				  // char�� �迭 ��ҷθ� ���� ��� �ٸ� ������ ���(�߰��ϱ�)�� �Ѱ谡 ����
	int		numberOfMine; //���� ������ ����
	bool	islooping;

	// constructor (������ �Լ�) �޸𸮰����� ����Ǵ� ���� ȣ��Ǵ�
	Screen(int width = 10, int height = 10)
		: width(width), height(height), cell(new Cell* [(width + 1) * height]), numberOfMine(5), islooping(true) //���� ��ũ�� ũ�⸸ŭ �ϳ��ϳ� ���� ����
	{
		//  ���ܿ� ���� ó�� �� �ʱ�ȭ
		bool faultyInput = false;
		if (this->width <= 0) {
			this->width = 10;
			faultyInput = true;
		}
		if (this->height <= 0) {
			this->height = 10;
			faultyInput = true;
		}
		size = (this->width + 1) * this->height;
		if (faultyInput == true) {
			delete cell;
			cell = new Cell * [size]; //Cell ���Ŀ� �°� ����
		}

		// memset�� �̿��� clear�Լ� ��ſ� ó�� ���� ����ִ� ��, ����X
		for (int i = 0; i < (width + 1) * height; i++)
		{
			char* shape = ToShape(Shape::UnClicked); //�ʱ� ���� UnClicked�� ����
			cell[i] = new Cell(i * ((int)strlen(shape)), i, shape); //���� �����ϴ� �迭, �� ���� ����, shape�� ��ĭ�� ��ƸԴ����� ����Ͽ� pos.x���� �Ѱ��ش�
		}

		setMine(); //���� �ɱ�
	}
	// destructor (�Ҹ��� �Լ�) �޸𸮰����󿡼� �������� ���� ȣ��Ǵ� �Լ�
	virtual ~Screen()
	{
		delete[] cell;
		cell = nullptr;
		width = 0; height = 0;
	}

	static Screen* Instance;

public:

	static Screen* GetInstance() {
		if (Instance == nullptr) {
			Instance = new Screen(10, 10);
		}
		return Instance;
	}

	//void draw(const Position& pos, Shape es, const Dimension& sz = Position{ 1, 1 })
	//{
	//	int offset = PositionToIndex(pos); //���� ���ϴ� ��ġ�� ���ѹ�

	//	char* shape = ToShape(es); //�ش��ϴ� ��� shape ������ ����
	//	cell[offset]->shape = shape; //����� shape�� ���� ����
	//}
	void draw(const Position& pos, const Dimension& sz = Position{ 1, 1 })
	{
		int offset = PositionToIndex(pos); //���� ���ϴ� ��ġ�� ���ѹ�

		char* clickedNotMineshape = ToShape(Shape::ClickedNotMine); //�ش��ϴ� ��� shape ������ ����
		char* clickedAndMineshape = ToShape(Shape::ClickedAndMine); //�ش��ϴ� ��� shape ������ ����

		//������ ���
		if (cell[offset]->isMine)
		{
			cell[offset]->shape = clickedAndMineshape;//����� shape�� ���� ����
			islooping = false;
		}
		//���ڰ� �ƴҰ��
		//if (!cell[offset]->isMine)
		//{
		//	cell[offset]->shape = clickedNotMineshape;//����� shape�� ���� ����
		//}
	}

	void render() //draw�Լ����� �׷��ְ� render�Լ����� �� ��ȯ ����
	{
		Borland::gotoxy(0, 0); //Ŀ���� ���� ��ġ�� �ٲ���´�

		char* newLineShape = ToShape(Shape::NewLine); //NewLine�� �ش��ϴ� ��� newLineShape ������ ����
		char* EndShape = ToShape(Shape::End); //End�� �ش��ϴ� ��� EndShape ������ ����

		cell[size - 1]->shape = EndShape; //������ ���� ����� EndShape�� ����

		for (int h = 0; h < height; h++)
		{
			Position endOfLine(width, h); //������ ���鼭 �� ���� ������ ���� ������ ����
			cell[PositionToIndex(endOfLine)]->shape = newLineShape; //�� ���� ������ ���� ���(char) ����

			for (int w = 0; w < width; w++)
			{
				printf("%s", cell[w + (h * width)]->shape); //�� ���鿡 ����� ����� ����Ѵ�
			}
		}
	}

	void setMine()
	{
		for (int i = 0; i < numberOfMine;) //���� ���� ä���������� �ݺ�
		{
			int index = rand() % size; //���� ��ȣ�� ����
			if (cell[index]->isMine == false) //���ڰ� �ƴϸ�
			{
				cell[index]->isMine = true; //���ڷ� �����
				i++;
			}
		}
	}

	void setFlag(const Position& pos) //��� �����
	{
		char* flagShape = ToShape(Shape::Flag); //�ش��ϴ� ��� shape ������ ����
		cell[PositionToIndex(pos)]->shape = flagShape;
	}

	int checkAround(int index) //�ֺ��� ���ڸ� ã�´�
	{
		/*int result = 0;
		if (cell[index - 11] != nullptr)
		{
			if (checkMine(index - 11))
			{
				result++;
			}
		}
		if (cell[index - 10] != nullptr)
		{
			if (checkMine(index - 10))
			{
				result++;
			}
		}
		if (cell[index - 9] != nullptr)
		{
			if (checkMine(index - 9))
			{
				result++;
			}
		}
		if (cell[index - 1] != nullptr)
		{
			if (checkMine(index - 1))
			{
				result++;
			}
		}
		if (cell[index + 1] != nullptr)
		{
			if (checkMine(index + 1))
			{
				result++;
			}
		}
		if (cell[index + 9] != nullptr)
		{
			if (checkMine(index + 9))
			{
				result++;
			}
		}
		if (cell[index + 10] != nullptr)
		{
			if (checkMine(index + 10))
			{
				result++;
			}
		}
		if (cell[index + 11] != nullptr)
		{
			if (checkMine(index + 11))
			{
				result++;
			}
		}
		return result;*/
		int result = 0;
		if (index - 11 > -1 && index - 11 < size)
		{
			if (checkMine(index - 11))
			{
				result++;
			}
		}
		if (index - 10 > -1 && index - 10 < size)
		{
			if (checkMine(index - 10))
			{
				result++;
			}
		}
		if (index - 9 > -1 && index - 9 < size)
		{
			if (checkMine(index - 9))
			{
				result++;
			}
		}
		if (index - 1 > -1 && index - 1 < size)
		{
			if (checkMine(index - 1))
			{
				result++;
			}
		}
		if (index + 1 > -1 && index + 1 < size)
		{
			if (checkMine(index + 1))
			{
				result++;
			}
		}
		if (index + 9 > -1 && index + 9 < size)
		{
			if (checkMine(index + 9))
			{
				result++;
			}
		}
		if (index + 10 > -1 && index + 10 < size)
		{
			if (checkMine(index + 10))
			{
				result++;
			}
		}
		if (index + 11 > -1 && index + 11 < size)
		{
			if (checkMine(index + 11))
			{
				result++;
			}
		}
		return result;
	}

	bool checkMine(int index) //�������� �ƴ��� �Ǵ��Ѵ�
	{
		if (cell[index]->isMine)
			return true;
		else return false;
	}

	int getWidth()
	{
		return width;
	}
	int getHeight()
	{
		return height;
	}
	int getSize()
	{
		return size;
	}
	void setClicked(const Position& pos)
	{
		int number = checkAround(PositionToIndex(pos));
		cell[PositionToIndex(pos)]->isClicked = true; //���� ����� �迭 �� ���콺 Ŭ�� �����ǰ� ���� �������� ���� �ִ� ����� isClicked ���� true�� ���� 

		if (number == 0)
		{
			char* zeroShape = ToShape(Shape::zero); //�ش��ϴ� ��� shape ������ ����
			cell[PositionToIndex(pos)]->shape = zeroShape;
		}
		else if (number == 1)
		{
			char* oneShape = ToShape(Shape::one); //�ش��ϴ� ��� shape ������ ����
			cell[PositionToIndex(pos)]->shape = oneShape;
		}
		else if (number == 2)
		{
			char* twoShape = ToShape(Shape::two); //�ش��ϴ� ��� shape ������ ����
			cell[PositionToIndex(pos)]->shape = twoShape;
		}
		else if (number == 3)
		{
			char* threeShape = ToShape(Shape::three); //�ش��ϴ� ��� shape ������ ����
			cell[PositionToIndex(pos)]->shape = threeShape;
		}
		else
		{
			char* clickedNotMineshape = ToShape(Shape::ClickedNotMine);
			cell[PositionToIndex(pos)]->shape = clickedNotMineshape;
		}
	}
	int PositionToIndex(const Position& pos)
	{
		return (width + 1) * pos.y + pos.x; //�Ű������� �Ѱܹ��� �������� ���� �ε��� ������ ��ȯ�ϴ� �Լ�
	}
	bool getIsLooping()
	{
		return islooping;
	}
};

class Input {
	DWORD cNumRead, fdwMode, i;
	INPUT_RECORD irInBuf[128];
	int counter;
	HANDLE hStdin;
	DWORD fdwSaveOldMode;
	char blankChars[80];

	void errorExit(const char*);
	void keyEventProc(KEY_EVENT_RECORD);
	void mouseEventProc(MOUSE_EVENT_RECORD, Screen* screen);
	void resizeEventProc(WINDOW_BUFFER_SIZE_RECORD);

	static Input* Instance; //�� ������Ʈ �ν��Ͻ��� ����� �� �ֵ��� Ŭ���� ���� ����

	Input() //������ �Լ��� ���η� ����� => ������ �Լ��� private �Լ���
	{
		memset(blankChars, ' ', 80);
		blankChars[79] = '\0';

		hStdin = GetStdHandle(STD_INPUT_HANDLE);
		if (hStdin == INVALID_HANDLE_VALUE)
			errorExit("GetStdHandle");
		if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
			errorExit("GetConsoleMode");
		/*
			   Step-1:
			   Disable 'Quick Edit Mode' option programmatically
		 */
		fdwMode = ENABLE_EXTENDED_FLAGS;
		if (!SetConsoleMode(hStdin, fdwMode))
			errorExit("SetConsoleMode");
		/*
		   Step-2:
		   Enable the window and mouse input events,
		   after you have already applied that 'ENABLE_EXTENDED_FLAGS'
		   to disable 'Quick Edit Mode'
		*/
		fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
		if (!SetConsoleMode(hStdin, fdwMode))
			errorExit("SetConsoleMode");
	}
	~Input()
	{
		SetConsoleMode(hStdin, fdwSaveOldMode);
	}

public:
	static Input* GetInstance()
	{
		if (Instance == nullptr) {
			Instance = new Input();
		}
		return Instance;
	}
	void ReadInputs(Screen* screen)
	{
		if (!GetNumberOfConsoleInputEvents(hStdin, &cNumRead)) {
			cNumRead = 0;
			return;
		}
		if (cNumRead == 0) return;

		Borland::gotoxy(0, 14);
		printf("number of inputs %d\n", cNumRead);

		if (!ReadConsoleInput(
			hStdin,      // input buffer handle
			irInBuf,     // buffer to read into
			128,         // size of read buffer
			&cNumRead)) // number of records read
			errorExit("ReadConsoleInput");
		// Dispatch the events to the appropriate handler.

		for (int i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT: // keyboard input
				keyEventProc(irInBuf[i].Event.KeyEvent);
				break;

			case MOUSE_EVENT: // mouse input
				mouseEventProc(irInBuf[i].Event.MouseEvent, screen);
				break;

			case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing
				resizeEventProc(irInBuf[i].Event.WindowBufferSizeEvent);
				break;

			case FOCUS_EVENT:  // disregard focus events

			case MENU_EVENT:   // disregard menu events
				break;

			default:
				errorExit("Unknown event type");
				break;
			}
		}
		Borland::gotoxy(0, 0);
	}
};

Screen* Screen::Instance = nullptr;
Input* Input::Instance = nullptr;

int main()
{
	Screen* screen = Screen::GetInstance();
	//Position pos{ 0, 0 };
	char shape[] = "��"; //ȭ�鿡 ������ ���
	Dimension sz{ (int)strlen(shape), 1 };

	Input* instance = Input::GetInstance();

	bool isLooping = true;
	while (isLooping) {

		instance->ReadInputs(screen);
		screen->render();
		if (!screen->getIsLooping()) isLooping = false;
		Sleep(100);

		//pos.x = (pos.x + 1) % (screen.getWidth()); //�׸� �����̴� �ڵ�
	}
	printf("\nGame Over\n");

	return 0;
}

void Input::errorExit(const char* lpszMessage)
{
	fprintf(stderr, "%s\n", lpszMessage);

	// Restore input mode on exit.

	SetConsoleMode(hStdin, fdwSaveOldMode);

	ExitProcess(0);
}

void Input::keyEventProc(KEY_EVENT_RECORD ker)
{
	Borland::gotoxy(0, 11); //Ư���� ��ġ�� �����Բ�
	printf("%s\r", blankChars);
	switch (ker.wVirtualKeyCode) {
	case VK_LBUTTON:
		printf("left button ");
		break;
	case VK_BACK:
		printf("back space");
		break;
	case VK_RETURN:
		printf("enter key");
		break;
	case VK_LEFT:
		printf("arrow left");
		break;
	case VK_UP:
		printf("arrow up");
		break;
	default:
		if (ker.wVirtualKeyCode >= 0x30 && ker.wVirtualKeyCode <= 0x39)
			printf("Key event: %c ", ker.wVirtualKeyCode - 0x30 + '0');
		else printf("Key event: %c ", ker.wVirtualKeyCode - 0x41 + 'A');
		break;
	}

	Borland::gotoxy(0, 0);
}

void Input::mouseEventProc(MOUSE_EVENT_RECORD mer, Screen* screen)
{
	Borland::gotoxy(0, 12);
	printf("%s\r", blankChars);
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif
	printf("Mouse event: ");

	switch (mer.dwEventFlags)
	{
	case 0:
		if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			printf("left button press %d %d\n", mer.dwMousePosition.X, mer.dwMousePosition.Y);
			//Ŭ���� x��ǥ�� ���� 2�� ���� ���� ����x ��ǥ�̴�
			screen->setClicked(Position(mer.dwMousePosition.X / 2, mer.dwMousePosition.Y)); //Ŭ���� �����ǿ� �ִ� ���� isClicked�� true�� ����
			screen->draw(Position(mer.dwMousePosition.X / 2, mer.dwMousePosition.Y)); //Ŭ���� �����ǿ� �ִ� ���� shape�� clicked�� ����
		}
		else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
		{
			printf("right button press \n");
			screen->setFlag(Position(mer.dwMousePosition.X / 2, mer.dwMousePosition.Y)); //��� �����
		}
		else
		{
			printf("button press\n");
		}
		break;
	case DOUBLE_CLICK:
		printf("double click\n");
		break;
	case MOUSE_HWHEELED:
		printf("horizontal mouse wheel\n");
		break;
	case MOUSE_MOVED:
		printf("mouse moved %d %d\n", mer.dwMousePosition.X, mer.dwMousePosition.Y);
		break;
	case MOUSE_WHEELED:
		printf("vertical mouse wheel\n");
		break;
	default:
		printf("unknown\n");
		break;
	}
	Borland::gotoxy(0, 0);
}

void Input::resizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
	Borland::gotoxy(0, 13);
	printf("%s\r", blankChars);
	printf("Resize event: ");
	printf("Console screen buffer is %d columns by %d rows.\n", wbsr.dwSize.X, wbsr.dwSize.Y);
	Borland::gotoxy(0, 0);
}