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

bool mixer_controller::probe(int timeout_sec)
{
  // invia un messaggio OSC '/xinfo' al mixer
  oscpkt::Message msg;
  msg.init("/xinfo");

  oscpkt::PacketWriter pw;
  pw.addMessage(msg);

  if (!pw.isOk())
    return false;

  int sent = ::sendto(sock_fd, static_cast<const char *>(pw.packetData()), static_cast<int>(pw.packetSize()), 0,
                      reinterpret_cast<const sockaddr *>(&remote_addr), sizeof(remote_addr));
  if (sent <= 0)
    return false;

  // attende una risposta con timeout
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(sock_fd, &read_fds);

  struct timeval tv;
  tv.tv_sec = timeout_sec;
  tv.tv_usec = 0;

  int result = ::select(sock_fd + 1, &read_fds, nullptr, nullptr, &tv);
  if (result > 0 && FD_ISSET(sock_fd, &read_fds))
  {
    char buf[512];
    int n = ::recvfrom(sock_fd, buf, sizeof(buf), 0, nullptr, nullptr);
    return (n > 0);
  }

  return false;
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

  // verifica che il mixer sia effettivamente raggiungibile
  isConnected = probe();
  if (!isConnected)
    disconnect();

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
  std::string address = "/ch/" + n_ch + "/mix/" + n_bus + "/level";
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

  // imposta l'indice dello snapshot
  oscpkt::Message idx_msg;
  idx_msg.init("/-snap/index").pushInt32(snap_index);

  oscpkt::PacketWriter idx_pw;
  idx_pw.addMessage(idx_msg);
  if (!idx_pw.isOk() || !send_udp_packet(idx_pw.packetData(), idx_pw.packetSize()))
    return false;

  // imposta il nome dello snapshot
  oscpkt::Message name_msg;
  name_msg.init("/-snap/name").pushStr(snap_name);

  oscpkt::PacketWriter name_pw;
  name_pw.addMessage(name_msg);
  if (!name_pw.isOk() || !send_udp_packet(name_pw.packetData(), name_pw.packetSize()))
    return false;

  // salva lo snapshot
  oscpkt::Message msg;
  msg.init("/-snap/save");

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

  // imposta l'indice dello snapshot da caricare
  oscpkt::Message idx_msg;
  idx_msg.init("/-snap/index").pushInt32(snap_index);

  oscpkt::PacketWriter idx_pw;
  idx_pw.addMessage(idx_msg);
  if (!idx_pw.isOk() || !send_udp_packet(idx_pw.packetData(), idx_pw.packetSize()))
    return false;

  // carica lo snapshot
  oscpkt::Message msg;
  msg.init("/-snap/load");

  oscpkt::PacketWriter pw;
  pw.addMessage(msg);

  if (!pw.isOk())
    return false;

  return send_udp_packet(pw.packetData(), pw.packetSize());
}

void mixer_controller::set_ip(const std::string &ip)
{
  this->mixer_ip = ip;
}

void mixer_controller::set_port(uint16_t port)
{
  this->mixer_port = port;
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