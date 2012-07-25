function [] = inverseLeftHand()
close all;
%clear all;
clc;
%% Nao kinematics for the left leg

global shoulderOffsetY
global elbowOffsetY
global upperArmLength
global shoulderOffsetZ
global LowerArmLength
global HandOffsetX
global HipOffsetZ
global HipOffsetY
global ThighLength
global TibiaLength
global FootHeight
global NeckOffsetZ

shoulderOffsetY = 98;
elbowOffsetY = 15;
upperArmLength = 105;
shoulderOffsetZ = 100;
LowerArmLength = 57.75;
HandOffsetX = 55.95;
HipOffsetZ = 85;
HipOffsetY = 50;
ThighLength = 100;
TibiaLength = 102.90;
FootHeight = 45.11;
NeckOffsetZ = 126.5;

%to d einai h y apostash apo ton proigoumeno joint
%to a einai h x apostash apo ton proigoumeno joint
%to alpha einai h diafora gwnias Z apo to proigoumeno joint
%to theta einai h gwnia tou joint
thetasLL(1) = deg2rad(20);
thetasLL(2) = deg2rad(20);%Roll
thetasLL(3) = deg2rad(-30);
thetasLL(4) = deg2rad(15);%pi/2;
thetasLL(5) = deg2rad(-60);
thetasLL(6) = deg2rad(35)
T=fLeftLeg(thetasLL);
xangle = -0.0250973;
yangle = 0.0813714;
Rt = Rofl2(xangle,yangle,0);
Rt2 = Rt;
Rt2(3,4) = -300;
Rt3 = (Rt^-1)*Rt2;


%inverseLeft(Rt3(1,4),Rt3(2,4),Rt3(3,4),-xangle,-yangle,0);
end

function [iLeft] = inverseLeft(px,py,pz,rx,ry,rz)
global HipOffsetZ
global HipOffsetY
global ThighLength
global TibiaLength
global FootHeight
x1 = ThighLength;
y1 = TibiaLength;
x = 0;
y = 0;
z = 0;

Tinv = rebuildHomogeneous(px,py,pz,rx,ry,rz);
temp = Tinv;
R = Rofl(pi/4,0,0);
base = eye(4,4);
base(2,4) = HipOffsetY;
base(3,4) = -HipOffsetZ;

temp = base^-1*temp;
Tend1 = eye(4,4);
Tend1(3,4) = -FootHeight;
temp = temp*Tend1^-1;
temp = R*temp;
temp = inv(temp);
rotZ = atan2(temp(2,1),temp(1,1));
rotY = atan2(-temp(3,1),sqrt(temp(3,2)^2 + temp(3,3)^2));
rotX = atan2(temp(3,2),temp(3,3));
px = temp(1,4);
py = temp(2,4);
pz = temp(3,4);
rx = rotX;
ry = rotY;
rz = rotZ;

Tinv = rebuildHomogeneous(px,py,pz,rx,ry,rz);
Ta2 = Tinv^-1
%% find theta4 first with trigonometry

%t1 t2 t3 t4 t5 and t6 are only for display reasons
apostash = sqrt((x-px)^2 + (y - py)^2 + (z-pz)^2);
theta4 = pi - acos((x1^2+y1^2-apostash^2)/(2*x1*y1));
theta4 = real(theta4); %%gia na petaksoume eksw thn periptwsh na einai 0 opou vgazei kai ena mikro migadiko
t4 = theta4

if(pz~=0)
	theta6 = atan(py/pz)
else
	theta6 = 0
end
temp2 = Ta2;
T6 = T(0,-pi/2,0,theta6);
R2 = Rofl2(0,-pi/2,pi);
temp = T6*R2;
temp2 = temp2*temp^-1;
temp2 = temp2^-1
if(TibiaLength+ThighLength*cos(theta4) == 0)
	theta5(1) = asin(temp2(1,4)/(-ThighLength*sin(theta4)));
else
	up = temp2(2,4)*(TibiaLength+ThighLength*cos(theta4)) + ThighLength*temp2(1,4)*sin(theta4);
	up = -up;
	down = (ThighLength^2)*(sin(theta4)^2) + (TibiaLength+ThighLength*cos(theta4))^2;
	theta5(1) = asin(up/down);
end

theta5(2) = pi - theta5(1);
if(theta5(1) < -1.1895 || theta5(1) > 0.9227)
	theta5(1) = -10;
end
if(theta5(2) < -1.1895 || theta5(2) > 0.9227)
	theta5(2) = -10;
end
theta5
for i=1:2
	if(theta5(i) ~= -10)
		
		
		T4 = T(-ThighLength,0,0,theta4);
		T5 = T(-TibiaLength,0,0,theta5(i));
		T6 = T(0,-pi/2,0,theta6);
		R = Rofl2(0,-pi/2,pi);
		temp = T4*T5*T6*R;
		Ta = Ta2;
		Ta = Ta*temp^-1;
		
		tempTheta2 = acos(Ta(2,3));% - pi/4
		theta2(1) = tempTheta2 - pi/4;
		theta2(2) = -tempTheta2 - pi/4;
		for k = 1:2
			if(theta2(k) < -0.3794 || theta2(k) > 0.7905)
				theta2(k) = -10;
			else
				t2 = theta2(k)
			end
		end
		for k=1:2
			if(theta2(k) ~=-10)
				tempTheta3 = asin(Ta(2,2)/sin(theta2(k)+pi/4));
				tempTheta1 = asin(-Ta(3,3)/sin(theta2(k)+pi/4));% + pi/2
				theta1(2*k-1) = tempTheta1 + pi/2;
				theta1(2*k) = pi - tempTheta1 + pi/2;
				theta3(2*k-1) = tempTheta3;
				theta3(2*k) = pi - tempTheta3;
			else
				theta1(2*k-1) = -10;
				theta1(2*k) = -10;
				theta3(2*k-1) = -10;
				theta3(2*k) = -10;
			end
		end
		for k = 1:4
			if(theta1(k) < -1.1452 || theta1(k) > 0.7407)
				theta1(k) = -10;
			else
				t1 = theta1(k)
			end
			if(theta3(k) < -1.7738 || theta3(k) > 0.4840)
				theta3(k) = -10;
			else
				t3 = theta3(k)
			end
		end
	end
