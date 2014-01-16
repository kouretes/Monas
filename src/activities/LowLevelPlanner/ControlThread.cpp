//
//  ControlThread.cpp
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//

#include "ControlThread.h"
#include <iostream>
LIPMPreviewController::LIPMPreviewController(RobotParameters &rp ) : OurRobot(rp), DynamicsX(OurRobot), DynamicsY(OurRobot), KalmanX(OurRobot), KalmanY(OurRobot)
{
    KalmanX.uBuffer.push(0.000);
    KalmanY.uBuffer.push(0.000);


    /** Compute Gains Kx, Ky **/
    DMPC();

    //Initializing Variables
    DeltauX=0.000;
    DeltauY=0.000;
    uX=0.000;
    uY=0.000;
}



void LIPMPreviewController::LIPMComPredictor(CircularBuffer<float> & ZmpBufferX,CircularBuffer<float> & ZmpBufferY, float CoMMeasuredX,float CoMMeasuredY,float ZMPMeasuredX,float ZMPMeasuredY )
{
    KalmanX.Filter(ZMPMeasuredX,CoMMeasuredX);

    KalmanY.Filter(ZMPMeasuredY,CoMMeasuredY);

    //Setting the Reference Signal
    int l = 0;
	for (int i = 1; i < PreviewWindow; i++, l++)
	{
		if (i < ZmpBufferX.size()){
			ZMPReferenceX(l) = ZmpBufferX[i];
			ZMPReferenceY(l) = ZmpBufferY[i];
		}
		else{

			ZMPReferenceX(l) = ZmpBufferX[ZmpBufferX.size() - 1];
			ZMPReferenceY(l) = ZmpBufferY[ZmpBufferY.size() - 1];
		}
	}


      DynamicsX.AugmentState();
      DynamicsY.AugmentState();

      /**define Laguerre Coefficients **/
      httaX=Ky*ZMPReferenceX+Kx*DynamicsX.State_e;
      httaY=Ky*ZMPReferenceY+Kx*DynamicsY.State_e;
      /** Optimal Preview Control **/
      DeltauX=L0.transp()*httaX;
      DeltauY=L0.transp()*httaY;

      KVecFloat2 errorX=KVecFloat2(CoMMeasuredX,KalmanX.StatePredict(0));
      KVecFloat2 errorY=KVecFloat2(CoMMeasuredY,KalmanY.StatePredict(0));
      //std::cout<<"X : "<< KalmanX.StatePredict(0)<<std::endl;
      //std::cout<<"Y : "<< KalmanY.StatePredict(0)<<std::endl;

      uX=uX+DeltauX;
      uY=uY+DeltauY;
      DynamicsX.Update(uX,errorX);
      DynamicsY.Update(uY,errorY);


        KalmanX.uBuffer.push(DynamicsX.zmpstateNew-DynamicsX.zmpstate);
        //KalmanX.uBuffer.push(ZMPReferenceX(1)-ZmpBufferX[0]);
        KalmanY.uBuffer.push(DynamicsY.zmpstateNew-DynamicsY.zmpstate);
        //KalmanY.uBuffer.push(ZMPReferenceY(1)-ZmpBufferY[0]);

         //Estimated COM position
        COM(0)=DynamicsX.State(0);
        COM(1)=DynamicsY.State(0);       //+0.5*(State(1)+1/2*State(2)*OurRobot.getWalkParameter(Ts))*OurRobot.getWalkParameter(Ts);//
        predictedErrorX=DynamicsX.predictedError;
        predictedErrorY=DynamicsY.predictedError;

}
void LIPMPreviewController::DMPC()
{

Ky(0,0) =-288.4548;
Ky(0,1) =-221.9847;
Ky(0,2) =-88.636;
Ky(0,3) =-1.7037;
Ky(0,4) =39.6128;
Ky(0,5) =58.867;
Ky(0,6) =68.632;
Ky(0,7) =72.3905;
Ky(0,8) =71.9162;
Ky(0,9) =69.4386;
Ky(0,10) =66.6581;
Ky(0,11) =64.0503;
Ky(0,12) =61.293;
Ky(0,13) =58.0512;
Ky(0,14) =54.3914;
Ky(0,15) =50.701;
Ky(0,16) =47.3845;
Ky(0,17) =44.6271;
Ky(0,18) =42.3475;
Ky(0,19) =40.3042;
Ky(0,20) =38.2496;
Ky(0,21) =36.0431;
Ky(0,22) =33.6862;
Ky(0,23) =31.2905;
Ky(0,24) =29.0108;
Ky(0,25) =26.9808;
Ky(0,26) =25.2711;
Ky(0,27) =23.8779;
Ky(0,28) =22.7363;
Ky(0,29) =21.7478;
Ky(0,30) =20.8095;
Ky(0,31) =19.8382;
Ky(0,32) =18.7838;
Ky(0,33) =17.6325;
Ky(0,34) =16.4026;
Ky(0,35) =15.1341;
Ky(0,36) =13.8781;
Ky(0,37) =12.6854;
Ky(0,38) =11.5994;
Ky(0,39) =10.6507;
Ky(0,40) =9.8553;
Ky(0,41) =9.2153;
Ky(0,42) =8.721;
Ky(0,43) =8.3537;
Ky(0,44) =8.0898;
Ky(0,45) =7.9029;
Ky(0,46) =7.7674;
Ky(0,47) =7.6595;
Ky(0,48) =7.5591;
Ky(0,49) =7.4499;
Ky(0,50) =7.3201;
Ky(0,51) =7.1614;
Ky(0,52) =6.9693;
Ky(0,53) =6.7422;
Ky(0,54) =6.4806;
Ky(0,55) =6.1869;
Ky(0,56) =5.8644;
Ky(0,57) =5.5174;
Ky(0,58) =5.1505;
Ky(0,59) =4.7683;
Ky(0,60) =4.3754;
Ky(0,61) =3.9761;
Ky(0,62) =3.5744;
Ky(0,63) =3.1741;
Ky(0,64) =2.7785;
Ky(0,65) =2.3906;
Ky(0,66) =2.013;
Ky(0,67) =1.6483;
Ky(0,68) =1.2987;
Ky(0,69) =0.966;
Ky(0,70) =0.65214;
Ky(0,71) =0.35875;
Ky(0,72) =0.087382;
Ky(0,73) =-0.16051;
Ky(0,74) =-0.38355;
Ky(0,75) =-0.58042;
Ky(0,76) =-0.74984;
Ky(0,77) =-0.89058;
Ky(0,78) =-1.0014;
Ky(0,79) =-1.0812;
Ky(0,80) =-1.1287;
Ky(0,81) =-1.1429;
Ky(0,82) =-1.1224;
Ky(0,83) =-1.0663;
Ky(0,84) =-0.97333;
Ky(0,85) =-0.84239;
Ky(0,86) =-0.67236;
Ky(0,87) =-0.46209;
Ky(0,88) =-0.21047;
Ky(0,89) =0.083635;
Ky(0,90) =0.42135;
Ky(0,91) =0.80379;
Ky(0,92) =1.2321;
Ky(0,93) =1.7074;
Ky(0,94) =2.2308;
Ky(0,95) =2.8034;
Ky(0,96) =3.4264;
Ky(0,97) =4.1008;
Ky(0,98) =4.8279;
Ky(0,99) =5.6087;
Ky(1,0) =1149.6723;
Ky(1,1) =693.5211;
Ky(1,2) =94.0439;
Ky(1,3) =-200.5322;
Ky(1,4) =-280.6536;
Ky(1,5) =-280.3973;
Ky(1,6) =-259.8312;
Ky(1,7) =-234.1722;
Ky(1,8) =-207.7979;
Ky(1,9) =-184.4909;
Ky(1,10) =-166.3294;
Ky(1,11) =-152.634;
Ky(1,12) =-141.312;
Ky(1,13) =-130.7204;
Ky(1,14) =-120.4638;
Ky(1,15) =-111.0388;
Ky(1,16) =-103.0508;
Ky(1,17) =-96.6632;
Ky(1,18) =-91.5095;
Ky(1,19) =-86.9432;
Ky(1,20) =-82.3715;
Ky(1,21) =-77.4831;
Ky(1,22) =-72.2982;
Ky(1,23) =-67.0749;
Ky(1,24) =-62.1541;
Ky(1,25) =-57.8159;
Ky(1,26) =-54.1945;
Ky(1,27) =-51.2604;
Ky(1,28) =-48.8561;
Ky(1,29) =-46.7583;
Ky(1,30) =-44.743;
Ky(1,31) =-42.6344;
Ky(1,32) =-40.3328;
Ky(1,33) =-37.8184;
Ky(1,34) =-35.1389;
Ky(1,35) =-32.3871;
Ky(1,36) =-29.6757;
Ky(1,37) =-27.1153;
Ky(1,38) =-24.797;
Ky(1,39) =-22.7838;
Ky(1,40) =-21.1061;
Ky(1,41) =-19.7644;
Ky(1,42) =-18.7343;
Ky(1,43) =-17.9734;
Ky(1,44) =-17.4286;
Ky(1,45) =-17.0427;
Ky(1,46) =-16.7602;
Ky(1,47) =-16.5306;
Ky(1,48) =-16.3112;
Ky(1,49) =-16.0685;
Ky(1,50) =-15.7777;
Ky(1,51) =-15.4226;
Ky(1,52) =-14.9945;
Ky(1,53) =-14.4906;
Ky(1,54) =-13.9129;
Ky(1,55) =-13.2671;
Ky(1,56) =-12.561;
Ky(1,57) =-11.804;
Ky(1,58) =-11.006;
Ky(1,59) =-10.1771;
Ky(1,60) =-9.3271;
Ky(1,61) =-8.4654;
Ky(1,62) =-7.6005;
Ky(1,63) =-6.7401;
Ky(1,64) =-5.8915;
Ky(1,65) =-5.0608;
Ky(1,66) =-4.2537;
Ky(1,67) =-3.4754;
Ky(1,68) =-2.7304;
Ky(1,69) =-2.0227;
Ky(1,70) =-1.356;
Ky(1,71) =-0.73395;
Ky(1,72) =-0.15957;
Ky(1,73) =0.36412;
Ky(1,74) =0.8343;
Ky(1,75) =1.2483;
Ky(1,76) =1.6034;
Ky(1,77) =1.8972;
Ky(1,78) =2.1272;
Ky(1,79) =2.291;
Ky(1,80) =2.3861;
Ky(1,81) =2.4103;
Ky(1,82) =2.3612;
Ky(1,83) =2.2364;
Ky(1,84) =2.0336;
Ky(1,85) =1.7506;
Ky(1,86) =1.3849;
Ky(1,87) =0.93441;
Ky(1,88) =0.39671;
Ky(1,89) =-0.23048;
Ky(1,90) =-0.94943;
Ky(1,91) =-1.7625;
Ky(1,92) =-2.6718;
Ky(1,93) =-3.6799;
Ky(1,94) =-4.7888;
Ky(1,95) =-6.001;
Ky(1,96) =-7.3188;
Ky(1,97) =-8.7443;
Ky(1,98) =-10.2799;
Ky(1,99) =-11.9279;
Ky(2,0) =-2004.6275;
Ky(2,1) =-669.2391;
Ky(2,2) =438.8999;
Ky(2,3) =691.7293;
Ky(2,4) =554.5567;
Ky(2,5) =371.8162;
Ky(2,6) =243.5209;
Ky(2,7) =166.6306;
Ky(2,8) =123.2655;
Ky(2,9) =101.6817;
Ky(2,10) =93.8383;
Ky(2,11) =92.7924;
Ky(2,12) =93.23;
Ky(2,13) =92.4281;
Ky(2,14) =90.021;
Ky(2,15) =86.8466;
Ky(2,16) =83.784;
Ky(2,17) =81.1555;
Ky(2,18) =78.7275;
Ky(2,19) =76.0345;
Ky(2,20) =72.7195;
Ky(2,21) =68.7165;
Ky(2,22) =64.2461;
Ky(2,23) =59.6899;
Ky(2,24) =55.4323;
Ky(2,25) =51.7393;
Ky(2,26) =48.7048;
Ky(2,27) =46.2609;
Ky(2,28) =44.231;
Ky(2,29) =42.3968;
Ky(2,30) =40.5595;
Ky(2,31) =38.5797;
Ky(2,32) =36.3946;
Ky(2,33) =34.0143;
Ky(2,34) =31.5047;
Ky(2,35) =28.9642;
Ky(2,36) =26.5006;
Ky(2,37) =24.2114;
Ky(2,38) =22.1718;
Ky(2,39) =20.4278;
Ky(2,40) =18.9955;
Ky(2,41) =17.8652;
Ky(2,42) =17.0065;
Ky(2,43) =16.3755;
Ky(2,44) =15.9215;
Ky(2,45) =15.5922;
Ky(2,46) =15.3388;
Ky(2,47) =15.1181;
Ky(2,48) =14.895;
Ky(2,49) =14.6425;
Ky(2,50) =14.3416;
Ky(2,51) =13.9808;
Ky(2,52) =13.5546;
Ky(2,53) =13.0624;
Ky(2,54) =12.5075;
Ky(2,55) =11.8957;
Ky(2,56) =11.2348;
Ky(2,57) =10.5331;
Ky(2,58) =9.7997;
Ky(2,59) =9.0433;
Ky(2,60) =8.2724;
Ky(2,61) =7.4947;
Ky(2,62) =6.7175;
Ky(2,63) =5.9475;
Ky(2,64) =5.1903;
Ky(2,65) =4.4513;
Ky(2,66) =3.7351;
Ky(2,67) =3.046;
Ky(2,68) =2.3877;
Ky(2,69) =1.7636;
Ky(2,70) =1.1768;
Ky(2,71) =0.63016;
Ky(2,72) =0.12638;
Ky(2,73) =-0.33204;
Ky(2,74) =-0.74271;
Ky(2,75) =-1.1034;
Ky(2,76) =-1.4118;
Ky(2,77) =-1.6658;
Ky(2,78) =-1.8633;
Ky(2,79) =-2.0022;
Ky(2,80) =-2.0805;
Ky(2,81) =-2.0961;
Ky(2,82) =-2.0471;
Ky(2,83) =-1.9313;
Ky(2,84) =-1.747;
Ky(2,85) =-1.4919;
Ky(2,86) =-1.1642;
Ky(2,87) =-0.76185;
Ky(2,88) =-0.2829;
Ky(2,89) =0.27465;
Ky(2,90) =0.91276;
Ky(2,91) =1.6334;
Ky(2,92) =2.4386;
Ky(2,93) =3.3303;
Ky(2,94) =4.3104;
Ky(2,95) =5.3811;
Ky(2,96) =6.5441;
Ky(2,97) =7.8015;
Ky(2,98) =9.1553;
Ky(2,99) =10.6075;
Ky(3,0) =2222.0585;
Ky(3,1) =-126.6117;
Ky(3,2) =-1145.9908;
Ky(3,3) =-807.0128;
Ky(3,4) =-249.0014;
Ky(3,5) =61.4254;
Ky(3,6) =141.1219;
Ky(3,7) =110.2466;
Ky(3,8) =48.1863;
Ky(3,9) =-11.2504;
Ky(3,10) =-56.588;
Ky(3,11) =-85.0141;
Ky(3,12) =-97.965;
Ky(3,13) =-99.4507;
Ky(3,14) =-94.3308;
Ky(3,15) =-86.6911;
Ky(3,16) =-78.999;
Ky(3,17) =-72.1614;
Ky(3,18) =-66.1144;
Ky(3,19) =-60.48;
Ky(3,20) =-54.999;
Ky(3,21) =-49.6747;
Ky(3,22) =-44.7083;
Ky(3,23) =-40.3524;
Ky(3,24) =-36.7801;
Ky(3,25) =-34.021;
Ky(3,26) =-31.963;
Ky(3,27) =-30.4011;
Ky(3,28) =-29.1009;
Ky(3,29) =-27.8551;
Ky(3,30) =-26.5189;
Ky(3,31) =-25.0233;
Ky(3,32) =-23.3678;
Ky(3,33) =-21.603;
Ky(3,34) =-19.8074;
Ky(3,35) =-18.0664;
Ky(3,36) =-16.4558;
Ky(3,37) =-15.0315;
Ky(3,38) =-13.8254;
Ky(3,39) =-12.8462;
Ky(3,40) =-12.0825;
Ky(3,41) =-11.5085;
Ky(3,42) =-11.0897;
Ky(3,43) =-10.7878;
Ky(3,44) =-10.5651;
Ky(3,45) =-10.3875;
Ky(3,46) =-10.2261;
Ky(3,47) =-10.0582;
Ky(3,48) =-9.8672;
Ky(3,49) =-9.6422;
Ky(3,50) =-9.3773;
Ky(3,51) =-9.0706;
Ky(3,52) =-8.723;
Ky(3,53) =-8.3377;
Ky(3,54) =-7.9194;
Ky(3,55) =-7.4735;
Ky(3,56) =-7.0056;
Ky(3,57) =-6.5215;
Ky(3,58) =-6.0266;
Ky(3,59) =-5.5261;
Ky(3,60) =-5.0244;
Ky(3,61) =-4.5258;
Ky(3,62) =-4.0339;
Ky(3,63) =-3.552;
Ky(3,64) =-3.083;
Ky(3,65) =-2.6292;
Ky(3,66) =-2.1931;
Ky(3,67) =-1.7764;
Ky(3,68) =-1.3811;
Ky(3,69) =-1.0088;
Ky(3,70) =-0.66087;
Ky(3,71) =-0.33883;
Ky(3,72) =-0.043955;
Ky(3,73) =0.22249;
Ky(3,74) =0.45929;
Ky(3,75) =0.66528;
Ky(3,76) =0.83931;
Ky(3,77) =0.98025;
Ky(3,78) =1.087;
Ky(3,79) =1.1584;
Ky(3,80) =1.1935;
Ky(3,81) =1.191;
Ky(3,82) =1.1501;
Ky(3,83) =1.0694;
Ky(3,84) =0.94808;
Ky(3,85) =0.78494;
Ky(3,86) =0.57895;
Ky(3,87) =0.32902;
Ky(3,88) =0.034078;
Ky(3,89) =-0.30694;
Ky(3,90) =-0.6951;
Ky(3,91) =-1.1315;
Ky(3,92) =-1.6172;
Ky(3,93) =-2.1532;
Ky(3,94) =-2.7407;
Ky(3,95) =-3.3806;
Ky(3,96) =-4.0742;
Ky(3,97) =-4.8224;
Ky(3,98) =-5.6262;
Ky(3,99) =-6.4869;
Ky(4,0) =-1953.3914;
Ky(4,1) =1036.5528;
Ky(4,2) =1231.9365;
Ky(4,3) =150.8841;
Ky(4,4) =-481.3288;
Ky(4,5) =-480.8413;
Ky(4,6) =-219.9255;
Ky(4,7) =24.5315;
Ky(4,8) =165.5478;
Ky(4,9) =214.8637;
Ky(4,10) =207.1731;
Ky(4,11) =171.3656;
Ky(4,12) =126.673;
Ky(4,13) =84.9055;
Ky(4,14) =52.2745;
Ky(4,15) =30.5741;
Ky(4,16) =18.5076;
Ky(4,17) =13.2476;
Ky(4,18) =11.8255;
Ky(4,19) =11.9916;
Ky(4,20) =12.48;
Ky(4,21) =12.8439;
Ky(4,22) =13.1099;
Ky(4,23) =13.4509;
Ky(4,24) =13.9794;
Ky(4,25) =14.6735;
Ky(4,26) =15.4;
Ky(4,27) =15.9805;
Ky(4,28) =16.2587;
Ky(4,29) =16.1444;
Ky(4,30) =15.6279;
Ky(4,31) =14.7699;
Ky(4,32) =13.6769;
Ky(4,33) =12.4729;
Ky(4,34) =11.2749;
Ky(4,35) =10.1763;
Ky(4,36) =9.2385;
Ky(4,37) =8.4902;
Ky(4,38) =7.932;
Ky(4,39) =7.5439;
Ky(4,40) =7.2931;
Ky(4,41) =7.1415;
Ky(4,42) =7.0511;
Ky(4,43) =6.9886;
Ky(4,44) =6.9272;
Ky(4,45) =6.8471;
Ky(4,46) =6.736;
Ky(4,47) =6.5873;
Ky(4,48) =6.3995;
Ky(4,49) =6.1747;
Ky(4,50) =5.917;
Ky(4,51) =5.6322;
Ky(4,52) =5.3263;
Ky(4,53) =5.0052;
Ky(4,54) =4.6747;
Ky(4,55) =4.3395;
Ky(4,56) =4.0038;
Ky(4,57) =3.671;
Ky(4,58) =3.3438;
Ky(4,59) =3.0244;
Ky(4,60) =2.7144;
Ky(4,61) =2.415;
Ky(4,62) =2.1272;
Ky(4,63) =1.8517;
Ky(4,64) =1.589;
Ky(4,65) =1.3397;
Ky(4,66) =1.1041;
Ky(4,67) =0.8825;
Ky(4,68) =0.67536;
Ky(4,69) =0.48295;
Ky(4,70) =0.30563;
Ky(4,71) =0.14375;
Ky(4,72) =-0.002329;
Ky(4,73) =-0.13223;
Ky(4,74) =-0.24555;
Ky(4,75) =-0.34189;
Ky(4,76) =-0.42084;
Ky(4,77) =-0.48197;
Ky(4,78) =-0.52484;
Ky(4,79) =-0.54902;
Ky(4,80) =-0.55407;
Ky(4,81) =-0.53953;
Ky(4,82) =-0.50496;
Ky(4,83) =-0.4499;
Ky(4,84) =-0.3739;
Ky(4,85) =-0.27649;
Ky(4,86) =-0.15723;
Ky(4,87) =-0.015656;
Ky(4,88) =0.1487;
Ky(4,89) =0.33629;
Ky(4,90) =0.54758;
Ky(4,91) =0.78302;
Ky(4,92) =1.0431;
Ky(4,93) =1.3282;
Ky(4,94) =1.6389;
Ky(4,95) =1.9756;
Ky(4,96) =2.3387;
Ky(4,97) =2.7288;
Ky(4,98) =3.1463;
Ky(4,99) =3.5916;
Ky(5,0) =1497.8952;
Ky(5,1) =-1616.8058;
Ky(5,2) =-665.9601;
Ky(5,3) =694.0746;
Ky(5,4) =782.1308;
Ky(5,5) =206.1535;
Ky(5,6) =-278.229;
Ky(5,7) =-437.2388;
Ky(5,8) =-362.4351;
Ky(5,9) =-203.7163;
Ky(5,10) =-56.8717;
Ky(5,11) =42.0427;
Ky(5,12) =91.1598;
Ky(5,13) =101.7876;
Ky(5,14) =87.8484;
Ky(5,15) =61.7953;
Ky(5,16) =33.1754;
Ky(5,17) =8.1833;
Ky(5,18) =-10.1801;
Ky(5,19) =-21.4538;
Ky(5,20) =-26.7822;
Ky(5,21) =-27.9812;
Ky(5,22) =-26.8305;
Ky(5,23) =-24.6798;
Ky(5,24) =-22.3401;
Ky(5,25) =-20.1638;
Ky(5,26) =-18.2061;
Ky(5,27) =-16.3872;
Ky(5,28) =-14.6107;
Ky(5,29) =-12.826;
Ky(5,30) =-11.043;
Ky(5,31) =-9.319;
Ky(5,32) =-7.7315;
Ky(5,33) =-6.3534;
Ky(5,34) =-5.2347;
Ky(5,35) =-4.395;
Ky(5,36) =-3.8241;
Ky(5,37) =-3.4876;
Ky(5,38) =-3.3364;
Ky(5,39) =-3.3157;
Ky(5,40) =-3.3724;
Ky(5,41) =-3.4604;
Ky(5,42) =-3.5438;
Ky(5,43) =-3.5977;
Ky(5,44) =-3.6072;
Ky(5,45) =-3.5666;
Ky(5,46) =-3.4765;
Ky(5,47) =-3.3423;
Ky(5,48) =-3.172;
Ky(5,49) =-2.9747;
Ky(5,50) =-2.7596;
Ky(5,51) =-2.5351;
Ky(5,52) =-2.3082;
Ky(5,53) =-2.0848;
Ky(5,54) =-1.8691;
Ky(5,55) =-1.6641;
Ky(5,56) =-1.4719;
Ky(5,57) =-1.2933;
Ky(5,58) =-1.1287;
Ky(5,59) =-0.97802;
Ky(5,60) =-0.84064;
Ky(5,61) =-0.71589;
Ky(5,62) =-0.60297;
Ky(5,63) =-0.50106;
Ky(5,64) =-0.40936;
Ky(5,65) =-0.32711;
Ky(5,66) =-0.25365;
Ky(5,67) =-0.18838;
Ky(5,68) =-0.1308;
Ky(5,69) =-0.080467;
Ky(5,70) =-0.037024;
Ky(5,71) =-0.00016954;
Ky(5,72) =0.030348;
Ky(5,73) =0.054732;
Ky(5,74) =0.073153;
Ky(5,75) =0.085749;
Ky(5,76) =0.092636;
Ky(5,77) =0.09391;
Ky(5,78) =0.089653;
Ky(5,79) =0.079935;
Ky(5,80) =0.064818;
Ky(5,81) =0.044355;
Ky(5,82) =0.018595;
Ky(5,83) =-0.012416;
Ky(5,84) =-0.048636;
Ky(5,85) =-0.090025;
Ky(5,86) =-0.13654;
Ky(5,87) =-0.18816;
Ky(5,88) =-0.24483;
Ky(5,89) =-0.30652;
Ky(5,90) =-0.3732;
Ky(5,91) =-0.44482;
Ky(5,92) =-0.52137;
Ky(5,93) =-0.60279;
Ky(5,94) =-0.68907;
Ky(5,95) =-0.78015;
Ky(5,96) =-0.87602;
Ky(5,97) =-0.97663;
Ky(5,98) =-1.082;
Ky(5,99) =-1.1919;
Ky(6,0) =-1038.537;
Ky(6,1) =1801.1909;
Ky(6,2) =-162.508;
Ky(6,3) =-1132.9373;
Ky(6,4) =-393.2006;
Ky(6,5) =455.4363;
Ky(6,6) =652.3984;
Ky(6,7) =373.1798;
Ky(6,8) =6.784;
Ky(6,9) =-224.3513;
Ky(6,10) =-285.3217;
Ky(6,11) =-231.5013;
Ky(6,12) =-130.6089;
Ky(6,13) =-30.3574;
Ky(6,14) =45.2129;
Ky(6,15) =89.238;
Ky(6,16) =104.763;
Ky(6,17) =99.3612;
Ky(6,18) =81.6347;
Ky(6,19) =59.1214;
Ky(6,20) =37.2652;
Ky(6,21) =19.2005;
Ky(6,22) =6.0865;
Ky(6,23) =-2.2838;
Ky(6,24) =-6.8433;
Ky(6,25) =-8.7347;
Ky(6,26) =-8.9845;
Ky(6,27) =-8.3567;
Ky(6,28) =-7.3367;
Ky(6,29) =-6.1874;
Ky(6,30) =-5.0268;
Ky(6,31) =-3.8987;
Ky(6,32) =-2.8202;
Ky(6,33) =-1.8072;
Ky(6,34) =-0.8818;
Ky(6,35) =-0.069692;
Ky(6,36) =0.60639;
Ky(6,37) =1.1324;
Ky(6,38) =1.506;
Ky(6,39) =1.7368;
Ky(6,40) =1.8442;
Ky(6,41) =1.8535;
Ky(6,42) =1.7924;
Ky(6,43) =1.6873;
Ky(6,44) =1.5614;
Ky(6,45) =1.4328;
Ky(6,46) =1.3141;
Ky(6,47) =1.2129;
Ky(6,48) =1.1326;
Ky(6,49) =1.0729;
Ky(6,50) =1.0313;
Ky(6,51) =1.0042;
Ky(6,52) =0.98696;
Ky(6,53) =0.97538;
Ky(6,54) =0.96553;
Ky(6,55) =0.95418;
Ky(6,56) =0.9389;
Ky(6,57) =0.91802;
Ky(6,58) =0.89059;
Ky(6,59) =0.85625;
Ky(6,60) =0.81513;
Ky(6,61) =0.76773;
Ky(6,62) =0.71477;
Ky(6,63) =0.65716;
Ky(6,64) =0.59587;
Ky(6,65) =0.53189;
Ky(6,66) =0.46623;
Ky(6,67) =0.39981;
Ky(6,68) =0.33352;
Ky(6,69) =0.26816;
Ky(6,70) =0.20448;
Ky(6,71) =0.14316;
Ky(6,72) =0.084806;
Ky(6,73) =0.029976;
Ky(6,74) =-0.020819;
Ky(6,75) =-0.067111;
Ky(6,76) =-0.10846;
Ky(6,77) =-0.14448;
Ky(6,78) =-0.17476;
Ky(6,79) =-0.19895;
Ky(6,80) =-0.21671;
Ky(6,81) =-0.22768;
Ky(6,82) =-0.23156;
Ky(6,83) =-0.228;
Ky(6,84) =-0.2167;
Ky(6,85) =-0.19734;
Ky(6,86) =-0.16962;
Ky(6,87) =-0.13322;
Ky(6,88) =-0.087853;
Ky(6,89) =-0.033202;
Ky(6,90) =0.031031;
Ky(6,91) =0.10515;
Ky(6,92) =0.18945;
Ky(6,93) =0.28425;
Ky(6,94) =0.38983;
Ky(6,95) =0.50649;
Ky(6,96) =0.63455;
Ky(6,97) =0.7743;
Ky(6,98) =0.92603;
Ky(6,99) =1.0901;
Ky(7,0) =670.4244;
Ky(7,1) =-1680.796;
Ky(7,2) =908.3888;
Ky(7,3) =1015.3453;
Ky(7,4) =-329.7224;
Ky(7,5) =-859.2383;
Ky(7,6) =-425.6051;
Ky(7,7) =183.8567;
Ky(7,8) =466.5252;
Ky(7,9) =397.633;
Ky(7,10) =165.2241;
Ky(7,11) =-58.5553;
Ky(7,12) =-192.6379;
Ky(7,13) =-229.5406;
Ky(7,14) =-196.4995;
Ky(7,15) =-127.6872;
Ky(7,16) =-51.3783;
Ky(7,17) =13.6954;
Ky(7,18) =58.0324;
Ky(7,19) =79.6346;
Ky(7,20) =81.7362;
Ky(7,21) =70.3798;
Ky(7,22) =52.2197;
Ky(7,23) =32.906;
Ky(7,24) =16.2296;
Ky(7,25) =3.9854;
Ky(7,26) =-3.6577;
Ky(7,27) =-7.5496;
Ky(7,28) =-8.9508;
Ky(7,29) =-9.0686;
Ky(7,30) =-8.7935;
Ky(7,31) =-8.6195;
Ky(7,32) =-8.6922;
Ky(7,33) =-8.9233;
Ky(7,34) =-9.1163;
Ky(7,35) =-9.0706;
Ky(7,36) =-8.6473;
Ky(7,37) =-7.7974;
Ky(7,38) =-6.559;
Ky(7,39) =-5.0355;
Ky(7,40) =-3.3667;
Ky(7,41) =-1.6995;
Ky(7,42) =-0.16552;
Ky(7,43) =1.1342;
Ky(7,44) =2.1355;
Ky(7,45) =2.811;
Ky(7,46) =3.1658;
Ky(7,47) =3.2296;
Ky(7,48) =3.0484;
Ky(7,49) =2.6773;
Ky(7,50) =2.173;
Ky(7,51) =1.5894;
Ky(7,52) =0.9738;
Ky(7,53) =0.36547;
Ky(7,54) =-0.20537;
Ky(7,55) =-0.71711;
Ky(7,56) =-1.1559;
Ky(7,57) =-1.5146;
Ky(7,58) =-1.7912;
Ky(7,59) =-1.9879;
Ky(7,60) =-2.1095;
Ky(7,61) =-2.1628;
Ky(7,62) =-2.1556;
Ky(7,63) =-2.0961;
Ky(7,64) =-1.9924;
Ky(7,65) =-1.8523;
Ky(7,66) =-1.6831;
Ky(7,67) =-1.4913;
Ky(7,68) =-1.2829;
Ky(7,69) =-1.0631;
Ky(7,70) =-0.83665;
Ky(7,71) =-0.60764;
Ky(7,72) =-0.37967;
Ky(7,73) =-0.15596;
Ky(7,74) =0.060635;
Ky(7,75) =0.26756;
Ky(7,76) =0.4625;
Ky(7,77) =0.6433;
Ky(7,78) =0.80799;
Ky(7,79) =0.95472;
Ky(7,80) =1.0817;
Ky(7,81) =1.1873;
Ky(7,82) =1.2698;
Ky(7,83) =1.3277;
Ky(7,84) =1.3594;
Ky(7,85) =1.3633;
Ky(7,86) =1.338;
Ky(7,87) =1.282;
Ky(7,88) =1.1937;
Ky(7,89) =1.0717;
Ky(7,90) =0.91455;
Ky(7,91) =0.7207;
Ky(7,92) =0.48872;
Ky(7,93) =0.21714;
Ky(7,94) =-0.095501;
Ky(7,95) =-0.45067;
Ky(7,96) =-0.84984;
Ky(7,97) =-1.2945;
Ky(7,98) =-1.786;
Ky(7,99) =-2.3259;
Ky(8,0) =-406.2389;
Ky(8,1) =1403.5639;
Ky(8,2) =-1380.0437;
Ky(8,3) =-500.948;
Ky(8,4) =935.5212;
Ky(8,5) =722.2548;
Ky(8,6) =-207.198;
Ky(8,7) =-660.4423;
Ky(8,8) =-458.5676;
Ky(8,9) =-14.8505;
Ky(8,10) =295.9934;
Ky(8,11) =361.466;
Ky(8,12) =252.9414;
Ky(8,13) =86.5419;
Ky(8,14) =-56.3412;
Ky(8,15) =-143.1176;
Ky(8,16) =-173.5079;
Ky(8,17) =-160.6552;
Ky(8,18) =-120.2281;
Ky(8,19) =-66.5544;
Ky(8,20) =-11.7963;
Ky(8,21) =34.4892;
Ky(8,22) =66.0437;
Ky(8,23) =80.4272;
Ky(8,24) =78.8689;
Ky(8,25) =65.3542;
Ky(8,26) =45.237;
Ky(8,27) =23.8226;
Ky(8,28) =5.2901;
Ky(8,29) =-7.8627;
Ky(8,30) =-14.8604;
Ky(8,31) =-16.3176;
Ky(8,32) =-13.7176;
Ky(8,33) =-8.8753;
Ky(8,34) =-3.4987;
Ky(8,35) =1.0932;
Ky(8,36) =4.0927;
Ky(8,37) =5.1961;
Ky(8,38) =4.5105;
Ky(8,39) =2.42;
Ky(8,40) =-0.55153;
Ky(8,41) =-3.8581;
Ky(8,42) =-7.0168;
Ky(8,43) =-9.6578;
Ky(8,44) =-11.5442;
Ky(8,45) =-12.5676;
Ky(8,46) =-12.7289;
Ky(8,47) =-12.1111;
Ky(8,48) =-10.8505;
Ky(8,49) =-9.1108;
Ky(8,50) =-7.0608;
Ky(8,51) =-4.8589;
Ky(8,52) =-2.6427;
Ky(8,53) =-0.5236;
Ky(8,54) =1.4147;
Ky(8,55) =3.115;
Ky(8,56) =4.5437;
Ky(8,57) =5.6868;
Ky(8,58) =6.5457;
Ky(8,59) =7.1331;
Ky(8,60) =7.4695;
Ky(8,61) =7.5801;
Ky(8,62) =7.4927;
Ky(8,63) =7.2353;
Ky(8,64) =6.8355;
Ky(8,65) =6.3192;
Ky(8,66) =5.7102;
Ky(8,67) =5.0302;
Ky(8,68) =4.2986;
Ky(8,69) =3.5326;
Ky(8,70) =2.7472;
Ky(8,71) =1.9559;
Ky(8,72) =1.1705;
Ky(8,73) =0.40163;
Ky(8,74) =-0.34139;
Ky(8,75) =-1.05;
Ky(8,76) =-1.7165;
Ky(8,77) =-2.3336;
Ky(8,78) =-2.8946;
Ky(8,79) =-3.3933;
Ky(8,80) =-3.8234;
Ky(8,81) =-4.1792;
Ky(8,82) =-4.4551;
Ky(8,83) =-4.6454;
Ky(8,84) =-4.7449;
Ky(8,85) =-4.748;
Ky(8,86) =-4.6495;
Ky(8,87) =-4.4441;
Ky(8,88) =-4.1267;
Ky(8,89) =-3.692;
Ky(8,90) =-3.1347;
Ky(8,91) =-2.4498;
Ky(8,92) =-1.6321;
Ky(8,93) =-0.67635;
Ky(8,94) =0.42256;
Ky(8,95) =1.6698;
Ky(8,96) =3.0705;
Ky(8,97) =4.6298;
Ky(8,98) =6.3529;
Ky(8,99) =8.2449;
Ky(9,0) =235.2281;
Ky(9,1) =-1076.4063;
Ky(9,2) =1549.7368;
Ky(9,3) =-128.6659;
Ky(9,4) =-1163.9795;
Ky(9,5) =-182.3145;
Ky(9,6) =779.2215;
Ky(9,7) =654.7505;
Ky(9,8) =-39.9959;
Ky(9,9) =-509.6031;
Ky(9,10) =-485.3732;
Ky(9,11) =-165.7624;
Ky(9,12) =149.1733;
Ky(9,13) =294.7521;
Ky(9,14) =269.2675;
Ky(9,15) =153.6646;
Ky(9,16) =29.6598;
Ky(9,17) =-58.4869;
Ky(9,18) =-103.1166;
Ky(9,19) =-115.5367;
Ky(9,20) =-109.6733;
Ky(9,21) =-94.225;
Ky(9,22) =-72.354;
Ky(9,23) =-44.8284;
Ky(9,24) =-13.0511;
Ky(9,25) =19.6937;
Ky(9,26) =48.7649;
Ky(9,27) =69.6041;
Ky(9,28) =79.1503;
Ky(9,29) =76.6362;
Ky(9,30) =63.6046;
Ky(9,31) =43.3101;
Ky(9,32) =19.814;
Ky(9,33) =-2.9201;
Ky(9,34) =-21.717;
Ky(9,35) =-34.5717;
Ky(9,36) =-40.7258;
Ky(9,37) =-40.5098;
Ky(9,38) =-35.0467;
Ky(9,39) =-25.9077;
Ky(9,40) =-14.7929;
Ky(9,41) =-3.2801;
Ky(9,42) =7.3415;
Ky(9,43) =16.1524;
Ky(9,44) =22.6152;
Ky(9,45) =26.5367;
Ky(9,46) =28.0007;
Ky(9,47) =27.2893;
Ky(9,48) =24.8049;
Ky(9,49) =21.0036;
Ky(9,50) =16.3411;
Ky(9,51) =11.2358;
Ky(9,52) =6.0455;
Ky(9,53) =1.0563;
Ky(9,54) =-3.5188;
Ky(9,55) =-7.5353;
Ky(9,56) =-10.9085;
Ky(9,57) =-13.6027;
Ky(9,58) =-15.6204;
Ky(9,59) =-16.9921;
Ky(9,60) =-17.7671;
Ky(9,61) =-18.0061;
Ky(9,62) =-17.7754;
Ky(9,63) =-17.1425;
Ky(9,64) =-16.1731;
Ky(9,65) =-14.9288;
Ky(9,66) =-13.4666;
Ky(9,67) =-11.8378;
Ky(9,68) =-10.0882;
Ky(9,69) =-8.2585;
Ky(9,70) =-6.3845;
Ky(9,71) =-4.4977;
Ky(9,72) =-2.6261;
Ky(9,73) =-0.79453;
Ky(9,74) =0.97493;
Ky(9,75) =2.6622;
Ky(9,76) =4.249;
Ky(9,77) =5.7183;
Ky(9,78) =7.0545;
Ky(9,79) =8.2424;
Ky(9,80) =9.268;
Ky(9,81) =10.1173;
Ky(9,82) =10.7772;
Ky(9,83) =11.2345;
Ky(9,84) =11.4764;
Ky(9,85) =11.4903;
Ky(9,86) =11.2636;
Ky(9,87) =10.7841;
Ky(9,88) =10.0392;
Ky(9,89) =9.0169;
Ky(9,90) =7.7046;
Ky(9,91) =6.0904;
Ky(9,92) =4.1619;
Ky(9,93) =1.907;
Ky(9,94) =-0.68649;
Ky(9,95) =-3.6308;
Ky(9,96) =-6.938;
Ky(9,97) =-10.6203;
Ky(9,98) =-14.6899;
Ky(9,99) =-19.1588;
Ky(10,0) =-130.2172;
Ky(10,1) =768.6952;
Ky(10,2) =-1464.6153;
Ky(10,3) =638.3055;
Ky(10,4) =1016.1581;
Ky(10,5) =-417.3751;
Ky(10,6) =-970.0648;
Ky(10,7) =-222.756;
Ky(10,8) =603.515;
Ky(10,9) =684.893;
Ky(10,10) =175.8568;
Ky(10,11) =-352.0641;
Ky(10,12) =-523.8104;
Ky(10,13) =-338.0286;
Ky(10,14) =-12.1094;
Ky(10,15) =236.6318;
Ky(10,16) =310.5656;
Ky(10,17) =233.2631;
Ky(10,18) =88.7638;
Ky(10,19) =-41.5969;
Ky(10,20) =-113.6603;
Ky(10,21) =-123.8157;
Ky(10,22) =-93.9278;
Ky(10,23) =-52.3331;
Ky(10,24) =-20.2335;
Ky(10,25) =-6.39;
Ky(10,26) =-8.5659;
Ky(10,27) =-18.3916;
Ky(10,28) =-26.574;
Ky(10,29) =-26.5654;
Ky(10,30) =-16.095;
Ky(10,31) =3.1254;
Ky(10,32) =26.7845;
Ky(10,33) =49.682;
Ky(10,34) =67.2014;
Ky(10,35) =76.2532;
Ky(10,36) =75.6324;
Ky(10,37) =65.8926;
Ky(10,38) =48.9104;
Ky(10,39) =27.3218;
Ky(10,40) =3.9724;
Ky(10,41) =-18.5247;
Ky(10,42) =-38.0821;
Ky(10,43) =-53.2812;
Ky(10,44) =-63.3885;
Ky(10,45) =-68.2809;
Ky(10,46) =-68.3184;
Ky(10,47) =-64.1958;
Ky(10,48) =-56.7986;
Ky(10,49) =-47.0793;
Ky(10,50) =-35.9609;
Ky(10,51) =-24.271;
Ky(10,52) =-12.7024;
Ky(10,53) =-1.7972;
Ky(10,54) =8.0525;
Ky(10,55) =16.5914;
Ky(10,56) =23.6823;
Ky(10,57) =29.283;
Ky(10,58) =33.4245;
Ky(10,59) =36.1897;
Ky(10,60) =37.6957;
Ky(10,61) =38.0794;
Ky(10,62) =37.4858;
Ky(10,63) =36.06;
Ky(10,64) =33.9413;
Ky(10,65) =31.2597;
Ky(10,66) =28.1338;
Ky(10,67) =24.6705;
Ky(10,68) =20.9648;
Ky(10,69) =17.1006;
Ky(10,70) =13.152;
Ky(10,71) =9.1839;
Ky(10,72) =5.254;
Ky(10,73) =1.4134;
Ky(10,74) =-2.2921;
Ky(10,75) =-5.8211;
Ky(10,76) =-9.1356;
Ky(10,77) =-12.2005;
Ky(10,78) =-14.983;
Ky(10,79) =-17.4518;
Ky(10,80) =-19.5773;
Ky(10,81) =-21.3307;
Ky(10,82) =-22.6841;
Ky(10,83) =-23.6101;
Ky(10,84) =-24.082;
Ky(10,85) =-24.0731;
Ky(10,86) =-23.5574;
Ky(10,87) =-22.5086;
Ky(10,88) =-20.9008;
Ky(10,89) =-18.7084;
Ky(10,90) =-15.9054;
Ky(10,91) =-12.4663;
Ky(10,92) =-8.3654;
Ky(10,93) =-3.5771;
Ky(10,94) =1.9243;
Ky(10,95) =8.1642;
Ky(10,96) =15.1683;
Ky(10,97) =22.9621;
Ky(10,98) =31.5712;
Ky(10,99) =41.021;
Ky(11,0) =70.3079;
Ky(11,1) =-505.0468;
Ky(11,2) =1201.5321;
Ky(11,3) =-880.4058;
Ky(11,4) =-651.0829;
Ky(11,5) =782.1626;
Ky(11,6) =775.5786;
Ky(11,7) =-278.8846;
Ky(11,8) =-838.9249;
Ky(11,9) =-431.1502;
Ky(11,10) =319.4115;
Ky(11,11) =687.4495;
Ky(11,12) =477.4567;
Ky(11,13) =-27.7206;
Ky(11,14) =-433.444;
Ky(11,15) =-521.5162;
Ky(11,16) =-316.2923;
Ky(11,17) =11.1039;
Ky(11,18) =278.3971;
Ky(11,19) =381.9082;
Ky(11,20) =316.7836;
Ky(11,21) =147.0943;
Ky(11,22) =-41.3409;
Ky(11,23) =-179.6621;
Ky(11,24) =-234.8356;
Ky(11,25) =-209.5441;
Ky(11,26) =-130.3687;
Ky(11,27) =-32.5145;
Ky(11,28) =52.8966;
Ky(11,29) =106.1514;
Ky(11,30) =120.7748;
Ky(11,31) =101.3457;
Ky(11,32) =59.2865;
Ky(11,33) =8.3748;
Ky(11,34) =-38.8698;
Ky(11,35) =-73.4144;
Ky(11,36) =-90.5496;
Ky(11,37) =-89.6334;
Ky(11,38) =-73.1604;
Ky(11,39) =-45.5545;
Ky(11,40) =-11.9943;
Ky(11,41) =22.5372;
Ky(11,42) =53.8976;
Ky(11,43) =79.1207;
Ky(11,44) =96.494;
Ky(11,45) =105.4502;
Ky(11,46) =106.3465;
Ky(11,47) =100.1993;
Ky(11,48) =88.4194;
Ky(11,49) =72.5823;
Ky(11,50) =54.2478;
Ky(11,51) =34.833;
Ky(11,52) =15.5366;
Ky(11,53) =-2.6962;
Ky(11,54) =-19.1757;
Ky(11,55) =-33.4486;
Ky(11,56) =-45.2663;
Ky(11,57) =-54.5463;
Ky(11,58) =-61.334;
Ky(11,59) =-65.7666;
Ky(11,60) =-68.0419;
Ky(11,61) =-68.3928;
Ky(11,62) =-67.0676;
Ky(11,63) =-64.3154;
Ky(11,64) =-60.3756;
Ky(11,65) =-55.4723;
Ky(11,66) =-49.8103;
Ky(11,67) =-43.5741;
Ky(11,68) =-36.9281;
Ky(11,69) =-30.0175;
Ky(11,70) =-22.9706;
Ky(11,71) =-15.9005;
Ky(11,72) =-8.9072;
Ky(11,73) =-2.0797;
Ky(11,74) =4.5021;
Ky(11,75) =10.7658;
Ky(11,76) =16.645;
Ky(11,77) =22.0782;
Ky(11,78) =27.0077;
Ky(11,79) =31.3788;
Ky(11,80) =35.1392;
Ky(11,81) =38.2382;
Ky(11,82) =40.6266;
Ky(11,83) =42.2563;
Ky(11,84) =43.0799;
Ky(11,85) =43.0507;
Ky(11,86) =42.1222;
Ky(11,87) =40.2484;
Ky(11,88) =37.3837;
Ky(11,89) =33.4823;
Ky(11,90) =28.4988;
Ky(11,91) =22.3879;
Ky(11,92) =15.1042;
Ky(11,93) =6.6025;
Ky(11,94) =-3.1624;
Ky(11,95) =-14.2357;
Ky(11,96) =-26.6627;
Ky(11,97) =-40.4884;
Ky(11,98) =-55.7581;
Ky(11,99) =-72.5168;
Ky(12,0) =-36.7889;
Ky(12,1) =291.5829;
Ky(12,2) =-829.6321;
Ky(12,3) =810.2997;
Ky(12,4) =277.7842;
Ky(12,5) =-781.8274;
Ky(12,6) =-409.6371;
Ky(12,7) =516.684;
Ky(12,8) =678.6318;
Ky(12,9) =52.1351;
Ky(12,10) =-563.6685;
Ky(12,11) =-605.6434;
Ky(12,12) =-148.214;
Ky(12,13) =370.0353;
Ky(12,14) =579.9168;
Ky(12,15) =403.3642;
Ky(12,16) =11.3558;
Ky(12,17) =-347.3038;
Ky(12,18) =-499.0428;
Ky(12,19) =-408.3123;
Ky(12,20) =-154.5937;
Ky(12,21) =133.4086;
Ky(12,22) =342.384;
Ky(12,23) =412.3624;
Ky(12,24) =343.2881;
Ky(12,25) =179.058;
Ky(12,26) =-17.6578;
Ky(12,27) =-188.5785;
Ky(12,28) =-294.6584;
Ky(12,29) =-321.2109;
Ky(12,30) =-275.2147;
Ky(12,31) =-178.1319;
Ky(12,32) =-57.503;
Ky(12,33) =60.3089;
Ky(12,34) =154.9878;
Ky(12,35) =214.5029;
Ky(12,36) =235.1233;
Ky(12,37) =219.9354;
Ky(12,38) =176.6252;
Ky(12,39) =115.1574;
Ky(12,40) =45.7759;
Ky(12,41) =-22.4525;
Ky(12,42) =-82.5034;
Ky(12,43) =-129.7307;
Ky(12,44) =-161.7964;
Ky(12,45) =-178.3368;
Ky(12,46) =-180.4918;
Ky(12,47) =-170.4039;
Ky(12,48) =-150.7556;
Ky(12,49) =-124.3871;
Ky(12,50) =-94.0125;
Ky(12,51) =-62.0311;
Ky(12,52) =-30.4248;
Ky(12,53) =-0.72161;
Ky(12,54) =25.9911;
Ky(12,55) =49.0221;
Ky(12,56) =68.014;
Ky(12,57) =82.8761;
Ky(12,58) =93.7177;
Ky(12,59) =100.7882;
Ky(12,60) =104.4269;
Ky(12,61) =105.0224;
Ky(12,62) =102.9812;
Ky(12,63) =98.706;
Ky(12,64) =92.5805;
Ky(12,65) =84.9604;
Ky(12,66) =76.169;
Ky(12,67) =66.4955;
Ky(12,68) =56.1966;
Ky(12,69) =45.4981;
Ky(12,70) =34.5987;
Ky(12,71) =23.673;
Ky(12,72) =12.8751;
Ky(12,73) =2.3422;
Ky(12,74) =-7.803;
Ky(12,75) =-17.4492;
Ky(12,76) =-26.4944;
Ky(12,77) =-34.8441;
Ky(12,78) =-42.4097;
Ky(12,79) =-49.1068;
Ky(12,80) =-54.855;
Ky(12,81) =-59.5762;
Ky(12,82) =-63.1948;
Ky(12,83) =-65.6363;
Ky(12,84) =-66.8277;
Ky(12,85) =-66.6968;
Ky(12,86) =-65.172;
Ky(12,87) =-62.1823;
Ky(12,88) =-57.657;
Ky(12,89) =-51.5257;
Ky(12,90) =-43.7181;
Ky(12,91) =-34.1641;
Ky(12,92) =-22.7937;
Ky(12,93) =-9.5369;
Ky(12,94) =5.676;
Ky(12,95) =22.915;
Ky(12,96) =42.2499;
Ky(12,97) =63.7504;
Ky(12,98) =87.4864;
Ky(12,99) =113.5275;
Ky(13,0) =18.2879;
Ky(13,1) =-127.1661;
Ky(13,2) =424.6912;
Ky(13,3) =-486.9865;
Ky(13,4) =-52.0548;
Ky(13,5) =482.4366;
Ky(13,6) =123.7535;
Ky(13,7) =-391.0702;
Ky(13,8) =-334.621;
Ky(13,9) =120.5941;
Ky(13,10) =418.0919;
Ky(13,11) =298.2944;
Ky(13,12) =-67.3105;
Ky(13,13) =-351.2695;
Ky(13,14) =-360.8781;
Ky(13,15) =-130.3287;
Ky(13,16) =165.1451;
Ky(13,17) =350.0301;
Ky(13,18) =343.1417;
Ky(13,19) =173.4443;
Ky(13,20) =-61.7693;
Ky(13,21) =-256.3142;
Ky(13,22) =-340.2706;
Ky(13,23) =-297.5131;
Ky(13,24) =-158.3783;
Ky(13,25) =21.5189;
Ky(13,26) =184.5523;
Ky(13,27) =288.3346;
Ky(13,28) =313.7238;
Ky(13,29) =264.3025;
Ky(13,30) =160.211;
Ky(13,31) =29.7133;
Ky(13,32) =-98.7408;
Ky(13,33) =-202.2924;
Ky(13,34) =-266.7089;
Ky(13,35) =-286.9112;
Ky(13,36) =-265.6782;
Ky(13,37) =-211.3431;
Ky(13,38) =-135.2132;
Ky(13,39) =-49.2421;
Ky(13,40) =35.7445;
Ky(13,41) =111.1638;
Ky(13,42) =171.1731;
Ky(13,43) =212.6714;
Ky(13,44) =234.9617;
Ky(13,45) =239.226;
Ky(13,46) =227.9379;
Ky(13,47) =204.3052;
Ky(13,48) =171.7969;
Ky(13,49) =133.7821;
Ky(13,50) =93.2829;
Ky(13,51) =52.8318;
Ky(13,52) =14.4135;
Ky(13,53) =-20.5303;
Ky(13,54) =-51.0539;
Ky(13,55) =-76.6362;
Ky(13,56) =-97.0991;
Ky(13,57) =-112.5264;
Ky(13,58) =-123.1889;
Ky(13,59) =-129.4806;
Ky(13,60) =-131.8659;
Ky(13,61) =-130.8396;
Ky(13,62) =-126.8972;
Ky(13,63) =-120.5148;
Ky(13,64) =-112.1363;
Ky(13,65) =-102.1666;
Ky(13,66) =-90.9694;
Ky(13,67) =-78.8673;
Ky(13,68) =-66.1447;
Ky(13,69) =-53.0514;
Ky(13,70) =-39.8064;
Ky(13,71) =-26.6027;
Ky(13,72) =-13.6111;
Ky(13,73) =-0.98387;
Ky(13,74) =11.1416;
Ky(13,75) =22.6402;
Ky(13,76) =33.3965;
Ky(13,77) =43.3026;
Ky(13,78) =52.2567;
Ky(13,79) =60.1619;
Ky(13,80) =66.9244;
Ky(13,81) =72.4537;
Ky(13,82) =76.661;
Ky(13,83) =79.4593;
Ky(13,84) =80.7626;
Ky(13,85) =80.486;
Ky(13,86) =78.545;
Ky(13,87) =74.8558;
Ky(13,88) =69.3348;
Ky(13,89) =61.8988;
Ky(13,90) =52.4648;
Ky(13,91) =40.9497;
Ky(13,92) =27.2707;
Ky(13,93) =11.3451;
Ky(13,94) =-6.9098;
Ky(13,95) =-27.5768;
Ky(13,96) =-50.7385;
Ky(13,97) =-76.4775;
Ky(13,98) =-104.8765;
Ky(13,99) =-136.0181;
Ky(14,0) =-6.0626;
Ky(14,1) =29.0642;
Ky(14,2) =-114.6386;
Ky(14,3) =141.5761;
Ky(14,4) =-5.9518;
Ky(14,5) =-142.1753;
Ky(14,6) =-13.4841;
Ky(14,7) =125.1776;
Ky(14,8) =78.6001;
Ky(14,9) =-64.6978;
Ky(14,10) =-131.4908;
Ky(14,11) =-67.0642;
Ky(14,12) =52.0714;
Ky(14,13) =121.4138;
Ky(14,14) =96.1299;
Ky(14,15) =5.791;
Ky(14,16) =-84.6638;
Ky(14,17) =-122.7452;
Ky(14,18) =-94.1421;
Ky(14,19) =-20.5961;
Ky(14,20) =59.1176;
Ky(14,21) =110.3419;
Ky(14,22) =115.8462;
Ky(14,23) =78.4614;
Ky(14,24) =15.542;
Ky(14,25) =-50.091;
Ky(14,26) =-98.5186;
Ky(14,27) =-117.9423;
Ky(14,28) =-106.1665;
Ky(14,29) =-69.0852;
Ky(14,30) =-17.5079;
Ky(14,31) =36.3564;
Ky(14,32) =81.8043;
Ky(14,33) =111.4539;
Ky(14,34) =121.9116;
Ky(14,35) =113.4995;
Ky(14,36) =89.3829;
Ky(14,37) =54.4432;
Ky(14,38) =14.1713;
Ky(14,39) =-26.2359;
Ky(14,40) =-62.4935;
Ky(14,41) =-91.5531;
Ky(14,42) =-111.6745;
Ky(14,43) =-122.3138;
Ky(14,44) =-123.8982;
Ky(14,45) =-117.5513;
Ky(14,46) =-104.8191;
Ky(14,47) =-87.4281;
Ky(14,48) =-67.0936;
Ky(14,49) =-45.383;
Ky(14,50) =-23.6322;
Ky(14,51) =-2.9052;
Ky(14,52) =16.0107;
Ky(14,53) =32.5866;
Ky(14,54) =46.519;
Ky(14,55) =57.6898;
Ky(14,56) =66.1244;
Ky(14,57) =71.9532;
Ky(14,58) =75.3773;
Ky(14,59) =76.6398;
Ky(14,60) =76.0041;
Ky(14,61) =73.7369;
Ky(14,62) =70.0967;
Ky(14,63) =65.3262;
Ky(14,64) =59.6483;
Ky(14,65) =53.2641;
Ky(14,66) =46.3531;
Ky(14,67) =39.0739;
Ky(14,68) =31.5662;
Ky(14,69) =23.9531;
Ky(14,70) =16.3428;
Ky(14,71) =8.8317;
Ky(14,72) =1.5055;
Ky(14,73) =-5.5584;
Ky(14,74) =-12.2895;
Ky(14,75) =-18.6226;
Ky(14,76) =-24.4973;
Ky(14,77) =-29.8561;
Ky(14,78) =-34.6447;
Ky(14,79) =-38.8104;
Ky(14,80) =-42.3025;
Ky(14,81) =-45.0712;
Ky(14,82) =-47.0678;
Ky(14,83) =-48.2441;
Ky(14,84) =-48.5526;
Ky(14,85) =-47.9461;
Ky(14,86) =-46.3778;
Ky(14,87) =-43.8008;
Ky(14,88) =-40.1688;
Ky(14,89) =-35.4351;
Ky(14,90) =-29.5536;
Ky(14,91) =-22.4779;
Ky(14,92) =-14.1617;
Ky(14,93) =-4.5589;
Ky(14,94) =6.3769;
Ky(14,95) =18.6916;
Ky(14,96) =32.4316;
Ky(14,97) =47.6429;
Ky(14,98) =64.3718;
Ky(14,99) =82.6644;

Kx(0,0) =33165.3449;
Kx(0,1) =6008.3209;
Kx(0,2) =109.8615;
Kx(0,3) =943.0008;
Kx(1,0) =-76268.8962;
Kx(1,1) =-13126.9731;
Kx(1,2) =-107.271;
Kx(1,3) =-2270.0421;
Kx(2,0) =69080.5449;
Kx(2,1) =11678.0874;
Kx(2,2) =77.0745;
Kx(2,3) =2106.8673;
Kx(3,0) =-44867.8826;
Kx(3,1) =-7612.2442;
Kx(3,2) =-44.8184;
Kx(3,3) =-1359.8155;
Kx(4,0) =26485.3956;
Kx(4,1) =4430.5371;
Kx(4,2) =23.5551;
Kx(4,3) =815.6437;
Kx(5,0) =-12447.8626;
Kx(5,1) =-2108.6838;
Kx(5,2) =-10.0277;
Kx(5,3) =-379.6233;
Kx(6,0) =5658.7676;
Kx(6,1) =927.8259;
Kx(6,2) =3.7251;
Kx(6,3) =176.2121;
Kx(7,0) =-1463.341;
Kx(7,1) =-253.8747;
Kx(7,2) =-0.51241;
Kx(7,3) =-43.8846;
Kx(8,0) =179.0657;
Kx(8,1) =14.9357;
Kx(8,2) =-0.41952;
Kx(8,3) =1.9005;
Kx(9,0) =732.8042;
Kx(9,1) =120.1705;
Kx(9,2) =0.87385;
Kx(9,3) =32.1943;
Kx(10,0) =-951.0302;
Kx(10,1) =-174.9591;
Kx(10,2) =-1.0048;
Kx(10,3) =-51.2484;
Kx(11,0) =1483.1165;
Kx(11,1) =258.448;
Kx(11,2) =1.3398;
Kx(11,3) =83.457;
Kx(12,0) =-1937.8253;
Kx(12,1) =-352.0039;
Kx(12,2) =-1.7667;
Kx(12,3) =-120.0635;
Kx(13,0) =2249.4416;
Kx(13,1) =402.6022;
Kx(13,2) =2.0029;
Kx(13,3) =140.2386;
Kx(14,0) =-1242.0998;
Kx(14,1) =-230.4049;
Kx(14,2) =-1.1444;
Kx(14,3) =-81.0641;

    L0(0) =0.81462;
    L0(1) =-0.47248;
    L0(2) =0.27404;
    L0(3) =-0.15894;
    L0(4) =0.092186;
    L0(5) =-0.053468;
    L0(6) =0.031011;
    L0(7) =-0.017987;
    L0(8) =0.010432;
    L0(9) =-0.0060507;
    L0(10) =0.0035094;
    L0(11) =-0.0020355;
    L0(12) =0.0011806;
    L0(13) =-0.00068473;
    L0(14) =0.00039714;
}

