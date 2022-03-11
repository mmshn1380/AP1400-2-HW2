#include "client.h"
#include "server.h"

std::shared_ptr<Client> Server::get_client(std::string id) const
{
    size_t flag {};
    std::shared_ptr<Client> C { nullptr };
    for (auto cli { clients.begin() }; cli != clients.end(); cli++) {
        if ((cli->first)->get_id()==id) {
            C = cli->first;
            break;
        }
    }
    return C;
}

std::shared_ptr<Client> Server::add_client(std::string id)
{
    size_t flag {};
    for (auto cli { clients.begin() }; cli != clients.end(); cli++) {
        if (((cli->first)->get_id()==id)) {
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

double Server::get_wallet(std::string id) const
{
    double money {};
    for (auto cli { clients.begin() }; cli != clients.end(); cli++) {
        if ((cli->first)->get_id()==id == 0) {
            money = cli->second;
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
    double size {};
    parse_trx(trx, &send, &receive, &size);

    if (get_wallet(send) >= size) {
        auto senderC { get_client(send) };
        if (crypto::verifySignature(senderC->get_publickey(),trx,signature)) {
            pending_trxs.push_back(trx);
            return true;
        } else {
            std::cout << "signature is wrong" << std::endl;
            return false;
        }
    } else {
        std::cout << "not enough coins in the wallet" << std::endl;
        return false;
    }
}

size_t Server::mine()
{
    std::string hash {};
    size_t nonce {};
    std::string mempool{};
    for (const auto& transaction : pending_trxs) {
        mempool += transaction;
    }
    while (true) {
        for (auto cli { clients.begin() }; cli != clients.end(); cli++) {
            nonce = (cli->first)->generate_nonce();
            hash = crypto::sha256(mempool+std::to_string(nonce));
            if (hash.find("000") < 7) {
                std::cout << "Mined by " << (cli->first)->get_id() << " with nonce = " << nonce << std::endl;
                clients[(cli->first)] += 6.25;
                for (const auto& transaction : pending_trxs) {
                    std::string sender{};
                    std::string receiver{};
                    double size{};
                    parse_trx(transaction,&sender,&receiver,&size);
                    clients[get_client(sender)]-=size;
                    clients[get_client(receiver)]+=size;
                }
                pending_trxs.clear();
                return nonce;
            }
        }
    }
}