end
%
% temp = T4*T5*T6*R;
% Ta = Ta*temp^-1;
% atan2(-Ta(1,2),Ta(2,2))
% theta2 = acos(Ta(2,3)) - pi/4
%
% theta3 = asin(Ta(2,2)/sin(theta2+pi/4))
%
% theta1 = asin(-Ta(3,3)/sin(theta2+pi/4)) + pi/2
end

function [left] = fLeftLeg(thetas)
global HipOffsetZ
global HipOffsetY
global ThighLength
global TibiaLength
global FootHeight
%% left leg
base = eye(4,4);
base(2,4) = HipOffsetY;
base(3,4) = -HipOffsetZ;

T1 = T(0,-3*pi/4,0,thetas(1)-pi/2);
T2 = T(0,-pi/2,0,thetas(2)+pi/4);
T3 = T(0,pi/2,0,thetas(3));
T4 = T(-ThighLength,0,0,thetas(4));
T5 = T(-TibiaLength,0,0,thetas(5));
T6 = T(0,-pi/2,0,thetas(6));
R = Rofl2(0,-pi/2,pi);
Tend1 = eye(4,4);
Tend1(3,4) = -FootHeight;
Tend = R;

Tendend = base*T1*T2*T3*T4*T5*T6*Tend*Tend1;
%Rofl(pi/4,0,0)*T1*T2*T3*T4*T5*T6*Tend
rotZ = atan2(Tendend(2,1),Tendend(1,1));
rotY = atan2(-Tendend(3,1),sqrt(Tendend(3,2)^2 + Tendend(3,3)^2));
rotX = atan2(Tendend(3,2),Tendend(3,3));

left = Tendend;%[Tendend(1:3,4);rotZ;rotY;rotX];
str = sprintf('%f,%f,%f,%f,%f,%f',Tendend(1,4),Tendend(2,4),Tendend(3,4),rotX,rotY,rotZ);
disp(str);
inverseLeft(Tendend(1,4),Tendend(2,4),Tendend(3,4),rotX,rotY,rotZ);

% R = Rofl(pi/4,0,0);
% Tendend= base^-1*Tendend;
% Tendend = Tendend*Tend1^-1
% Tendend = R*Tendend;
% Tendend = inv(Tendend);
%
% rotZ = atan2(Tendend(2,1),Tendend(1,1));
% rotY = atan2(-Tendend(3,1),sqrt(Tendend(3,2)^2 + Tendend(3,3)^2));
% rotX = atan2(Tendend(3,2),Tendend(3,3));

end

function [Taf] = T(a,alpha,d,theta)
Taf = [cos(theta),            -sin(theta),            0,              a;
	sin(theta)*cos(alpha), cos(theta)*cos(alpha),  -sin(alpha),    -sin(alpha)*d;
	sin(theta)*sin(alpha), cos(theta)*sin(alpha),  cos(alpha),     cos(alpha)*d;
	0,                     0,                      0,              1;];
end

function [Taf] = rebuildHomogeneous(Px,Py,Pz,Rx,Ry,Rz)
Taf = [ cos(Rz)*cos(Ry),	cos(Rz)*sin(Ry)*sin(Rx)-sin(Rz)*cos(Rx),	cos(Rz)*sin(Ry)*cos(Rx)+sin(Rz)*sin(Rx),	Px;
	sin(Rz)*cos(Ry),	sin(Rz)*sin(Ry)*sin(Rx)+cos(Rz)*cos(Rx),	sin(Rz)*sin(Ry)*cos(Rx)-cos(Rz)*sin(Rx),	Py;
	-sin(Ry),			cos(Ry)*sin(Rx),							cos(Ry)*cos(Rx),							Pz;
	0,					0,											0,											1;];
end

function [Tranos] = Tran(x,y,z)
Tranos = eye(4,4);
Tranos(1,4) = x;
Tranos(2,4) = y;
Tranos(3,4) = z;
end

function [Ro] = Rofl(xAngle,yAngle,zAngle)
Rx = [1,                0,          0;
	0,                cos(xAngle), -sin(xAngle);
	0,                sin(xAngle), cos(xAngle);];

Ry = [cos(yAngle),       0,          sin(yAngle);
	0,                1,          0;
	-sin(yAngle),      0,          cos(yAngle);];

Rz = [cos(zAngle),       -sin(zAngle),0;
	sin(zAngle)        cos(zAngle), 0;
	0,                0,          1;];
R = Rx*Ry*Rz;

R = [R, [0;0;0];
	[0,0,0],1];
Ro = R;
end
function [Ro] = Rofl2(xAngle,yAngle,zAngle)
Rx = [1,                0,          0;
	0,                cos(xAngle), -sin(xAngle);
	0,                sin(xAngle), cos(xAngle);];

Ry = [cos(yAngle),       0,          sin(yAngle);
	0,                1,          0;
	-sin(yAngle),      0,          cos(yAngle);];

Rz = [cos(zAngle),       -sin(zAngle),0;
	sin(zAngle)        cos(zAngle), 0;
	0,                0,          1;];
R = Rz*Ry*Rx;

R = [R, [0;0;0];
	[0,0,0],1];
Ro = R;
end
