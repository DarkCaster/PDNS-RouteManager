#include "InterfaceConfig.h"

InterfaceConfig::InterfaceConfig():
    isUp(false),
    isPtP(false),
    localIPs(),
    remoteIPs()
{
}

InterfaceConfig::InterfaceConfig(const InterfaceConfig& other):
    InterfaceConfig(other.isUp,other.isPtP,other.localIPs,other.remoteIPs)
{
}

InterfaceConfig::InterfaceConfig(const bool _isUp, const bool _isPtP, const std::set<IPAddress> &_localIPs, const std::set<IPAddress> &_remoteIPs):
    isUp(_isUp),
    isPtP(_isPtP),
    localIPs(_localIPs),
    remoteIPs(_remoteIPs)
{
}

InterfaceConfig InterfaceConfig::AddLocalIP(const IPAddress& ip) const
{
    auto tmpSet(localIPs);
    tmpSet.insert(ip);
    return InterfaceConfig(isUp,isPtP,tmpSet,remoteIPs);
}

InterfaceConfig InterfaceConfig::DelLocalIP(const IPAddress& ip) const
{
    auto tmpSet(localIPs);
    tmpSet.erase(ip);
    return InterfaceConfig(isUp,isPtP,tmpSet,remoteIPs);
}

InterfaceConfig InterfaceConfig::AddRemoteIP(const IPAddress& ip) const
{
    auto tmpSet(remoteIPs);
    tmpSet.insert(ip);
    return InterfaceConfig(isUp,isPtP,localIPs,tmpSet);
}

InterfaceConfig InterfaceConfig::DelRemoteIP(const IPAddress& ip) const
{
    auto tmpSet(remoteIPs);
    tmpSet.erase(ip);
    return InterfaceConfig(isUp,isPtP,localIPs,tmpSet);
}

InterfaceConfig InterfaceConfig::SetState(const bool _isUp) const
{
    return InterfaceConfig(_isUp,isPtP,localIPs,remoteIPs);
}

InterfaceConfig InterfaceConfig::SetType(const bool _isPtP) const
{
    return InterfaceConfig(isUp,_isPtP,localIPs,remoteIPs);
}

bool InterfaceConfig::isIPV4Avail() const
{
    if(!isUp)
        return false;
    for(auto el: localIPs)
        if(el.isValid && !el.isV6)
            return true;
    return false;
}

bool InterfaceConfig::isIPV6Avail() const
{
    if(!isUp)
        return false;
    for(auto el: localIPs)
        if(el.isValid && el.isV6)
            return true;
    return false;
}

std::ostream& operator<<(std::ostream& stream, const InterfaceConfig& target)
{
    stream<<"isUp="<<target.isUp<<",isPtP="<<target.isPtP;
    stream<<",localIPs={";
    bool first=false;
    for(const auto &ip: target.localIPs)
    {
        if(first)
            stream<<",";
        stream<<ip;
        first=true;
    }
    stream<<"},remoteIPs={";
    first=false;
    for(const auto &ip: target.remoteIPs)
    {
        if(first)
            stream<<",";
        stream<<ip;
        first=true;
    }
    stream<<"}";
    return stream;
}
