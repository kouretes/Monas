#ifndef __CONTROLTHREAD_H__
#define __CONTROLTHREAD_H__

#define PreviewWindow 121
#define CONST_SKIP 0
#define CONST_SIZE 50
#define CONST_STEP 2
#define LagN 40

#include "core/elements/math/KMat.hpp"
#include <vector>
#include <queue>
#include <iostream>
#include "CircularBuffer.hpp"
//#include "core/elements/math/optimization/FunctionQuadraticPenalty.hpp"
//#include "core/elements/math/optimization/PenaltySolver.hpp"
#include "core/elements/math/optimization/PenaltyQuadraticSolver.hpp"
//#include "core/elements/math/optimization/ConjugateGradientSolver.hpp"
//#include "core/elements/math/optimization/NewtonSolver.hpp"
//#include "core/elements/math/optimization/FunctionAffine.hpp"
//#include "core/elements/math/optimization/FunctionQuadratic.hpp"
//#include "core/elements/math/optimization/FunctionQuadraticSymmetric.hpp"
#include "matlog.h"
#include "RobotParameters.h"
#include "Kalman.h"
#include "Dynamics.h"
#include "cem.h"
//#define KPROFILING_ENABLED
//#include "core/architecture/time/Profiler.hpp"

class LIPMPreviewController{
private:
	//KMath::KMat::GenMatrix<float,LagN,PreviewWindow-1> Ky;

	KMath::KMat::GenMatrix<float, PreviewWindow - 1, 1> ZMPReferenceX, ZMPReferenceY, ZMPtheta, pX, pY;
	/** DMPC **/
	//KMath::KMat::GenMatrix<float, LagN, LagN>  Omega;

	KMath::KMat::GenMatrix<float, LagN, PreviewWindow - 1>  Phitransp;

	KMath::KMat::GenMatrix<float, PreviewWindow - 1, 4>  Tau;

	KMath::KMat::GenMatrix<float, LagN, LagN>  Al;

	KMath::KMat::GenMatrix<float, LagN, 1>  L0, httaX, httaY;

	KMath::KMat::GenMatrix<float, 2 * LagN, 1>  htta;

	KMath::KMat::GenMatrix<float, LagN * 2, LagN * 2> H, Hinv;

	KMath::KMat::GenMatrix<float, PreviewWindow - 1, LagN> Phi;

	KMath::KMat::GenMatrix<float, LagN, LagN> Ky;

	/** CONSTRAINTS **/
	KMath::KMat::GenMatrix<float, LagN * 2, 1> f;

	KMath::KMat::GenMatrix<float, LagN, 1> fx, fy;

	KMath::KMat::GenMatrix<float, CONST_SIZE, 2 * LagN> Aineq1, Aineq2, Aineq3, Aineq4;

	KMath::KMat::GenMatrix<float, CONST_SIZE, 1> bineq1, bineq2, bineq3, bineq4;

	KMath::PenaltyQuadraticSolver<float, 2 * LagN, CONST_SIZE> solver;
	//KMath::PenaltySolver<float,2*LagN,KMath::NewtonSolver> solver;

	/** @fn void fillConstraints(
		KMath::KMat::GenMatrix<float, CONST_SIZE, 2 * LagN> & A,
		KMath::KMat::GenMatrix<float, CONST_SIZE, 1> &b,
		bool ecos, bool fcos
		)
		@brief fills the inequality constraints for the
		constrained MPC
    **/
	void fillConstraints(
		KMath::KMat::GenMatrix<float, CONST_SIZE, 2 * LagN> & A,
		KMath::KMat::GenMatrix<float, CONST_SIZE, 1> &b,
		bool ecos, bool fcos
		);

    /** @fn void generateLaguerre()
     *  @brief computes the orthonormal basis functions
     **/

	void generateLaguerre();


	/** @fn void solveConstrainedMPC()
	 *  @brief solves the convex optimization
	 *  control problem
	 */
	void solveConstrainedMPC();
	//mutable KProfiling::profiler walkprof;
	cem cemBalance;

public:

	RobotParameters &OurRobot;

	Kalman KalmanX, KalmanY;

	Dynamics DynamicsX, DynamicsY;

	LIPMPreviewController(RobotParameters&);


	/** @fn void LIPMComPredictor(CircularBuffer<KVecFloat3> & ZmpBuffer, float CoMmeasuredX, float CoMmeasuredY, float ZMPMeasuredX, float ZMPMeasuredY);
	 *  @brief Computes the desired COM
	 */

	void LIPMComPredictor(CircularBuffer<KVecFloat3> & ZmpBuffer, float CoMmeasuredX, float CoMmeasuredY, float ZMPMeasuredX, float ZMPMeasuredY, int balance);

	/** @fn void DMPC()
	 *  @brief Computes the MPC gains
	**/

	void DMPC();

	float DeltauX, DeltauY, uX, uY;

	float predictedErrorX, predictedErrorY;

	float d;

	bool isDoubleSupport;

	KVecFloat2 COM;

	mat_log flog;

};


#endif
