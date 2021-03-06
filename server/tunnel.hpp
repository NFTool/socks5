/*******************************************************************************
 *
 * socks5
 * A C++11 socks5 proxy server based on Libevent 
 *
 * Copyright 2018 Senlin Zhan. All rights reserved.
 *
 ******************************************************************************/

#ifndef TUNNEL_H
#define TUNNEL_H

#include "address.hpp"
#include "auth.hpp"
#include "base.hpp"
#include "config.hpp"
#include "cipher.hpp"
#include "request.hpp"

#include <memory>

/**
   Forward declaration
 **/
struct  event_base;
struct  evdns_base;
struct  bufferevent;
class   Auth;

class Tunnel
{
public:
    enum class State
    {
        init, waitUserPassAuth, authorized,
        clientMustClose, connected, waitForConnect
    };
    
    Tunnel(const Config &config, std::shared_ptr<ServerBase> base, int inConnFd);
    ~Tunnel();

    Tunnel(const Tunnel &) = delete;
    Tunnel &operator=(const Tunnel &) = delete;
    
    State state() const;
    void setState(State state);

    Auth::State handleAuthentication(bufferevent *inConn);
    Auth::State handleUserPassAuth(bufferevent *inConn);
    
    Request::State handleRequest(bufferevent *inConn);
    
    bufferevent *inConnection() const;
    bufferevent *outConnection() const;
    
    void setOutConnection(bufferevent *outConn);

    int clientID() const;

    Cryptor cryptor() const
    {
        return cryptor_;
    }

    void encryptTransfer()
    {
        assert(inConn_ != nullptr);        
        assert(outConn_ != nullptr);
        
        cryptor_.encryptTransfer(outConn_, inConn_);
    }
    
    void decryptTransfer()
    {
        assert(inConn_ != nullptr);        
        assert(outConn_ != nullptr);
        
        cryptor_.decryptTransfer(inConn_, outConn_);
    }
    
private:
    Config                       config_;
    std::shared_ptr<ServerBase>  base_;
    int                          inConnFd_;    
    bufferevent                  *inConn_;
    bufferevent                  *outConn_;
    State                        state_;
    Cryptor                      cryptor_;
};

#endif /* TUNNEL_H */
