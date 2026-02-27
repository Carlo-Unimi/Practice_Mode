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

  bool send_udp_packet(const void* data, size_t size);

public:
  bool isConnected;
  
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
   * @brief invia un messaggio OSC al mixer per impostare il valore di un fader in mandata ad un bus.
   * @param n_ch il numero del canale (01-16)
   * @param n_bus il numero del bus (01-06)
   * @param value il valore da inviare al fader (0.0-1.0)
   * @return true se l'invio è avvenuto con successo, false altrimenti
   */
  bool send_float(const std::string &n_ch, const std::string &n_bus, float value);

  /**
   * @brief invia il messaggio OSC di salvataggio scena al mixer nello slot specificato.
   * @param snap_index il numero dello snapshot in cui salvare la scena corrente (1-32)
   * @param snap_name il nome della scena da salvare (default: practice_mode)
   * @return true se l'invio è avvenuto con successo, false altrimenti
   */
  bool save_scene(int snap_index = 1, std::string snap_name = "practice_mode");

  /**
   * @brief invia il messaggio OSC di caricamento scena al mixer nello slot specificato.
   * @param snap_index il numero dello snapshot da caricare (1-32)
   * @return true se l'invio è avvenuto con successo, false altrimenti
   */
  bool load_scene(int snap_index = 1);

  /**
   * @brief azzera i fader di una mandata bus, a parte quello specificato nel parametro 'keep_ch'.
   * @param keep_ch il numero di canale da mantenere (01-16)
   * @param bus il numero di bus su cui operare (01-06)
   * @return true se l'invio è avvenuto con successo, false altrimenti
   */
  bool zero_bus(std::string bus, std::string keep_ch);
};

#endif