message ( STATUS "" )
message ( STATUS " _..::: Configuring Elements :::.._ " )
message ( STATUS "" )


set(ELEMENT_SRCS
      buffer/LockedBuffer.hpp
      factory/Registrar.hpp
      factory/Factory.hpp
      math/Common.hpp
      math/KMat.hpp
      math/Quaternion.hpp
      math/Specific.hpp
      math/stat/Kalman.hpp
      math/stat/Cumulative.hpp
      math/stat/MovingAverage.hpp
      math/optimization/IPenaltyFunction.hpp
      math/optimization/ITwiceDiffFunction.hpp
      math/optimization/IDiffFunction.hpp
      math/optimization/NewtonSolver.hpp
      math/optimization/ConjugateGradientSolver.hpp
      math/optimization/PenaltySolver.hpp
      math/optimization/PenaltyQuadraticSolver.hpp
      math/optimization/FunctionQuadratic.hpp
      math/optimization/FunctionQuadraticSymmetric.hpp
      math/optimization/FunctionAffine.hpp
      math/optimization/FunctionQuadraticPenalty.hpp
      customTypes/K3DArray.hpp
      customTypes/ReservaplePrioryQueue.hpp
      StringRegistry.hpp
      Singleton.hpp
      KStandard.hpp
      Logger.hpp
      vision/KImage.h
)

add_custom_target(Monas-Elements SOURCES ${ELEMENT_SRCS}  )



#add_subdirectory ( ./buffer/ )


