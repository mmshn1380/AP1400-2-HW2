#include "client.h"
#include "server.h"


Client::Client(std::string id, const Server& server)
    : id { id }
    , server { &server }
{
    crypto::generate_key(public_key, private_key);
}

std::string Client::get_id() const
{
    return id;
}

std::string Client::get_publickey() const
{
    return public_key;
}

double Client::get_wallet() const
{
    return server->get_wallet(id);
}

std::string Client::sign(std::string txt) const
{
    return crypto::signMessage(private_key, txt);
}

bool Client::transfer_money(std::string receiver, double value)
{
    if (get_wallet() >= value) {
        std::string trx {};
        trx = id + "-" + receiver + "-" + std::to_string(value);
        const_cast<Server&>(*server).add_pending_trx(trx, crypto::signMessage(private_key,crypto::sha256(trx)));
        return true;
    } else {
        std::cout << "There is not enough coins in the wallet" << std::endl;
        return false;
    }
}

size_t Client::generate_nonce() const
{
    std::random_device seeder;
    std::default_random_engine generator(seeder());
    std::uniform_int_distribution<> distribution(0, 9999);
    return static_cast<size_t>(distribution(generator));
}