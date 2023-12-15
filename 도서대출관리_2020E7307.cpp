#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#define NOMINMAX
#include <Windows.h>
//관리자 비밀번호
#define Admin_passwd "rlaalsgks"

using namespace std;


void showBooks();

int bookOptions() {


	int selectedOption = 0;
	cout << "옵션: ";
	cin >> selectedOption;
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	return selectedOption;
}

void clearScreen() {//화면 지우기
	system("cls");
}

void addBook() { //Admin 함수
	clearScreen();
	string bookTitle, bookAuthor, bookDetail, line;
	int count = 0;

	ifstream countData("도서관리.dat");
	while (getline(countData, line)) {
		stringstream bookInfo(line);
		int bookNumber;
		char comma;
		bookInfo >> bookNumber >> comma; // 책 번호와 쉼표(,)를 읽어옴
		count = max(count, bookNumber);
	}

	countData.close();

	count++; // 다음 책 번호 설정

	cout << "--------------------------------------" << endl;
	cout << "|             책 추가                  |" << endl;
	cout << "--------------------------------------" << endl;
	cout << "책의 제목: ";
	getline(cin, bookTitle);
	cout << "책의 저자: ";
	getline(cin, bookAuthor);

	ofstream writeToLibrary;

	writeToLibrary.open("도서관리.dat", ios::app);

	// 변경된 저장 포맷에 맞춰서 저장
	bookDetail = to_string(count) + ", " + bookTitle + ", " + bookAuthor + ", 0"; // 초기 대여 상태 0으로 설정

	writeToLibrary << bookDetail << endl;

	writeToLibrary.close();

	cout << "추가된 책 정보: " << bookDetail << endl;
}

void UpdateBook() { //Admin 함수
	clearScreen();
	cout << "--------------------------------------" << endl;
	cout << "|             책 수정                 |" << endl;
	cout << "--------------------------------------" << endl;
	string id;
	cout << "책의 번호(ID)를 입력: ";
	cin >> id;
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	bool bookExist = false;
	ifstream data("도서관리.dat");
	ofstream temp("temp.dat");

	if (!data || !temp)
	{
		cout << "파일 불러오기 실패" << endl;
		return;
	}

	string strTemp;
	while (getline(data, strTemp))
	{
		size_t found = strTemp.find(id);
		if (found != string::npos) {
			string title, author, status;

			cout << "-> 책 수정 대상: " << strTemp << endl;

			cout << "수정하는 책의 제목: ";
			getline(cin, title);
			cout << "수정하는 책의 저자: ";
			getline(cin, author);

			size_t pos = strTemp.rfind(","); // 대여 상태 위치 확인
			status = strTemp.substr(pos + 1); // 현재 대여 상태

			strTemp = id + ", " + title + ", " + author + ", " + status;
			bookExist = true;
		}
		temp << strTemp << endl;
	}

	data.close();
	temp.close();

	if (bookExist) {
		remove("도서관리.dat");
		rename("temp.dat", "도서관리.dat");

		cout << "책 정보 수정 완료" << endl;
	}
	else cout << id << "번 의 책이 존재하지 않습니다." << endl;
}


void DeleteBook() {//Admin 함수
	clearScreen();
	showBooks();
	cout << endl << "--- 책 삭제 ---" << endl;
	string id;
	cout << "삭제할 책의 번호(ID): ";
	cin >> id;
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	bool bookExist = false;
	ifstream data("도서관리.dat");
	ofstream temp;
	temp.open("temp.dat", ios::app); //임시파일

	if (!data || !temp)
	{
		cout << "파일 불러오기 오류" << endl;
		return;
	}

	string strTemp;
	bool delBook = false;
	while (getline(data, strTemp))
	{
		size_t found = strTemp.find(id);
		if (found != string::npos) {
			cout << "삭제된 책 정보: " << strTemp << endl;
			bookExist = true;
			delBook = true;
		}
		if (delBook) {
			delBook = false;
			continue;
		}
		temp << strTemp << endl;
	}

	data.close();
	temp.close();

	if (bookExist) {

		if (remove("도서관리.dat") != 0)
			perror("Error deleting file");


		if (rename("temp.dat", "도서관리.dat")) {
			perror("Error renaming");
			return;
		}

		cout << id << "번의 책이 삭제되었습니다." << endl;

	}
	else cout << id << "번의 책이 존재하지 않습니다." << endl;
}


void showBooks() { //공통 함수
	clearScreen();
	cout << "--- 저장된 책 목록 ---" << endl;

	ifstream data("도서관리.dat");
	string row;
	while (getline(data, row)) {
		stringstream bookInfo(row);
		string bookNumberStr, bookTitle, bookAuthor, bookStatus;

		getline(bookInfo, bookNumberStr, ',');
		int bookNum = stoi(bookNumberStr);

		getline(bookInfo >> ws, bookTitle, ',');
		getline(bookInfo >> ws, bookAuthor, ',');
		getline(bookInfo >> ws, bookStatus, ',');

		// 공백 제거
		bookStatus.erase(remove_if(bookStatus.begin(), bookStatus.end(), ::isspace), bookStatus.end());

		cout << bookNum << "번, " << bookTitle << ", " << bookAuthor << ", ";

		if (bookStatus == "1") {
			cout << "대여 중" << endl;
		}
		else {
			cout << "대여 가능" << endl;
		}
	}
}

