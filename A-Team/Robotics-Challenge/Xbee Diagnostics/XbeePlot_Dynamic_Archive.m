
function [RSSI, Heading] = XbeePlot_Dynamic(SerialPort,Samples);

% Time interval between each input.
TimeInterval=0.03;
% Set up the serial port object
s = serial(SerialPort)
fopen(s);

% Initial Variables
error = 0;
RSSI = 0;
Heading = 0;
%% Set up the figure 
figureHandle = figure('NumberTitle','off',...
    'Name','RSSI Levels (Currently Gathering Data)',...
    'Color',[0 0 0],'Visible','off');

% Set axes
axesHandle = axes('Parent',figureHandle,...
    'YGrid','on',...
    'YColor',[0.9725 0.9725 0.9725],...
    'XGrid','on',...
    'XColor',[0.9725 0.9725 0.9725],...
    'Color',[.1 .1 .1]);

hold on;

plotHandle = plot(axesHandle,RSSI,Heading, 'r.');
plotHandle2 = plot(axesHandle,RSSI,Heading, 'bo');

xlim(axesHandle,[min(0) max(180)]);

% Create xlabel
xlabel('Heading','FontWeight','bold','FontSize',14,'Color',[.8 .8 .8]);

% Create ylabel
ylabel('RSSI','FontWeight','bold','FontSize',14,'Color',[.8 .8 .8]);

% Create title
title('XBee Serial Output','FontSize',15,'Color',[1 1 1]);




%% Reads values from Xbee and plots them 
% Two values are read from serial. First is the Heading and second the RSSI
% These values are stored on arrays. Each Heading/RSSI pair that is
% received is plotted as a point before another pair is received.

count = 1;

% Everything within 'try' will execute as long as there are no errors
try
    
% Will execute Samples amount of times.

while ~isequal(count,Samples+1)
    %%Serial data accessing 
    Hindex = fscanf(s,'%f');
    Heading(Hindex+1) = Hindex;
    RSSI(Hindex+1) = fscanf(s,'%f');
    Heading2(Hindex+1) = Hindex;
    RSSI2(Hindex+1) = fscanf(s,'%f');
        
    %%data pair is plotted
    set(plotHandle,'YData',RSSI,'XData',Heading);
        set(figureHandle,'Visible','on');
            set(plotHandle2,'YData',RSSI2,'XData',Heading);
        set(figureHandle,'Visible','on');
        
%     set(plot(axesHandle,RSSI2,Heading2+180, 'b.'),'YData',RSSI2,'XData',Heading2+180);     
%     set(figureHandle,'Visible','on');
    %%small pause to allow next transmission
    pause(TimeInterval);
    count = count +1;
end

%% Basic Analysis of data

% If there was no error above, change the label to Finished
if (error == 0) 
    set(figureHandle, 'Name', 'RSSI Levels (Finished)');
end

% Finds the Heading that corresponds to the Maximum RSSI value
[Y, I] = max(RSSI);
fprintf('\nMaximum RSSI and Heading: %d at %d degrees\n', max(RSSI), Heading(I))
    



%% Clean up the serial port

% If there was an error above, it will skip down to here.
catch ME
    error = 1;
    errorMessage = sprintf('Error in function XBee_RealTime().\n\nError Message: %s', ME.message);
	fprintf(1,'%s', errorMessage);
    fprintf('\nSerial communication with Serial Port %s has been closed\n', SerialPort)
end
% Close the Serial Port
fclose(s);
delete(s);
clear s;

end