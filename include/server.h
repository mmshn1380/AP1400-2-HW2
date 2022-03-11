#ifndef SERVER_H
#define SERVER_H
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include <vector>
#include <sstream>
#include "client.h"
inline std::vector<std::string> pending_trxs;

class Server {
public:
    Server() = default;
    std::shared_ptr<Client> add_client(std::string id);
    std::shared_ptr<Client> get_client(std::string id) const;
    double get_wallet(std::string id) const;
    bool parse_trx(std::string trx, std::string* sender, std::string* receiver, double* value) const;
    bool add_pending_trx(std::string trx, std::string signature);
    size_t mine();

private:
    std::map<std::shared_ptr<Client>, double> clients;
};

inline void show_wallets(const Server& server)
{
    auto p = (std::map<std::shared_ptr<Client>, double>*)(&server);
    std::cout << std::string(20, '*') << std::endl;
    for (const auto& client : *p)
        std::cout << client.first->get_id() << " : " << client.second << std::endl;
    std::cout << std::string(20, '*') << std::endl;
}

#endif //SERVER_H