void searchBook() {//유저함수
	clearScreen();
	cout << "--------------------------------------" << endl;
	cout << "|            책 검색                  |" << endl;
	cout << "--------------------------------------" << endl;
	string searchTitle;
	cout << "검색할 책 제목을 입력하세요: ";
	getline(cin, searchTitle);

	// 검색어에서 특수문자 및 공백 제거
	searchTitle.erase(std::remove_if(searchTitle.begin(), searchTitle.end(), [](unsigned char c) { return !std::isalnum(c); }), searchTitle.end());

	ifstream data("도서관리.dat");
	string row;
	bool found = false;
	while (getline(data, row)) {
		stringstream bookInfo(row);
		int bookNum;
		string bookTitle, bookAuthor, bookStatus;

		bookInfo >> bookNum;
		bookInfo.ignore(1); // 두 번째 필드 구분 기호(,)를 무시하여 문자열 읽기 시작
		getline(bookInfo, bookTitle, ',');
		getline(bookInfo, bookAuthor, ',');
		getline(bookInfo, bookStatus);

		// 책 제목에서 특수문자 및 공백 제거
		string sanitizedTitle = bookTitle;
		sanitizedTitle.erase(std::remove_if(sanitizedTitle.begin(), sanitizedTitle.end(), [](unsigned char c) { return !std::isalnum(c); }), sanitizedTitle.end());

		if (sanitizedTitle == searchTitle) {
			string Status;
			if (bookStatus == "1") {
				Status = "대여 중";
			}
			else {
				Status = "대여 가능!";
			}
			cout << "==========책 정 보==========\n";
			cout << "번호 : " << bookNum << "번\n";
			cout << "제목 : " << bookTitle << "\n";
			cout << "저자 : " << bookAuthor << "\n";
			cout << "대출상태 : " << Status << "\n";
			cout << "============================\n";


			found = true;
			break;
		}
	}

	if (!found) {
		cout << "책을 찾을 수 없습니다." << endl;
	}
}

void bookRent() {//유저 함수
	clearScreen();
	cout << "--------------------------------------" << endl;
	cout << "|            도서 대여                 |" << endl;
	cout << "--------------------------------------" << endl;
	showBooks();

	cout << "대여할 책의 번호를 입력하세요: ";
	int bookNumber;
	cin >> bookNumber;
	cin.ignore();

	ifstream data("도서관리.dat");
	ofstream temp("temp.dat");

	bool found = false;
	string row;
	while (getline(data, row)) {
		stringstream bookInfo(row);
		int num;
		string title, author;
		int status; // 대여 상태를 정수로 저장

		// 쉼표(,)를 기준으로 필드를 읽어옴
		bookInfo >> num;
		bookInfo.ignore(); // 쉼표를 무시
		getline(bookInfo, title, ',');
		getline(bookInfo, author, ',');
		bookInfo >> status;

		if (num == bookNumber) {
			found = true;
			if (status == 0) { // 문자열 비교에서 정수 비교로 변경
				temp << num << "," << title << "," << author << ", 1" << endl;
				cout << "대여 완료" << endl;
			}
			else {
				temp << row << endl;
				cout << "이미 대여 중인 책입니다." << endl;
			}
		}
		else {
			temp << row << endl;
		}
	}

	if (!found) {
		cout << "책을 찾을 수 없습니다." << endl;
	}

	data.close();
	temp.close();

	remove("도서관리.dat");
	rename("temp.dat", "도서관리.dat");
}

void bookReturn() {//유저 함수
	clearScreen();
	cout << "--------------------------------------" << endl;
	cout << "|            도서 반납                 |" << endl;
	cout << "--------------------------------------" << endl;
	showBooks();

	cout << "반납할 책의 번호를 입력하세요: ";
	int bookNumber;
	cin >> bookNumber;
	cin.ignore();

	ifstream data("도서관리.dat");
	ofstream temp("temp.dat");

	bool found = false;
	string row;
	while (getline(data, row)) {
		stringstream bookInfo(row);
		int num;
		string title, author;
		int status; // 대여 상태를 정수로 저장

		bookInfo >> num;
		bookInfo.ignore(); // 쉼표를 무시
		getline(bookInfo, title, ',');
		getline(bookInfo, author, ',');
		bookInfo >> status;

		if (num == bookNumber) {
			found = true;
			if (status == 1) { // status를 문자열에서 정수로 변경
				temp << num << "," << title << "," << author << ", 0" << endl; // 반납 완료로 상태 변경
				cout << "반납 완료!" << endl;
			}
			else {
				temp << row << endl;
				cout << "이미 반납완료된 책입니다." << endl;
			}
		}
		else {
			temp << row << endl;
		}
	}

	if (!found) {
		cout << "존재하지 않는 책 번호입니다." << endl;
	}

	data.close();
	temp.close();

	remove("도서관리.dat");
	rename("temp.dat", "도서관리.dat");
}

