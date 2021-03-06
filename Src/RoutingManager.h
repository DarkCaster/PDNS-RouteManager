#ifndef ROUTINGMANAGER_H
#define ROUTINGMANAGER_H

#include "ILogger.h"
#include "IPAddress.h"
#include "InterfaceConfig.h"
#include "ImmutableStorage.h"
#include "IMessageSubscriber.h"
#include "WorkerBase.h"

#include <mutex>
#include <atomic>
#include <ctime>
#include <unordered_map>
#include <map>

class RoutingManager : public IMessageSubscriber, public WorkerBase
{
    private:
        //constants and thread-safe stuff
        ILogger &logger;
        const std::string ifname;
        const IPAddress gateway4;
        const IPAddress gateway6;
        const unsigned int extraTTL;
        const int mgIntervalSec;
        const int mgPercent;
        const int metric; //must be int, according to rtnetlink.7
        const int ksMetric; //must be int, according to rtnetlink.7
        const int addRetryCount; //TODO: make this value configurable
        //varous locking stuff and cross-thread counters
        std::mutex opLock;
        std::atomic<bool> shutdownPending;
        std::atomic<uint64_t> curTime;
        //all other fields must be accesed only using opLock mutex
        bool started=false;
        int sock;
        ImmutableStorage<InterfaceConfig> ifCfg;
        //containters for storing routes at various states
        std::unordered_map<IPAddress,uint64_t> pendingInserts; //pending (new and failed) routes
        std::unordered_map<IPAddress,int32_t> pendingRetries; //tries counter for pending routes
        std::unordered_map<IPAddress,uint64_t> activeRoutes; //confirmed active routes
        std::multimap<uint64_t,IPAddress> pendingExpires; //routes sorted by expiration time, used by background management worker to decide what route to remove
        //service methods that will use opLock internally
        void ManageRoutes();
        void InsertRoute(const IPAddress &dest, unsigned int ttl);
        void ConfirmRouteAdd(const IPAddress &dest);
        void ConfirmRouteDel(const IPAddress &dest);
        void ProcessNetDevUpdate(const InterfaceConfig &newConfig);
        //internal service methods that is not using opLock.
        uint64_t _UpdateCurTime();
        void _InvalidateActiveRoutes(const bool ipv4, const bool ipv6);
        void _ProcessPendingInserts();
        void _FinalizeRouteInsert(const IPAddress &dest);
        void _FinalizeRouteDelete(const IPAddress &dest);
        void _ProcessRoute(const IPAddress &ip, const bool blackhole, const bool isAddRequest);
        void _ProcessStaleRoutes();
    public:
        RoutingManager(ILogger &logger, const std::string &ifname, const IPAddress &gateway4, const IPAddress &gateway6, const unsigned int extraTTL, const int mgIntervalSec, const int mgPercent, const int metric, const int ksMetric, const int addRetryCount);
        //WorkerBase
        void Worker() final;
        void OnShutdown() final;
        bool Startup() final;
        bool Shutdown() final;
        //IMessageSubscriber
        bool ReadyForMessage(const MsgType msgType) final;
        void OnMessage(const IMessage &message) final;
};

#endif // ROUTINGMANAGER_H
