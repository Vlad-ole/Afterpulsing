function ScopeMath_TwoBeams()

% GUI variables
hFigure = [];
hAxesRaw = [];
hAxesMath = [];

% Instrument control variables
interfaceObj     = []; 
deviceObj        = [];    
channel1Obj      = [];   
waveformObj      = [];
samplingInterval = [];

% final data sets
xData1           = [];
yData1           = [];

%internal flags
nSaved           = 0;

dir_name = 'D:/Data_work/tektronix_signal/KETEK PM1125NS-SB0/275K/30_76V/raw/binary/run_';
%dir_name = 'D:/Data_work/tektronix_signal/KETEK PM1125NS-SB0/275K/run_';

makeGUI(); 
connectToInstrument();

for c = 1:1000
    c_string = int2str(c);
    file_out_0 = strcat(dir_name, c_string);
    file_out = strcat(file_out_0, '.bin');
    
    getDataFromInstrument();
    fprintf('%i\n', c)
    fprintf(file_out)
end

              
  %%---------------------------------------------------   
   function connectToInstrument
      try
         % THE FOLLOWING TWO LINES SHOULD BE CHANGED TO SUIT YOUR HARDWARE
         interfaceObj = visa('tek', 'TCPIP::192.168.255.5::INSTR'); 
         
         % Before you can perform a read or write operation, obj must be connected to the instrument with the fopen function.
         fopen(interfaceObj);
         % read/write current settings from a VISA object.
         settings1 = query(interfaceObj, 'HORizontal?');

         % Sets the time per division
         fprintf(interfaceObj, 'HORizontal:SCAle 100E-9');

         % Sets number or record points         
         fprintf(interfaceObj, 'HORizontal:RECordlength 5000000'); %5M is max!!!!
         
         % Get sampling pitch
         samplingInterval = str2double( query(interfaceObj, 'WFMPre:XINcr?'));
         
         fclose(interfaceObj);
         
         deviceObj = icdevice('DPO4104.mdd', interfaceObj);
         connect(deviceObj);
         channel1Obj = deviceObj.Channel(1); % read from channel 1
         waveformObj = deviceObj.Waveform(1);  % default waveform measurement
      catch         
         cleanupObjects();
         rethrow(lasterror);
      end
   end

  %%---------------------------------------------------
   function getDataFromInstrument(hObject, eventdata)

       if ~(strcmp(deviceObj.Status, 'open') && strcmp(channel1Obj.State, 'on'))
            cleanupObjects();
            error('Not connected to device, or channel is disabled on the scope.');
       end

       [yData1, xData1, yUnits1, xUnits1] = invoke(waveformObj, 'readwaveform', channel1Obj.name);

       %----------------------------------------------------------
       
           %yOut0 = [xData1' yData1']; '-mat' '-ascii'
                    
           yOut0 = yData1';
           
%             [m,n] = size(yOut0);
%             yOut0 = [m;yOut0];
           
            fileID = fopen(file_out,'w');
            fwrite(fileID, yOut0,'double');
            fclose(fileID);         
           
          % save(file_out,'yOut0','-mat');
          
       

         %----------------------------------------------------------
         % check the user closed the window while we were waiting
         % for the instrument to return the waveform data
         if ishandle(hFigure),
            cla(hAxesRaw);
            axes(hAxesRaw);
            hold on;
            plot(xData1, yData1,  char('-b'));
            hold off;
            xlabel(xUnits1); ylabel(yUnits1);
            %
            axes(hAxesMath);

         end
     
   end

  %%---------------------------------------------------   
   function makeGUI
      hFigure = figure('deleteFcn', @figureCloseCallback);
      hAxesRaw  = axes('position', [0.05  0.55  0.45 0.40]);
      title('Raw Data. Beam1');
      hAxesMath = axes('position', [0.05  0.05  0.45 0.40]);      
      title('Raw Data. Beam2');
     
   end

  %%---------------------------------------------------   
   function figureCloseCallback(hObject, eventdata)
      cleanupObjects();
   end

  %%---------------------------------------------------   
   function cleanupObjects()
      
      try
         if ~isnumeric(deviceObj) && isvalid(deviceObj)
            disconnect(deviceObj);
            delete(deviceObj);
         end
      catch
         delete(deviceObj); 
      end

      if ~isnumeric(deviceObj) && isvalid(interfaceObj)
         fclose(interfaceObj);
         delete(interfaceObj);
      end

      if ishandle(hFigure), 
         delete(hFigure); 
      end
   end

end % of ScopeMath_Simple