stringstream currentdateAndSec() {//메인화면에 현재시간 띄우는 함수
	time_t now = time(0);
	tm* ltm = localtime(&now);

	// 시간 형식 변경을 위한 stringstream 사용
	stringstream currentdateS;
	currentdateS << setw(4) << ltm->tm_year + 1900 << "년 " << setw(2) << ltm->tm_mon + 1 << "월 " << setw(2) << ltm->tm_mday << "일 - ";

	// 오전/오후 표시를 위한 처리
	string amOrPm = (ltm->tm_hour < 12) ? "오전" : "오후";
	int hour = (ltm->tm_hour < 12) ? ltm->tm_hour : ltm->tm_hour - 12;

	currentdateS << amOrPm << setw(2) << hour << "시 " << setw(2) << ltm->tm_min << "분";
	return currentdateS;
}

void displayMenuAdmin() {
	stringstream st = currentdateAndSec();
	cout << endl;
	cout << "       " << st.str() << endl;
	cout << "--------------------------------------" << endl;
	cout << "|       도서 관리 - 관리자 모드       |" << endl;
	cout << "--------------------------------------" << endl;
	cout << "| 1. 책 추가                          |" << endl;
	cout << "| 2. 책 수정                          |" << endl;
	cout << "| 3. 책 삭제                          |" << endl;
	cout << "| 4. 저장된 책 목록 보기              |" << endl;
	cout << "| 0. 관리자 모드 종료                 |" << endl;
	cout << "--------------------------------------" << endl;
}

void UserMain() {
	stringstream st = currentdateAndSec();
	cout << endl;
	cout << "       " << st.str() << endl;
	cout << "--------------------------------------" << endl;
	cout << "|       안양대 미니 도서관 관리       |" << endl;
	cout << "--------------------------------------" << endl;
	cout << "| 1. 도서 검색                        |" << endl;
	cout << "| 2. 도서 목록                        |" << endl;
	cout << "| 3. 도서 대여                        |" << endl;
	cout << "| 4. 도서 반납                        |" << endl;
	cout << "| 5. 관리자 모드                      |" << endl;
	cout << "| 0. 프로그램 종료                    |" << endl;
	cout << "--------------------------------------" << endl;
}

void showNotification(const string& message) {
	MessageBoxA(NULL, message.c_str(), "알림", MB_OK | MB_ICONINFORMATION);
}

void changeAdminMode() {//관리자모드 진입
	while (true) {
		string password;
		clearScreen();
		cout << "관리자 암호를 입력하세요: ";
		cin >> password;

		if (password != Admin_passwd) {
			cout << "패스워드가 일치하지 않습니다.\n";
			cout << "1. 다시 시도하기\n2. 관리자 모드 진입 취소하기\n";
			int choice;
			cin >> choice;

			if (choice == 2) {
				cout << "메인화면으로 돌아갑니다." << endl;
				return;
			}
		}
		else {
			showNotification("Admin님 환영합니다.");
			while (true) {
				clearScreen();
				string yn;

				cout << endl << "--- 안양대 미니 도서관 관리자 버전 ---" << endl;
				displayMenuAdmin();

				int adminOption = bookOptions();

				switch (adminOption) {
				case 1:
					addBook();
					break;
				case 2:
					UpdateBook();
					break;
				case 3:
					DeleteBook();
					break;
				case 4:
					showBooks();
					break;
				case 0:
					return;
				default:
					cout << "유요하지 않은 옵션입니다. 다시 적어주세요." << endl;
					break;
				}

				cout << endl << "관리자 모드를 계속 진행할까요? (Y/N) :";
				cin >> yn;
				if (yn != "y" && yn != "Y") {
					return;
				}
				else {
					clearScreen();
				}
			}
		}
	}
}

int main() {
	cout << "	*** AYU Mini Library ***";
	string yn;
	while (true) {
		cout << endl << "--- 안양대 미니 도서관 책 대출 프로그램 ---" << endl;
		UserMain();

		int option = bookOptions();

		switch (option) {
		case 1:
			searchBook();
			break;
		case 2:
			showBooks();
			break;
		case 3:
			bookRent();
			break;
		case 4:
			bookReturn();
			break;
		case 5:
			changeAdminMode();
			break;
		case 0:
			exit(0);
			break;
		default:
			cout << "유요하지 않은 옵션입니다. 다시 적어주세요." << endl;
			break;
		}

		cout << endl << "계속 진행할까요? (Y/N) :";
		cin >> yn;
		if (yn != "y" && yn != "Y") {
			break;
		}
		else {
			clearScreen();
		}
	}

	return 0;
}
