
#include <iostream>
#include <gtest/gtest.h>
#include "client.h"
#include "server.h"

 void  show_pending_transactions()
 {
 	std::cout  <<  std::string(20, '*') <<  std::endl;
 	for(const  auto& trx : pending_trxs)
 		std::cout << trx <<  std::endl;
 	std::cout  <<  std::string(20, '*') <<  std::endl;
 }

int main(int argc, char **argv)
{
    if (true) // make false to run unit-tests
    {
        Server s;
        s.add_client("ali");
        s.add_client("sina");
        auto c=s.get_client("ali");
        c.get()->transfer_money("sina",2);
        std::cout<<s.get_wallet("sina")<<std::endl;
        std::cout<<s.get_wallet("ali")<<std::endl;
        show_pending_transactions();
        s.mine();
        std::cout<<s.get_wallet("sina")<<std::endl;
        std::cout<<s.get_wallet("ali")<<std::endl;
    }
    else
    {
        ::testing::InitGoogleTest(&argc, argv);
        std::cout << "RUNNING TESTS ..." << std::endl;
        int ret{RUN_ALL_TESTS()};
        if (!ret)
            std::cout << "<<<SUCCESS>>>" << std::endl;
        else
            std::cout << "FAILED" << std::endl;
    }
    return 0;   
}