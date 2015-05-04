#ifndef CONNECTION_INITIATOR_BASE_H
#define CONNECTION_INITIATOR_BASE_H

#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "glob.h"

const string TMP_CERT = \
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDXTCCAkWgAwIBAgIJAI42ahOdjmOeMA0GCSqGSIb3DQEBCwUAMEUxCzAJBgNV\n"
    "BAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYDVQQKDBhJbnRlcm5ldCBX\n"
    "aWRnaXRzIFB0eSBMdGQwHhcNMTUwNDI3MTQyODE4WhcNMTUwNTI3MTQyODE4WjBF\n"
    "MQswCQYDVQQGEwJBVTETMBEGA1UECAwKU29tZS1TdGF0ZTEhMB8GA1UECgwYSW50\n"
    "ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB\n"
    "CgKCAQEApulNN3PaWBdfGrWNm6AqRsLpI9/Q3T7TmKbSdTWUJcYndQdMJJ2DERKX\n"
    "KiNDKM294QfXld81HpaecFaBkLQ8aETAi/0/1bVe+5GSUuskIpEYRb175kafO4yS\n"
    "6Ewj8hTJihNICMINo6tw4fAV8ULv4VUKliEwoqqLR3KLuw6fwnmUBpybNp5fTgRz\n"
    "xIPtMDxxCFyvh+J347K3JtIgS3iBvWc9zws05+uW3CB5u09JsCjaqJ0BTRcccYfL\n"
    "UmxrJSb+WKgGjyQblYqLfsdu78DgyBeelDQtuGy8uTcN1HMb23ogAyHT19fOg2xt\n"
    "QrHY+Av9Uz4E6NV9Bhhw/KQVCcqSmQIDAQABo1AwTjAdBgNVHQ4EFgQUUZLkMcfC\n"
    "CSiEX21uMpBpbQYjxTcwHwYDVR0jBBgwFoAUUZLkMcfCCSiEX21uMpBpbQYjxTcw\n"
    "DAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAeo7ip9r4fulslrttEl+B\n"
    "bTGDqL16NpTwt7J6Pk7RotifBVymgOqw/sH33FrHt+/oFPc9dneXLT4ed8dgaL3W\n"
    "H28C7W2bzEVENNEbqVV0enLWY2s+Qi74WwfOcKE3l2vfTVMnDB/dZCcsl+QjoOTw\n"
    "NkgapcxOoFruBAa83ikV9y9sZ8P6vNt2FaF+1oT6ZiyWQGjf++18cHN37ZOCNne1\n"
    "H4XARgMZUzvWe9lOyfCqfQZ1hCbTHT/8pUsrCe9wkXuxXZ2pfgv98ckjNTKoW22L\n"
    "1F1N6LcPsJzHVzFGidYenNgzrJh2dTpvo6g7YwBh/JcHek8B2l6Y+wz/Azm8E/eG\n"
    "Aw==\n"
    "-----END CERTIFICATE----- \n";

