set ( ActivityName HeadAnglePublisher )

set ( ActivitySrcs HeadAnglePublisher.cpp ../../messages/ledMsg.pb.cc )

include ( ../activTemplate.cmake )


target_link_libraries ( ${ActivityName} NaoQiDep )
