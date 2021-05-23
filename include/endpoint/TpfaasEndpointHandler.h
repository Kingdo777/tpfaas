#pragma once

#include <pistache/http.h>

class TpfaasEndpointHandler : public Pistache::Http::Handler
{
  public:
    HTTP_PROTOTYPE(TpfaasEndpointHandler)

    void onTimeout(const Pistache::Http::Request& request,
                   Pistache::Http::ResponseWriter writer) override;

    void onRequest(const Pistache::Http::Request& request,
                   Pistache::Http::ResponseWriter response) override;

    std::string handleFunction(const std::string& requestStr);

  private:
//    std::string executeFunction(faabric::Message& msg);
};
