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

bool mixer_controller::send_float(const std::string &n_ch, const std::string &n_bus, float value)
{
  if (!isConnected)
    return false;
    
  // inizializza il messaggio OSC
  oscpkt::Message msg;
  std::string address = "/ch/" + n_ch + "/bus/" + n_bus + "/fader";
  msg.init(address).pushFloat(value);

  // creazine pacchetto OSC
  oscpkt::PacketWriter pw;
  pw.addMessage(msg);

  if (!pw.isOk())
    return false;

  // invia il pacchetto OSC tramite UDP
  return send_udp_packet(pw.packetData(), pw.packetSize());
}

bool mixer_controller::save_scene(int snap_index, std::string snap_name)
{
  if (!isConnected || snap_index < 1 || snap_index > 32)
    return false;
  
  std::string address = "/-snap/save/";
  oscpkt::Message msg;
  msg.init(address).pushInt32(snap_index).pushStr(snap_name);

  oscpkt::PacketWriter pw;
  pw.addMessage(msg);

  if (!pw.isOk())
    return false;

  return send_udp_packet(pw.packetData(), pw.packetSize());
}

bool mixer_controller::load_scene(int snap_index)
{
  if (!isConnected)
    return false;

  std::string address = "/-snap/recall/";
  oscpkt::Message msg;
  msg.init(address).pushInt32(snap_index);

  oscpkt::PacketWriter pw;
  pw.addMessage(msg);

  if (!pw.isOk())
    return false;

  return send_udp_packet(pw.packetData(), pw.packetSize());
}

bool mixer_controller::zero_bus(std::string bus, std::string keep_ch)
{
  if (!isConnected)
    return false;

  for (int ch = 1; ch <= 16; ch++)
  {
    std::string n_ch = (ch < 10) ? "0" + std::to_string(ch) : std::to_string(ch);
    // nodifica il livello di tutti i canali tranne quello da mantenere
    if (n_ch != keep_ch)
      send_float(n_ch, bus, 0.0f);
  }
  return true;
}