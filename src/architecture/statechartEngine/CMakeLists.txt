message ( STATUS "" )
message ( STATUS " _..::: Configuring Statechart Engine :::.._ " )
message ( STATUS "" )

set ( statechartEngineSrcs  AndState.cpp
                            BasicState.cpp
                            EndState.cpp
                            IAction.cpp
                            IActivity.cpp
                            ICondition.cpp
                            IEvent.cpp
                            IParameter.cpp
                            OrState.cpp
                            AndState.cpp
                            StartState.cpp
                            State.cpp
                            Statechart.cpp
                            Transitions/ConditionConnector.cpp
                            Transitions/TransitionConnector.cpp )

# TODO
include_directories ( AFTER ../narukom/
                            ../narukom/message_definitions/
                            ../narukom/messages/
                            ../narukom/network/
                            ../narukom/pub_sub/
                            ../narukom/system/ )

add_library ( statechartEngine ${statechartEngineSrcs} )


set ( TargetLinkLibraries ${TargetLinkLibraries} statechartEngine PARENT_SCOPE )

