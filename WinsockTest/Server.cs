using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Threading;
 
namespace Example
{
  class Program
  {
    // 실행 함수
    static void Main(string[] args)
    {
      // 포트 9090으로 서버를 설정한다.
      IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 9090);
      // 소켓을 얻는다.
      using (Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp))
      {
        // bind하고 Listen 대기한다.
        server.Bind(ipep);
        server.Listen(20);
        // 콘솔 출력.
        Console.WriteLine("Server Start... Listen port 9090...");
        // 다중 접속 환경
        while (true)
        {
          // 접속을 대기힌다.
          ThreadPool.QueueUserWorkItem((_) =>
          {
            Socket client = (Socket)_;
            // 클라이언트 정보 취득
            IPEndPoint ip = (IPEndPoint)client.RemoteEndPoint;
            // 클라이언트 정보 콘솔 출력
            Console.WriteLine("Client connected IP address = {0} : {1}", ip.Address, ip.Port);
            // 취득 람다 함수식 - 데이터의 사이즈를 받고 데이터를 받는다.
            Func<byte[]> Receive = () =>
            {
              // big endian타입으로 데이터 크기를 받는다.
              var data = new byte[4];
              client.Receive(data, SocketFlags.None);
              // binary 객체 선언
              data = new byte[BitConverter.ToInt32(data, 0)];
              // 데이터 받기
              client.Receive(data, SocketFlags.None);
              return data;
            };
            // 파일 이름 받기
            var filename = Encoding.UTF8.GetString(Receive());
            // 파일 데이터를 받고 저장한다.
            File.WriteAllBytes("d:\\work\\" + filename, Receive());
            // byte=1의 데이터를 보낸다.
            client.Send(new byte[1] { 1 }, SocketFlags.None);
            // 접속 종료 정보 콘솔 출력
            Console.WriteLine("Client disconnected IP address = {0} : {1}", ip.Address, ip.Port);
          }, server.Accept());
        }
      }
    }
  }
}
