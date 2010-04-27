message(INFO "Include publish subscribe files")
SET(NARUKOM_PUB_SUB_SRCS
  ${NARUKOM_DIR}/pub_sub/message_queue.h
  ${NARUKOM_DIR}/pub_sub/message_buffer.h
  ${NARUKOM_DIR}/pub_sub/topic_tree.h
  ${NARUKOM_DIR}/pub_sub/topic_tree_node.h
  ${NARUKOM_DIR}/pub_sub/blackboard.h
  ${NARUKOM_DIR}/pub_sub/tuple.h
  ${NARUKOM_DIR}/pub_sub/subscriber.h
  ${NARUKOM_DIR}/pub_sub/publisher.h
  ${NARUKOM_DIR}/pub_sub/publisher.cpp
  ${NARUKOM_DIR}/pub_sub/subscriber.cpp
  ${NARUKOM_DIR}/pub_sub/blackboard.cpp
  ${NARUKOM_DIR}/pub_sub/message_queue.cpp
  ${NARUKOM_DIR}/pub_sub/message_buffer.cpp
)
SET(NARUKOM_PUB_SUB_INCLUDE_DIR ${NARUKOM_DIR}/pub_sub/)

message(INFO "Include messages files")
set(NARUKOM_MESSAGES_SRCS  ${NARUKOM_DIR}/messages/Basic.pb.h
    ${NARUKOM_DIR}/messages/Basic.pb.cc
${NARUKOM_DIR}/messages/pingpong.pb.h
    ${NARUKOM_DIR}/messages/pingpong.pb.cc


)

SET(NARUKOM_MESSAGES_INCLUDE_DIR ${NARUKOM_DIR}/messages/)

message(INFO "Include system files")
set( NARUKOM_SYSTEM_SRCS 
	${NARUKOM_DIR}/system/Thread.h
	${NARUKOM_DIR}/system/Mutex.h
	${NARUKOM_DIR}/system/Thread.cpp
	${NARUKOM_DIR}/system/Mutex.cpp
)

SET(NARUKOM_SYSTEM_INCLUDE_DIR ${NARUKOM_DIR}/system/)


message(INFO "Include network files")
set(NARUKOM_NETWORK_SRCS
	${NARUKOM_DIR}/network/Nack.pb.h
	${NARUKOM_DIR}/network/Nack.pb.cc
	${NARUKOM_DIR}/network/channel.h
	${NARUKOM_DIR}/network/channel.cpp
	${NARUKOM_DIR}/network/network_channel.h
	${NARUKOM_DIR}/network/network_channel.cpp
	${NARUKOM_DIR}/network/multicastsender_receiver.h
	${NARUKOM_DIR}/network/network_message.h
	${NARUKOM_DIR}/network/network_message.cpp
	${NARUKOM_DIR}/network/network_message_buffer.h
	${NARUKOM_DIR}/network/network_message_buffer.cpp
        ${NARUKOM_DIR}/network/network_packet.pb.h
        ${NARUKOM_DIR}/network/network_packet.pb.cc
      
)

SET(NARUKOM_NETWORK_INCLUDE_DIR ${NARUKOM_DIR}/network/)