const string TMP_KEY = \
    "-----BEGIN PRIVATE KEY-----\n"
    "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCm6U03c9pYF18a\n"
    "tY2boCpGwukj39DdPtOYptJ1NZQlxid1B0wknYMREpcqI0Mozb3hB9eV3zUelp5w\n"
    "VoGQtDxoRMCL/T/VtV77kZJS6yQikRhFvXvmRp87jJLoTCPyFMmKE0gIwg2jq3Dh\n"
    "8BXxQu/hVQqWITCiqotHcou7Dp/CeZQGnJs2nl9OBHPEg+0wPHEIXK+H4nfjsrcm\n"
    "0iBLeIG9Zz3PCzTn65bcIHm7T0mwKNqonQFNFxxxh8tSbGslJv5YqAaPJBuViot+\n"
    "x27vwODIF56UNC24bLy5Nw3UcxvbeiADIdPX186DbG1Csdj4C/1TPgTo1X0GGHD8\n"
    "pBUJypKZAgMBAAECggEAOF0iYS5zNjo2OHvEpSASBrE9L4seKghURgFXopMEQzlG\n"
    "C8P6F4dFQlenymf7/uRzUXFngMnOU1UDjeX4fNgQahmFeAB+NPm5aHSBGBUlr7KV\n"
    "or9Y/qssjFxtI5rKrK4OXN0j/ykgl/uF9BYyIrW68bnZCFeO5+91dxwcSX+nvXDr\n"
    "E74Y9ET6JhDP/tvemWZXo8XyW2Wvoy/pzbsk/5CSm5WLPc97Qfna7bAaS5R2f60M\n"
    "tnjqReg8JRDDPy45/XjB0snNsBsR8DzXTg+mPEzSbcu2DVa2vfOfPe50NzP3KT8J\n"
    "VeAT7SZIljKI5O9YgUCtaA4rrSgMURHgiEnrNXC9MQKBgQDagzRRD1Vr5DAOjPYn\n"
    "skgdqeRnR3qhEDReRPVsndHUFsrx2wVgmEV5E0bB3KAjjXAwq6Cdgt6F+NRimXuY\n"
    "ObIc6bBm0DznWuX4uBcPQXtrCdPP9WC/kUUNaW7qpb1HxdbCKt38VSmRedlVi8lP\n"
    "ZOK39KQyFTTBe//pPIqMQtaYxQKBgQDDi9aK+TB4HXYqeyrUV29zEvw4Liy9kHEp\n"
    "Dgn38/aVool6eTm+UOiPoxUPRBB9CmCD6eF2sXjp+UNOElQp2lbwxvcn4m3/2RLc\n"
    "77gnvbCrpvF1vzHGFM4F40s642F2a48jz83s2Y/C2ych2o56vQiM7diOnvnR4u6O\n"
    "dSYXP+0nxQKBgEHew2qcILgBdastOxcEW1D14fBwT7s101OHniQ7/wDcThlSWPCh\n"
    "dm/lK7NM+sk83dEOK8DPrpzcQcnnUqa03chxecfEhleHw0OS2wCyUK4XrE5IPf4B\n"
    "oLtGSTVa+RAlx3OI0Jo8YeeEv8tV9D2GwAGMMeMXGWwqIR8CLRHTJDWxAoGAXkup\n"
    "mnyNve5fFB8uyRScvr4R+Dl0A7mKiASoE/SGHSQy7WdVzAuSCenXdvQYi2OcpR/8\n"
    "RFNSfmlf5l76TPBn6lXXy1jqOG0CJp5GjItxjWTEnUYkh2Ny8e3oUi5hykwVy2wi\n"
    "3isPoyQxG6cb+936KJNtUWEiEO9AoFunkaCXCQ0CgYEAoGnECN2oeUbblM9HrBdK\n"
    "fsz64g6MSV7h2StJ1UiMO5x4k2e6VXPWfMgi6Slvu3O7sy+3eUPQWY56fcpIrOH6\n"
    "8SPpkZxzdzGETGJ+pKgH/hgm4Xzo57/tGLNI7CPhUgoYQNzaZfh8+RNkw/DJwiAw\n"
    "sALYUSDIHADRkR+qfpi5as8=\n"
    "-----END PRIVATE KEY-----\n";

using tcp = boost::asio::ip::tcp;
namespace ba = boost::asio;
namespace bs = boost::system;

typedef ba::ssl::stream<tcp::socket> ssl_socket_t;
typedef shared_ptr<ssl_socket_t> socket_ptr;

class Connection_initiator_base {
public:
  void set_port(const uint16_t& port);
  void run();
  void stop();
  void start_listen();
protected:
  Connection_initiator_base();
  virtual ~Connection_initiator_base(){}

  void connect(const string& addr, const uint16_t& port);
  uint16_t get_port() const;

private:
  virtual void handle_new_connection(socket_ptr socket) = 0;

  void await_stop();
  void listen();

//maybe a bit overkill, but best to keep it standard
  struct Data {
    uint16_t port = DEFAULT_LISTEN_PORT;
  } data;
  mutable rw_mutex port_mtx;

  ba::io_service io_service_;
  ba::signal_set signals_;
  tcp::acceptor acceptor_;
  tcp::resolver resolver_;
  socket_ptr accept_socket_;
  ba::ssl::context client_context_;
  ba::ssl::context server_context_;
};

#endif // CONNECTION_INITIATOR_BASE_H
