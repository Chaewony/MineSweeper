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

// 셀의 상태 타입 열거형
enum Shape {
	None = 0, //미지정
	UnClicked, //클릭 안된 셀
	ClickedNotMine, //클릭 됐고 지뢰가 아님 
	ClickedAndMine, //클릭됐고 지뢰임
	zero,
	one,
	two,
	three,
	Flag, //깃발
	NewLine, //\n
	End //종료점
};

// 셀의 상태 타입에 맞는 문자 지정
char* ToShape(Shape es)
{
	char* shape = new char;

	switch (es)
	{
	case UnClicked:
	{
		strncpy(shape, "■", sizeof("■"));
		break;
	}
	case ClickedNotMine:
	{
		strncpy(shape, "□", sizeof("□"));
		break;
	}
	case ClickedAndMine:
	{
		strncpy(shape, "★", sizeof("★"));
		break;
	}
	case zero:
	{
		strncpy(shape, "0 ", sizeof("□"));
		break;
	}
	case one:
	{
		strncpy(shape, "1 ", sizeof("□"));
		break;
	}
	case two:
	{
		strncpy(shape, "2 ", sizeof("□"));
		break;
	}
	case three:
	{
		strncpy(shape, "3 ", sizeof("□"));
		break;
	}
	case Flag:
	{
		strncpy(shape, "¶", sizeof("¶"));
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
	Cell** cell; // 칸 하나를 캔버스 전체중 하나의 요소로 보지 말고 셀 하나하나로 보자는 아이디어
				  // char형 배열 요소로만 봤을 경우 다른 정보를 담기(추가하기)에 한계가 있음
	int		numberOfMine; //심을 지뢰의 개수
	bool	islooping;

	// constructor (생성자 함수) 메모리공간상에 적재되는 순간 호출되는
	Screen(int width = 10, int height = 10)
		: width(width), height(height), cell(new Cell* [(width + 1) * height]), numberOfMine(5), islooping(true) //셀을 스크린 크기만큼 하나하나 동적 생성
	{
		//  예외에 대한 처리 및 초기화
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
			cell = new Cell * [size]; //Cell 형식에 맞게 수정
		}

		// memset을 이용한 clear함수 대신에 처음 셀들 깔아주는 것, 루프X
		for (int i = 0; i < (width + 1) * height; i++)
		{
			char* shape = ToShape(Shape::UnClicked); //초기 상태 UnClicked로 지정
			cell[i] = new Cell(i * ((int)strlen(shape)), i, shape); //셀을 저장하는 배열, 셀 동적 생성, shape가 몇칸씩 잡아먹는지를 고려하여 pos.x값을 넘겨준다
		}

		setMine(); //지뢰 심기
	}
	// destructor (소멸자 함수) 메모리공간상에서 없어지는 순간 호출되는 함수
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
	//	int offset = PositionToIndex(pos); //내가 원하는 위치의 셀넘버

	//	char* shape = ToShape(es); //해당하는 모양 shape 변수에 저장
	//	cell[offset]->shape = shape; //저장된 shape를 셀에 저장
	//}
	void draw(const Position& pos, const Dimension& sz = Position{ 1, 1 })
	{
		int offset = PositionToIndex(pos); //내가 원하는 위치의 셀넘버

		char* clickedNotMineshape = ToShape(Shape::ClickedNotMine); //해당하는 모양 shape 변수에 저장
		char* clickedAndMineshape = ToShape(Shape::ClickedAndMine); //해당하는 모양 shape 변수에 저장

		//지뢰일 경우
		if (cell[offset]->isMine)
		{
			cell[offset]->shape = clickedAndMineshape;//저장된 shape를 셀에 저장
			islooping = false;
		}
		//지뢰가 아닐경우
		//if (!cell[offset]->isMine)
		//{
		//	cell[offset]->shape = clickedNotMineshape;//저장된 shape를 셀에 저장
		//}
	}

	void render() //draw함수에서 그려주고 render함수에서 행 변환 해줌
	{
		Borland::gotoxy(0, 0); //커서를 원래 위치로 바꿔놓는다

		char* newLineShape = ToShape(Shape::NewLine); //NewLine에 해당하는 모양 newLineShape 변수에 저장
		char* EndShape = ToShape(Shape::End); //End에 해당하는 모양 EndShape 변수에 저장

		cell[size - 1]->shape = EndShape; //마지막 셀의 모양을 EndShape로 저장

		for (int h = 0; h < height; h++)
		{
			Position endOfLine(width, h); //포문을 돌면서 각 줄의 마지막 셀의 포지션 저장
			cell[PositionToIndex(endOfLine)]->shape = newLineShape; //각 줄의 마지막 셀의 모양(char) 저장

			for (int w = 0; w < width; w++)
			{
				printf("%s", cell[w + (h * width)]->shape); //각 셀들에 저장된 모양을 출력한다
			}
		}
	}

	void setMine()
	{
		for (int i = 0; i < numberOfMine;) //지뢰 갯수 채워질때까지 반복
		{
			int index = rand() % size; //랜덤 번호의 셀이
			if (cell[index]->isMine == false) //지뢰가 아니면
			{
				cell[index]->isMine = true; //지뢰로 만들기
				i++;
			}
		}
	}

	void setFlag(const Position& pos) //깃발 세우기
	{
		char* flagShape = ToShape(Shape::Flag); //해당하는 모양 shape 변수에 저장
		cell[PositionToIndex(pos)]->shape = flagShape;
	}

	int checkAround(int index) //주변의 지뢰를 찾는다
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

	bool checkMine(int index) //지뢰인지 아닌지 판단한다
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
		cell[PositionToIndex(pos)]->isClicked = true; //셀이 저장된 배열 중 마우스 클릭 포지션과 같은 포지션을 갖고 있는 요소의 isClicked 값을 true로 변경 

		if (number == 0)
		{
			char* zeroShape = ToShape(Shape::zero); //해당하는 모양 shape 변수에 저장
			cell[PositionToIndex(pos)]->shape = zeroShape;
		}
		else if (number == 1)
		{
			char* oneShape = ToShape(Shape::one); //해당하는 모양 shape 변수에 저장
			cell[PositionToIndex(pos)]->shape = oneShape;
		}
		else if (number == 2)
		{
			char* twoShape = ToShape(Shape::two); //해당하는 모양 shape 변수에 저장
			cell[PositionToIndex(pos)]->shape = twoShape;
		}
		else if (number == 3)
		{
			char* threeShape = ToShape(Shape::three); //해당하는 모양 shape 변수에 저장
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
		return (width + 1) * pos.y + pos.x; //매개변수로 넘겨받은 포지션을 셀의 인덱스 값으로 변환하는 함수
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

	static Input* Instance; //위 오브젝트 인스턴스가 저장될 수 있도록 클래스 내에 선언

	Input() //생성자 함수를 내부로 숨기기 => 생성자 함수를 private 함수로
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
	char shape[] = "■"; //화면에 나오는 모양
	Dimension sz{ (int)strlen(shape), 1 };

	Input* instance = Input::GetInstance();

	bool isLooping = true;
	while (isLooping) {

		instance->ReadInputs(screen);
		screen->render();
		if (!screen->getIsLooping()) isLooping = false;
		Sleep(100);

		//pos.x = (pos.x + 1) % (screen.getWidth()); //그림 움직이는 코드
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
	Borland::gotoxy(0, 11); //특정한 위치에 나오게끔
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
			//클릭된 x좌표의 값을 2로 나눈 몫이 셀의x 좌표이다
			screen->setClicked(Position(mer.dwMousePosition.X / 2, mer.dwMousePosition.Y)); //클릭된 포지션에 있는 셀의 isClicked값 true로 지정
			screen->draw(Position(mer.dwMousePosition.X / 2, mer.dwMousePosition.Y)); //클릭된 포지션에 있는 셀의 shape를 clicked로 지정
		}
		else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
		{
			printf("right button press \n");
			screen->setFlag(Position(mer.dwMousePosition.X / 2, mer.dwMousePosition.Y)); //깃발 세우기
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