
function [Data, X] = Dynamic_Realtime(SerialPort,Samples,style);

% Set up the serial port object
s = serial(SerialPort)
mode = 0;
magnitude = 2;

TimeInterval = .03;

fopen(s);

% Initial Variables
error = 0;
Data = 0;
X = 0;
style = 180;
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

plotHandle = zeros(1,magnitude);
Key = zeros(3,3);

for i=1:magnitude
    Key(i,:) = [rand rand rand];
    plotHandle(i) = plot(axesHandle,X,Data,'.-','Color',Key(i,:));
end



    xlim(axesHandle,[min(0) max(style)]);





% Create xlabel
xlabel('X','FontWeight','bold','FontSize',14,'Color',[.8 .8 .8]);

% Create ylabel
ylabel('Data','FontWeight','bold','FontSize',14,'Color',[.8 .8 .8]);

% Create title
title('Serial Output','FontSize',15,'Color',[1 1 1]);

legend('raw','filtered')



%% Reads values from Xbee and plots them 
% Two values are read from serial. First is the Heading and second the RSSI
% These values are stored on arrays. Each Heading/RSSI pair that is
% received is plotted as a point before another pair is received.

count = 1;

% Everything within 'try' will execute as long as there are no errors
try
    
% Will execute Samples amount of times.


while (1==1)
    %%Serial data accessing 
    
    for i=1:magnitude
        index = fscanf(s,'%f');
        if (mode~=0)
           
            X(index+1,i) = index;
        end
        if (mode==0)
             X(count+1,i) = count;
        end
        
            Data(count+1,i) = fscanf(s,'%f');
        set(plotHandle(i),'YData',Data(:,i),'XData',X(:,i));
        set(figureHandle,'Visible','on');

    end
  if (mode == 0)
       xlim(axesHandle,[count-100 count+.01]);
  end
    
    pause(.001);
    count = count +1;
end

%% Basic Analysis of data

% If there was no error above, change the label to Finished
if (error == 0) 
    set(figureHandle, 'Name', 'RSSI Levels (Finished)');
end

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