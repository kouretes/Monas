function [] = inverseLeftHand()
close all;
%clear all;
clc;
%% Nao kinematics for the left arm

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

thetasL(1) = 0;
thetasL(2) = 0;
thetasL(3) = 0;
thetasL(4) = 0;

fLeftHand(thetasL)


end

function [iLeft] = inverseLeft(px,py,pz,rx,ry,rz)
global shoulderOffsetY
global shoulderOffsetZ
global elbowOffsetY
global LowerArmLength
global HandOffsetX
global upperArmLength
x1 = upperArmLength;
y1 = LowerArmLength + HandOffsetX;
x = 0;
y = shoulderOffsetY + elbowOffsetY;
z = shoulderOffsetZ;

%% find theta4 first with trigonometry
apostash = sqrt((x-px)^2 + (y - py)^2 + (z-pz)^2);
theta4 = pi - acos((x1^2+y1^2-apostash^2)/(2*x1*y1));
theta4 = -real(theta4) % Einai panta arnhtikh gt milame gia to aristero xeri
l4 = sin(theta4);
l5 = cos(theta4);

%% Next we will find theta2
up = py-113-((sin(rz)*sin(rx)*sin(ry)+cos(rz)*cos(rx))*113.7*l4)/l5;
down = 105+113.7*l5+113.7*(l4^2)/l5;
updown = up/down;
if(real(acos(updown)) + pi/2 <= 1.3265)
	theta21 = real(acos(updown)) +pi/2
else
	theta21 = -10;
end
if(-real(acos(updown)) + pi/2 >= - 0.3142)
	theta22 = -real(acos(updown)) +pi/2
else
	theta22 = -10;
end

%% Next we will find theta3
up = sin(rz)*sin(ry)*cos(rx) - cos(rz)*sin(rx);
if(theta21 ~= -10)
	down = sin(theta21-pi/2);
	updown = up/down;
	theta3 = asin(updown);
	theta311 = theta3
	if(pi-theta3 <= 2.0875 && pi - theta3 >= -2.0875)
		theta312 = real(pi - theta3)
	else
		theta312 = -10;
	end
else
	theta311 = -10;
	theta312 = -10;
end
if(theta22 ~= -10)
	down = sin(theta22-pi/2);
	updown = up/down;
	theta3 = asin(updown);
	theta321 = theta3
	if(pi-theta3 <= 2.0875 && pi - theta3 >= -2.0875)
		theta322 = real(pi - theta3)
	else
		theta322 = -10;
	end
else
	theta321 = -10;
	theta322 = -10;
end
%% Next we will find theta1
eq3 = cos(rz)*sin(ry)*cos(rx) + sin(rz)*sin(rx);
eq4 = cos(ry)*cos(rx);%cos(rz)*sin(ry)*sin(rx) - sin(rz)*cos(rx);
if(theta311 ~= -10)
	%up = -(eq4 - l5*eq3*sin(theta311)/cos(theta311));
	if(theta21 ~= -10)
        up = eq4 + eq3*sin(theta311)*cos(theta21-pi/2)/cos(theta311);
        down = cos(theta311) + (cos(theta21-pi/2)^2)*(sin(theta311)^2)/cos(theta311);
		%down = ((l5*cos(theta21-pi/2)*sin(theta311)^2)/cos(theta311) + cos(theta21-pi/2)*cos(theta311)*l5 - l4*sin(theta21-pi/2));
		if(theta311 ~= pi/2 && theta311 ~= -pi/2)
			theta1 = acos(up/down);
			theta1111 = theta1;
			theta1112 = - theta1;
		else
			theta1 = acos(eq3/(cos(theta21-pi/2)*sin(theta311)));
			theta1111 = theta1;
			theta1112 = - theta1;
		end
		if(theta1 <= 2.0857)
			theta1111
			theta1112
		end
	end
	if(theta22 ~= -10)
        up = eq4 + eq3*sin(theta311)*cos(theta22-pi/2)/cos(theta311);
        down = cos(theta311) + (cos(theta22-pi/2)^2)*(sin(theta311)^2)/cos(theta311);
		%down = ((l5*cos(theta22-pi/2)*sin(theta311)^2)/cos(theta311) + cos(theta22-pi/2)*cos(theta311)*l5 - l4*sin(theta22-pi/2));
		if(theta311 ~= pi/2 && theta311 ~= -pi/2)
			theta1 = acos(up/down);
			theta1211 = theta1;
			theta1212 = - theta1;
		else
			theta1 = acos(eq3/(cos(theta22-pi/2)*sin(theta311)));
			theta1211 = theta1;
			theta1212 = - theta1;
		end
		if(theta1 <= 2.0857)
			theta1211
			theta1212
		end
	end
