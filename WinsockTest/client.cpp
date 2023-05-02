// 소켓을 사용하기 위해서 라이브러리 참조해야 한다.
#pragma comment(lib, "ws2_32")
// inet_ntoa가 deprecated가 되었는데.. 사용하려면 아래 설정을 해야 한다.
#pragma warning(disable:4996)
#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>
// 소켓을 사용하기 위한 라이브러리
#include <WinSock2.h>
// 수신 버퍼 사이즈
#define BUFFERSIZE 1024
// 실행 함수
int main()
{
  // 파일 리소스 취득
  FILE* fp = fopen("d:\\work\\nowonbuntistory.png", "rb");
  // 취득 실패하면 종료한다.
  if (fp == NULL) 
  {
    cout << "File open failed" << endl;
    return 1;
  }
  // 파일 길이 취득
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  // 파일 데이터를 읽어온다.
  unsigned char* data = new unsigned char[size];
  fread(data, 1, size, fp);
  // 파일 리소스 닫기
  fclose(fp);
 
  // 소켓 정보 데이터 설정
  WSADATA wsaData;
  // 소켓 실행.
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
  {
    return 1;
  }
  // Internet의 Stream 방식으로 소켓 생성 
  SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
  // 소켓 주소 설정
  SOCKADDR_IN addr;
  // 구조체 초기화
  memset(&addr, 0, sizeof(addr));
  // 소켓은 Internet 타입
  addr.sin_family = AF_INET;
  // 127.0.0.1(localhost)로 접속하기
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // 포트 9090으로 접속
  addr.sin_port = htons(9090);
  // 접속
  if (connect(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
  {
    // 에러 콘솔 출력
    cout << "error" << endl;
    return 1;
  }
  // 업로드 파일 이름 전송
  const wchar_t* filename = L"Download2.png";
  // unicode에서 utf8형식으로 변환
  char buffer[BUFFERSIZE];
  WideCharToMultiByte(CP_ACP, 0, filename, wcslen(filename) * 2, buffer, BUFFERSIZE, NULL, NULL);
  // 파일 이름 길이
  int len = strlen(buffer);
  // 파일 이름 길이 전송
  send(sock, (char*)&len, 4, 0);
  // 파일 이름 전송
  send(sock, (char*)buffer, len, 0);
  // 파일 사이즈 전송
  send(sock, (char*)&size, 4, 0);
  // 파일 데이터 전송
  send(sock, (char*)data, size, 0);
  // 데이터 메모리 해제
  delete data;
  // 수신 대기 byte=1오면 종료한다.
  unsigned char ret[1];
  recv(sock, (char*)ret, 1, 0);
  if (ret[0] == 1)
  {
    cout << "Completed" << endl;
  }
  // 서버 소켓 종료
  closesocket(sock);
  // 소켓 종료
  WSACleanup();
  return 0;
}
