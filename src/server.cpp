#include "client.h"
#include "server.h"

std::shared_ptr<Client> Server::add_client(std::string id)
{
    size_t flag {};
    for (auto name { clients.begin() }; name != clients.end(); name++) {
        if (((name->first)->get_id()==id)) {
            flag = 1;
            break;
        }
    }

    std::shared_ptr<Client> newClient { nullptr };
    if (flag == 1) {
        std::string num {};
        std::random_device seeder;
        std::default_random_engine generator(seeder());
        std::uniform_int_distribution<> distribution(0, 9999);
        num = std::to_string(distribution(generator));
        newClient = std::make_shared<Client>(id + num, *this);
    } else {
        newClient = std::make_shared<Client>(id, *this);
    }
    clients[newClient] = 3.0;
    return newClient;
}

std::shared_ptr<Client> Server::get_client(std::string id) const
{
    size_t flag {};
    std::shared_ptr<Client> C { nullptr };
    for (auto name { clients.begin() }; name != clients.end(); name++) {
        if ((name->first)->get_id()==id) {
            C = name->first;
            break;
        }
    }
    return C;
}

double Server::get_wallet(std::string id) const
{
    double money {};

    for (auto name { clients.begin() }; name != clients.end(); name++) {
        if ((name->first)->get_id()==id == 0) {
            money = name->second;
            break;
        }
    }
    return money;
}

bool Server::parse_trx(std::string trx, std::string* sender, std::string* receiver, double* value) const
{
    std::stringstream str(trx);
    std::string segment;
    std::vector<std::string> seglist;
    while(std::getline(str, segment, '-'))
    {
        seglist.push_back(segment);
    }
    if(seglist.size()!=3){
        throw std::runtime_error("String is not standard");
    }
    *sender = seglist[0];
    *receiver = seglist[1];
    *value = stod(seglist[2]);
    return true;
}

bool Server::add_pending_trx(std::string trx, std::string signature)
{
    std::string send {};
    std::string receive {};
    double val {};
    parse_trx(trx, &send, &receive, &val);

    if (get_wallet(send) >= val) {
        auto senderC { get_client(send) };
        if (crypto::verifySignature(senderC->get_publickey(),trx,signature)) {
            pending_trxs.push_back(trx);
            clients[senderC] -= val;
            return true;
        } else {
            std::cout << "Senders' signature is wrong" << std::endl;
            return false;
        }
    } else {
        std::cout << "There is not enough coins in the wallet" << std::endl;
        return false;
    }
}

size_t Server::mine()
{
    std::string nonceid {};
    size_t nonce {};
    std::string hash {};

    std::string mempool;
    for (const auto& ptrxs : pending_trxs) {
        mempool += ptrxs;
    }
    while (true) {
        for (auto name { clients.begin() }; name != clients.end(); name++) {
            nonce = (name->first)->generate_nonce();
            mempool += std::to_string(nonce);
            hash = crypto::sha256(mempool);
            if (hash.find("000", 0, 10) != std::string::npos) {
                std::cout << "Mined by " << (name->first)->get_id() << "with nonce = " << nonce << std::endl;
                clients[(name->first)] += 6.25;
                pending_trxs.clear();
                return nonce;
            }
        }
    }
}