end
if(theta312 ~= -10)
	%up = -(eq4 - l5*eq3*sin(theta312)/cos(theta312));
	if(theta21 ~= -10)
		up = eq4 + eq3*sin(theta312)*cos(theta21-pi/2)/cos(theta312);
        down = cos(theta312) + (cos(theta21-pi/2)^2)*(sin(theta312)^2)/cos(theta312);
        %down = ((l5*cos(theta21-pi/2)*sin(theta312)^2)/cos(theta311) + cos(theta21-pi/2)*cos(theta311)*l5 - l4*sin(theta21-pi/2));
		if(theta311 ~= pi/2 && theta312 ~= -pi/2)
			theta1 = acos(up/down);
			theta1121 = theta1;
			theta1122 = - theta1;
		else
			theta1 = acos(eq3/(cos(theta21-pi/2)*sin(theta312)));
			theta1121 = theta1;
			theta1122 = - theta1;
		end
		if(theta1 <= 2.0857)
			theta1121
			theta1122
		end
	end
	if(theta22 ~= -10)
		up = eq4 + eq3*sin(theta312)*cos(theta22-pi/2)/cos(theta312);
        down = cos(theta312) + (cos(theta22-pi/2)^2)*(sin(theta312)^2)/cos(theta312);
		%down = ((l5*cos(theta22-pi/2)*sin(theta312)^2)/cos(theta312) + cos(theta22-pi/2)*cos(theta312)*l5 - l4*sin(theta22-pi/2));
		if(theta312 ~= pi/2 && theta312 ~= -pi/2)
			theta1 = acos(up/down);
			theta1221 = theta1;
			theta1222 = - theta1;
		else
			theta1 = acos(eq3/(cos(theta22-pi/2)*sin(theta312)));
			theta1221 = theta1;
			theta1222 = - theta1;
		end
		if(theta1 <= 2.0857)
			theta1221
			theta1222
		end
	end
end
if(theta321 ~= -10)
	%up = -(eq4 - l5*eq3*sin(theta321)/cos(theta321));
	if(theta21 ~= -10)
		
		up = eq4 + eq3*sin(theta321)*cos(theta21-pi/2)/cos(theta321);
        down = cos(theta321) + (cos(theta21-pi/2)^2)*(sin(theta321)^2)/cos(theta321);
        %down = ((l5*cos(theta21-pi/2)*sin(theta321)^2)/cos(theta321) + cos(theta21-pi/2)*cos(theta321)*l5 - l4*sin(theta21-pi/2));
		if(theta321 ~= pi/2 && theta321 ~= -pi/2)
			theta1 = acos(up/down);
			theta1131 = theta1;
			theta1132 = - theta1;
		else
			theta1 = acos(eq3/(cos(theta21-pi/2)*sin(theta321)));
			theta1131 = theta1;
			theta1132 = - theta1;
		end
		if(theta1 <= 2.0857)
			theta1131
			theta1132
		end
	end
	if(theta22 ~= -10)
		
		up = eq4 + eq3*sin(theta321)*cos(theta22-pi/2)/cos(theta321);
        down = cos(theta321) + (cos(theta22-pi/2)^2)*(sin(theta321)^2)/cos(theta321);
		%down = ((l5*cos(theta22-pi/2)*sin(theta321)^2)/cos(theta321) + cos(theta22-pi/2)*cos(theta321)*l5 - l4*sin(theta22-pi/2));
		if(theta321 ~= pi/2 && theta321 ~= -pi/2)
			theta1 = acos(up/down);
			theta1231 = theta1;
			theta1232 = - theta1;
		else
			theta1 = acos(eq3/(cos(theta22-pi/2)*sin(theta321)));
			theta1231 = theta1;
			theta1232 = - theta1;
		end
		if(theta1 <= 2.0857)
			theta1231
			theta1232
		end
	end
