%CPE-310 Mike Stepanik FINAL EXAM
clear all
clc
m=10^-3
%% SETUP
port = "COM3"
baud=250000;
conn = serialport(port,baud);
%% READ 20s worth of data
nSamples=3*2000; 
samples= read(conn, nSamples-1, "string")
%% Convert Samples to Voltages
vals_as_text=split(samples, ',')
dec_vals=hex2dec(vals_as_text)-127
ticksPS=dec_vals*100
revolutionsPS=(ticksPS/1400)
RPM=revolutionsPS*60;

RevolutionsPerMS=dec_vals/1400;
%Revolutions=sum(RevoltionsPerMS);


%Revolutions=dec_vals/14
%change=diff(dec_vals)

%revolutionsPS = revolutions(1:end-1);


figure
t=10:10:20000;
plot(t./1000,revolutionsPS)
title("RevoltionsPerSeconed");
xlabel("Seconds");
ylabel("RPS");
ylim([-0.5,3])



figure
t=10:10:20000;
plot(t./1000,RPM)
title("RPM");
xlabel("Seconds");
ylabel("RPM");
ylim([-20,180])




position = zeros(1,2000)
for i=1:2000
    if(i > 1)
        position(i) = position(i-1) + RevolutionsPerMS(i);
    else 
        position(i) = RevolutionsPerMS(i);
    end
    
end


figure 
t=10:10:20000;
plot(t./1000,position)
title("Position vs. Time")
xlabel("Seconds");
ylabel("Revoltions Traveled");


angularpos=position.*360

figure 
t=10:10:20000;
plot(t./1000,angularpos)
title("angular position vs. Time")
xlabel("Seconds");
ylabel("Degrees");

