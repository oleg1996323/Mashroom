#include <gtest/gtest.h>
#include "network/common/message/message_handler.h"

using namespace network;

TEST(NetworkMesssageHandler,ClientSide){
    MessageHandler<Side::CLIENT> hmsg;
    hmsg.emplace_message<MESSAGE_ID<Side::CLIENT>::SERVER_STATUS>();
    ASSERT_TRUE(hmsg.has_message());
    ASSERT_EQ(hmsg.index(),MESSAGE_ID<Side::CLIENT>::SERVER_STATUS+1);
    hmsg.clear();
    ASSERT_EQ(hmsg.index(),0);
    ASSERT_EQ(hmsg.emplace_default_message_by_id(MESSAGE_ID<Side::CLIENT>::INDEX_REF),ErrorCode::NONE);
    ASSERT_EQ(hmsg.index(),MESSAGE_ID<Side::CLIENT>::INDEX_REF+1);
}

TEST(NetworkMesssageHandler,ServerSide){
    MessageHandler<Side::SERVER> hmsg;
    hmsg.emplace_message<MESSAGE_ID<Side::SERVER>::SERVER_STATUS>();
    ASSERT_TRUE(hmsg.has_message());
    ASSERT_EQ(hmsg.index(),MESSAGE_ID<Side::SERVER>::SERVER_STATUS+1);
    hmsg.clear();
    ASSERT_EQ(hmsg.index(),0);
    
    ASSERT_EQ(hmsg.emplace_default_message_by_id(MESSAGE_ID<Side::SERVER>::DATA_REPLY_INDEX_REF),ErrorCode::NONE);
    ASSERT_EQ(hmsg.index(),MESSAGE_ID<Side::SERVER>::DATA_REPLY_INDEX_REF+1);
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}