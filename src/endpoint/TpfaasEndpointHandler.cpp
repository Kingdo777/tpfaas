#include <endpoint/TpfaasEndpointHandler.h>

#include <faabric/util/json.h>
#include <utils/logging.h>
#include <utils/timing.h>
#include <utils/config.h>


void TpfaasEndpointHandler::onTimeout(const Pistache::Http::Request &request,
                                      Pistache::Http::ResponseWriter writer) {
    writer.send(Pistache::Http::Code::No_Content);
}

void TpfaasEndpointHandler::onRequest(const Pistache::Http::Request &request,
                                      Pistache::Http::ResponseWriter response) {
    const std::shared_ptr<spdlog::logger> &logger = getLogger();
    logger->debug("Tpfaas handler received request");

    // Very permissive CORS
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>(
            "*");
    response.headers().add<Pistache::Http::Header::AccessControlAllowMethods>(
            "GET,POST,PUT,OPTIONS");
    response.headers().add<Pistache::Http::Header::AccessControlAllowHeaders>(
            "User-Agent,Content-Type");

    // Text response type
    response.headers().add<Pistache::Http::Header::ContentType>(
            Pistache::Http::Mime::MediaType("text/plain"));

    PROF_START(endpointRoundTrip)

//    // Set response timeout
//    SystemConfig &conf = getSystemConfig();
//    response.timeoutAfter(std::chrono::milliseconds(conf.globalMessageTimeout));

    // Parse message from JSON in request
    const std::string requestStr = request.body();
    std::string responseStr = handleFunction(requestStr);

    PROF_END(endpointRoundTrip)
    response.send(Pistache::Http::Code::Ok, responseStr);
}

std::string TpfaasEndpointHandler::handleFunction(
        const std::string &requestStr) {
    std::string responseStr;
//    if (requestStr.empty()) {
//        responseStr = "Empty request";
//    } else {
//        faabric::Message msg = TpfaasjsonToMessage(requestStr);
//        faabric::scheduler::Scheduler& sched =
//          faabric::scheduler::getScheduler();
//
//        if (msg.isstatusrequest()) {
//            responseStr = sched.getMessageStatus(msg.id());
//
//        } else if (msg.isexecgraphrequest()) {
//            faabric::scheduler::ExecGraph execGraph =
//              sched.getFunctionExecGraph(msg.id());
//            responseStr = faabric::scheduler::execGraphToJson(execGraph);
//
//        } else if (msg.type() == faabric::Message_MessageType_FLUSH) {
//            const std::shared_ptr<spdlog::logger>& logger =
//              TpfaasgetLogger();
//            logger->debug("Broadcasting flush request");
//
//            sched.broadcastFlush();
//        } else {
//            responseStr = executeFunction(msg);
//        }
//    }
    sleep(1);
    responseStr = "Hello Kingdo";
    return responseStr;
}

//std::string TpfaasEndpointHandler::executeFunction(faabric::Message& msg)
//{
//    const std::shared_ptr<spdlog::logger>& logger = TpfaasgetLogger();
//    TpfaasSystemConfig& conf = TpfaasgetSystemConfig();
//
//    if (msg.user().empty()) {
//        return "Empty user";
//    } else if (msg.function().empty()) {
//        return "Empty function";
//    }
//
//    // Set message ID and master host
//    TpfaassetMessageId(msg);
//    std::string thisHost = TpfaasgetSystemConfig().endpointHost;
//    msg.set_masterhost(thisHost);
//
//    auto tid = (pid_t)syscall(SYS_gettid);
//
//    const std::string funcStr = TpfaasfuncToString(msg, true);
//    logger->debug("Worker HTTP thread {} scheduling {}", tid, funcStr);
//
//    // Schedule it
//    faabric::scheduler::Scheduler& sch = faabric::scheduler::getScheduler();
//    sch.callFunction(msg);
//
//    // Await result on global bus (may have been executed on a different worker)
//    if (msg.isasync()) {
//        return TpfaasbuildAsyncResponse(msg);
//    } else {
//        logger->debug("Worker thread {} awaiting {}", tid, funcStr);
//
//        try {
//            const faabric::Message result =
//              sch.getFunctionResult(msg.id(), conf.globalMessageTimeout);
//            logger->debug("Worker thread {} result {}", tid, funcStr);
//
//            if (result.sgxresult().empty()) {
//                return result.outputdata() + "\n";
//            } else {
//                return TpfaasgetJsonOutput(result);
//            }
//        } catch (faabric::redis::RedisNoResponseException& ex) {
//            return "No response from function\n";
//        }
//    }
//}
