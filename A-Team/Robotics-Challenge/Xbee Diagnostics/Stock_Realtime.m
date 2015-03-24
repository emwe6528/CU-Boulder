%%real time data plot from a serial port 
% This matlab script is for ploting a graph by accessing serial port data in
% real time. Change the com values and all variable values accroding to
% your requirements. Dont forget to add terminator in to your serial device program.
% This script can be modified to be used on any platform by changing the
% serialPort variable. 
% Author: Moidu thavot.

%%Clear all variables

clear all;
%%Variables (Edit yourself)

SerialPort='com7'; %serial port
MaxDeviation = 3;%Maximum Allowable Change from one value to next 
TimeInterval=0.2;%time interval between each input.
loop=70000;%count values
%%Set up the serial port object

s = serial(SerialPort)
fopen(s);



time =now;
voltage = 0;
%% Set up the figure 
figureHandle = figure('NumberTitle','off',...
    'Name','Voltage Characteristics',...
    'Color',[0 0 0],'Visible','off');

% Set axes
axesHandle = axes('Parent',figureHandle,...
    'YGrid','on',...
    'YColor',[0.9725 0.9725 0.9725],...
    'XGrid','on',...
    'XColor',[0.9725 0.9725 0.9725],...
    'Color',[0 0 0]);

hold on;

plotHandle = plot(axesHandle,time,voltage,'Marker','.','LineWidth',1,'Color',[0 1 0]);

xlim(axesHandle,[max(time)-3 max(time+0.001)]);

% Create xlabel
xlabel('Time','FontWeight','bold','FontSize',14,'Color',[1 1 0]);

% Create ylabel
ylabel('Voltage in V','FontWeight','bold','FontSize',14,'Color',[1 1 0]);

% Create title
title('Real Time Data','FontSize',15,'Color',[1 1 0]);




%% Initializing variables

voltage(1)=0;
time(1)=0;
count = 2;
k=1;
try
while ~isequal(count,loop)
   
    %%Re creating Serial port before timeout
    
    %%Serial data accessing 
    waste(count) = fscanf(s,'%f');
     voltage(count) = fscanf(s,'%f');
     
     %%For reducing Error Use your own costant
     
     
     time(count) = count;
    set(plotHandle,'YData',voltage,'XData',time);
    set(figureHandle,'Visible','on');

    xlim(axesHandle,[max(time)-500 max(time+0.001)]);
    
    pause(.0000001);
    
    count = count +1;
end

catch

%% Clean up the serial port
fclose(s);
delete(s);
clear s;
end
fclose(s);
delete(s);
clear s;