#ifndef MIXER_CONTROLLER
#define MIXER_CONTROLLER

#include <string>
#include <cstdint>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include "../oscpkt.hh"

/**
 * @class mixer_controller
 * @brief manager di connessione e invio messaggi OSC tramite UDP al mixer.
 */
class mixer_controller
{
private:
#ifdef _WIN32
  SOCKET sock_fd;
#else
  int sock_fd;
#endif
  sockaddr_in remote_addr;

  std::string mixer_ip;
  uint16_t mixer_port;
  bool isConnected;
  oscpkt::Message msg;

  bool send_udp_packet(const void* data, size_t size);

public:
  /**
   * @brief costruttore del mixer_controller.
   * @param ip l'indirizzo IP del mixer
   * @param port la porta UDP su cui il mixer è in ascolto
   */
  mixer_controller(std::string ip, uint16_t port);
  
  /**
   * @brief distruttore del mixer_controller, chiude la connessione se ancora aperta.
   */
  ~mixer_controller();

  /**
   * @brief stabilisce la connessione al mixer.
   */
  bool connect();
  
  /**
   * @brief chiude la connessione al mixer.
   */
  void disconnect();

  /**
   * @brief invia un messaggio OSC al mixer.
   * @param address il path dell'indirizzo OSC
   * @param msg il messaggio OSC da inviare, già costruito con i relativi argomenti
   * @return true se l'invio è avvenuto con successo, false altrimenti
   */
  bool send_float(const std::string& address, float value);
};

#endif