end
if(theta322 ~= -10 && theta322 ~= theta321)
	%up = -(eq4 - l5*eq3*sin(theta322)/cos(theta322));
	if(theta21 ~= -10)
		up = eq4 + eq3*sin(theta322)*cos(theta21-pi/2)/cos(theta322);
        down = cos(theta322) + (cos(theta21-pi/2)^2)*(sin(theta322)^2)/cos(theta322);
		%down = ((l5*cos(theta21-pi/2)*sin(theta322)^2)/cos(theta322) + cos(theta21-pi/2)*cos(theta322)*l5 - l4*sin(theta21-pi/2));
		if(theta322 ~= pi/2 && theta322 ~= -pi/2)
			theta1 = acos(up/down);
			theta1131 = theta1;
			theta1132 = - theta1;
		else
			theta1 = acos(eq3/(cos(theta21-pi/2)*sin(theta322)));
			theta1131 = theta1;
			theta1132 = - theta1;
		end
		if(theta1 <= 2.0857)
			theta1131
			theta1132
		end
	end
	if(theta22 ~= -10)
		
		up = eq4 + eq3*sin(theta322)*cos(theta22-pi/2)/cos(theta322);
        down = cos(theta322) + (cos(theta22-pi/2)^2)*(sin(theta322)^2)/cos(theta322);
		%down = ((l5*cos(theta22-pi/2)*sin(theta322)^2)/cos(theta322) + cos(theta22-pi/2)*cos(theta322)*l5 + l4*sin(theta22-pi/2));
		if(theta322 ~= pi/2 && theta322 ~= -pi/2)
			theta1 = acos(up/down);
			theta1241 = theta1;
			theta1242 = - theta1;
		else
			theta1 = acos(eq3/(cos(theta22-pi/2)*sin(theta322)));
			theta1241 = theta1;
			theta1242 = - theta1;
		end
		if(theta1 <= 2.0857)
			theta1241
			theta1242
		end
	end
end
%theta1 = asin(eq3)
end

function [left] = fLeftHand(thetas)
global shoulderOffsetY
global shoulderOffsetZ
global elbowOffsetY
global LowerArmLength
global HandOffsetX
global upperArmLength
%% left hand
base = eye(4,4);
base(2,4) = shoulderOffsetY + elbowOffsetY;
base(3,4) = shoulderOffsetZ;

T1 = T(0,-pi/2,0,thetas(1));
T2 = T(0,pi/2,0,thetas(2)-pi/2); %To -pi/2 to afinoume panta !!!
T3 = T(0,-pi/2,upperArmLength,thetas(3));
T4 = T(0,pi/2,0,thetas(4));

R = Rofl(0,0,pi/2);

Tend1 = eye(4,4);
Tend1(1,4) = HandOffsetX+LowerArmLength;
Tend = R*Tend1;
Tendend = (base*T1*T2*T3*T4*Tend)
rotZ = atan2(Tendend(2,1),Tendend(1,1));
rotY = atan2(-Tendend(3,1),sqrt(Tendend(3,2)^2 + Tendend(3,3)^2));
rotX = atan2(Tendend(3,2),Tendend(3,3));
inverseLeft(Tendend(1,4),Tendend(2,4),Tendend(3,4),rotX,rotY,rotZ)
%inverseLeft(154.385,180.953,-39.2027,0,0.680053,0.915264)

left = [Tendend(1:3,4);rotZ;rotY;rotX];
end

function [Taf] = T(a,alpha,d,theta)
Taf = [cos(theta),            -sin(theta),            0,              a;
	sin(theta)*cos(alpha), cos(theta)*cos(alpha),  -sin(alpha),    -sin(alpha)*d;
	sin(theta)*sin(alpha), cos(theta)*sin(alpha),  cos(alpha),     cos(alpha)*d;
	0,                     0,                      0,              1;];
end

function [Taf] = T222(cth,sth,a,alpha,d)
Taf = [cth,            -sth,            0,              a;
	sth*cos(alpha), cth*cos(alpha),  -sin(alpha),    -sin(alpha)*d;
	sth*sin(alpha), cth*sin(alpha),  cos(alpha),     cos(alpha)*d;
	0,                     0,                      0,              1;];
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
