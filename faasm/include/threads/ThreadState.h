#pragma once

#include <mutex>
#include <vector>

#include <faabric/proto/faabric.pb.h>
#include <faabric/util/barrier.h>
#include <faabric/util/environment.h>
#include <faabric/util/locks.h>

namespace threads {
    class PthreadTask
{
  public:
    PthreadTask(faabric::Message* parentMsgIn,
                std::shared_ptr<faabric::Message> msgIn)
      : parentMsg(parentMsgIn)
      , msg(msgIn)
    {}

    bool isShutdown = false;
    faabric::Message* parentMsg;
    std::shared_ptr<faabric::Message> msg;
};
}
