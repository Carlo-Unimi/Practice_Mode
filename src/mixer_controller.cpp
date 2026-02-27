#include "../include/mixer_controller.h"

mixer_controller::mixer_controller(std::string ip, uint16_t port) : mixer_ip(ip), mixer_port(port), isConnected(false)
{
  #ifdef _WIN32
    sock_fd = INVALID_SOCKET;
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
  #else
    sock_fd = -1;
  #endif
    memset(&remote_addr, 0, sizeof(remote_addr));
}

mixer_controller::~mixer_controller()
{
  disconnect();
  #ifdef _WIN32
    WSACleanup();
  #endif
}

bool mixer_controller::connect()
{
  if (this->isConnected)
    disconnect();

  // crea socket UDP
  sock_fd = ::socket(AF_INET, SOCK_DGRAM, 0);

  #ifdef _WIN32
    if (sock_fd == INVALID_SOCKET)
      return false;
  #else
    if (sock_fd < 0)
      return false;
  #endif

  // configurazione indirizzo remoto
  remote_addr.sin_family = AF_INET;
  remote_addr.sin_port = htons(mixer_port);

  if (::inet_pton(AF_INET, mixer_ip.c_str(), &remote_addr.sin_addr) <= 0)
  {
    disconnect();
    return false;
  }

  isConnected = true;
  return isConnected;
}

void mixer_controller::disconnect()
{
  isConnected = false;
  #ifdef _WIN32
    if (sock_fd != INVALID_SOCKET)
    {
      closesocket(sock_fd);
      sock_fd = INVALID_SOCKET;
    }
  #else
    if (sock_fd >= 0)
    {
      ::close(sock_fd);
      sock_fd = -1;
    }
  #endif
}

bool mixer_controller::send_udp_packet(const void *data, size_t size)
{
  if (!isConnected || data == nullptr || size == 0)
    return false;

  int sent_bytes = ::sendto(sock_fd, static_cast<const char *>(data), static_cast<int>(size), 0, reinterpret_cast<const sockaddr *>(&remote_addr), sizeof(remote_addr));

  return (sent_bytes == static_cast<int>(size));
}

bool mixer_controller::send_float(const std::string &address, float value)
{
  if (!isConnected)
    return false;

  // inizializza il messaggio OSC
  msg.init(address).pushFloat(value);

  // creazine pacchetto OSC
  oscpkt::PacketWriter pw;
  pw.addMessage(msg);

  if (!pw.isOk())
    return false;

  // invia il pacchetto OSC tramite UDP
  return send_udp_packet(pw.packetData(), pw.packetSize());
}