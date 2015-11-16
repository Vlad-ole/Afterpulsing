function ScopeMath_TwoBeams()

% GUI variables
hFigure = [];
hAxesRaw = [];
hAxesMath = [];
hAxesHist = [];
hStartButton = [];
hSaveButton = [];
acquiringData = false;

% Instrument control variables
interfaceObj     = []; 
deviceObj        = [];    
channel1Obj      = [];   
channel2Obj      = [];   
waveformObj      = [];
samplingInterval = [];

% final data sets
xData1           = [];
yData1           = [];
xData2           = [];
yData2           = [];
yAVG             = [];
nAVG             = 1;

SX               = [];
SY               = [];
SXdif            = [];
SYdif            = [];

SXs              = [];
SYs              = [];

SXc              = [];
SYc              = [];

%signal convolution with real data
SXd              = [];
SYd              = [];


%internal flags
nSaved           = 0;

%histogramms
histData         = [];      %raw histogramm data
histIntegrationT = 1000E-9;  %for measure pulse shape we changed it to 1us from 200ns

% set up a timer to periodically get the data 
% from the instrument
timerObj = timer('Period', 0.01, 'ExecutionMode', 'fixedSpacing', ...
                  'timerFcn', @getDataFromInstrument);
               
makeGUI(); 
connectToInstrument();

              
  %%---------------------------------------------------   
   function connectToInstrument
      try
         % THE FOLLOWING TWO LINES SHOULD BE CHANGED TO SUIT YOUR HARDWARE
         interfaceObj = visa('tek', 'USB0::0x0699::0x0405::C025165::0::INSTR'); 
         
         % Create a VISA-TCPIP object.
        %interfaceObj = instrfind('Type', 'visa-tcpip', 'RsrcName', 'TCPIP0::192.168.255.5::inst0::INSTR', 'Tag', '');
         
         % Create the VISA-TCPIP object if it does not exist
         % otherwise use the object that was found.
        %if isempty(interfaceObj)
        %    interfaceObj = visa('TEK', 'TCPIP0::192.168.255.5::inst0::INSTR');
        %else
        %    fclose(interfaceObj);
        %    interfaceObj = interfaceObj(1);
        %end
         
         % Before you can perform a read or write operation, obj must be connected to the instrument with the fopen function.
         fopen(interfaceObj);
         % read/write current settings from a VISA object.
         settings1 = query(interfaceObj, 'HORizontal?');

         % Sets the time per division
         fprintf(interfaceObj, 'HORizontal:SCAle 100E-9');
         %fprintf(interfaceObj, 'HORizontal:POSition 1E-6');
           % Sets number or record points         
         fprintf(interfaceObj, 'HORizontal:RECordlength 5000000'); %5M is max!!!!
         
         % Get sampling pitch
         samplingInterval = str2double( query(interfaceObj, 'WFMPre:XINcr?'));
         
         fclose(interfaceObj);
         
         deviceObj = icdevice('DPO4104.mdd', interfaceObj);
         connect(deviceObj);
         channel1Obj = deviceObj.Channel(1); % read from channel 1
         waveformObj = deviceObj.Waveform(1);  % default waveform measurement
         channel2Obj = deviceObj.Channel(4); % read from channel 1
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
   vThreshold = -2.0E-4;

   %----------------------------------------------------------
   if nSaved==0
       yOut0 = [xData1' yData1'];
       save('d:/yData1_org.txt','yOut0','-ascii');
       
       %generate SiPM pulse
       %[SY,SX] = SiPM_Pulse(  histIntegrationT, xData1(2)-xData1(1), 2.3E-9, 0.03971, 0.01033, 28.3E-9, 455.E-9, 1);  
       %yOut1 = [SX' SY'];
       [SY,    SX   ] = SiPM_SignalFromFile( 'yAVG.txt',     1 );
       [SYdif, SXdif] = SiPM_SignalFromFile( 'yAVG_Dif.txt', 1 );
       yOut1 = [SX' SY'];
       save('d:/yData1.txt','yOut1','-ascii');
       
       %generate signal 
       %[SYs, SXs] = SiPM_Signal(  10E-6, xData1(2)-xData1(1), 2E-6, 5.E-9, 50.E-9);  
       %yOut2 = [SXs' SYs'];
       %save('d:/yData2.txt','yOut2','-ascii');
       
       %calculate convolution
       %SYc = conv(SY, SYs);
       %SXc = 1:length(SYc);
       %SXc = SXc * (xData1(2)-xData1(1));
       %yOut3 = [SXc' SYc'];
       %save('d:/yData3.txt','yOut3','-ascii');
       
       %calculate convolution with real data
       SYd = conv(SY, yData1);
       SXd = 1:length(SYd);
       SXd = SXd * (xData1(2)-xData1(1));
       yOut4 = [SXd' SYd'];
       save('d:/yData1_Conv.txt','yOut4','-ascii');

       SYd = conv(SYdif, [0 diff(yData1)]);
       yOut4 = [SXd' SYd'];
       save('d:/yData1_ConvDif.txt','yOut4','-ascii');
       
       %calculate number of noise events
       SYd_Th   = SYd<vThreshold;
       Syd_Diff = [diff(SYd_Th)>0 0];
       yOut5 = [SXd' Syd_Diff'];
       save('d:/yData1_Events.txt','yOut5','-ascii');      
       nSaved    = 1;
   end
    
  %process the data
  %calculate convolution with real data
  SYd      = conv(SYdif, diff(yData1));
  SYd_Th   = SYd<vThreshold;
  SYd_ThP  = [diff(SYd_Th)>0 0];
  NNoise   = sum(SYd_ThP);
  histData = [histData; NNoise];
  
  disp(NNoise);

   
     %----------------------------------------------------------
     % check the user closed the window while we were waiting
     % for the instrument to return the waveform data
     if ishandle(hFigure),
        cla(hAxesRaw);
        axes(hAxesRaw);
        hold on;
        plot(xData1, yData1,  char('-b'));
        %plot(xData1, yCR,     char('-g'));
        %plot(xData1, yCR2,    char('-r'));
        %plot(xData1, yAVG,    char('-m'));
        hold off;
        xlabel(xUnits1); ylabel(yUnits1);
        %
        axes(hAxesMath);
        plot(SX,SY);
        %xlabel(xUnits2); ylabel(yUnits2);
        %
        axes(hAxesHist);
        bins = 0:1:200;
        H = hist(histData(:),bins);
        plot(bins, H, 'linewidth',2, 'color', 'r');
        
     end
     
   end
  %%---------------------------------------------------   
  % Return first derivation of given input samples,
  function [y] = first_derivation( x )
       N     = length(x);
       y     = zeros(size(x));
       for i = 2:N-1
           y(i) = x(i+1) - x(i-1);
       end
   end
  %%---------------------------------------------------   
  % Return RC high-pass filter output samples, given input samples,
  % time interval dt, and time constant RC
  function [y] = highpass( x, dt, RC)
       N     = length(x);
       y     = zeros(size(x));
       Alpha = RC / (RC + dt);
       y(1) = x(1);
       
       for i = 2:N
           y(i) = Alpha * (y(i-1) + x(i) - x(i-1));
       end
   end

  %%---------------------------------------------------   
  % Return RC high-pass filter output samples, given input samples,
  % time interval dt, and time constant RC
  function [y] = lowpass( x, dt, RC)
       N     = length(x);
       y     = zeros(size(x));
       Alpha = dt / (RC + dt);
       y(1) = x(1);
       
       for i = 2:N
           y(i) = Alpha * x(i) + (1-Alpha) * y(i-1);
       end
  end

  %%---------------------------------------------------   
  % Generate SiPM responce. 
  % Array length is defined as Time/dt and shape is defined by 2 constants
    function [y, x] =SiPM_Pulse( Time, dt, Trise, A1, A2, Tdec1, Tdec2, flip)
        %N samples
        N  = Time/dt;  
        %calculate shape
        x  = 1:N; 
        x  = (x-1)*dt;
        y  = (1-exp(-x/Trise)).*(A1*exp(-x/Tdec1)+A2*exp(-x/Tdec2));
        y  = y/sum(y);
        
        %flip array if required
        if flip==1
            y = fliplr(y);
        end
    end

  %%---------------------------------------------------   
  % Generate detector responce. 
  % Array length is defined as Time/dt and start position and shape is defined by 2 constants
    function [y, x] =SiPM_Signal( Time, dt, Tstart, Trise, Tdec)
        N  = Time/dt;    %N samples
        %calculate shape
        x  = 1:N; 
        x  = (x-1)*dt;
        y  = zeros(size(x));
        %calculate pulse
        xs   = int32(Tstart/dt);
        y(xs:end) = (1-exp(-(x(xs:end)-Tstart)/Trise)).*exp(-(x(xs:end)-Tstart)/Tdec); 
        y    = y/sum(y);
    end

  %%---------------------------------------------------   
  % Read SiPM pulse shape from file 
    function [y, x] =SiPM_SignalFromFile( fileName, flip )
       %read formatted data 
       [rx, ry] = textread('yAVG.txt');
       %transpone it for compatibility
       y = ry';
       x = rx';
       
       %flip array if required
       if flip==1
           y = fliplr(y);
       end       
    end

  %%---------------------------------------------------   
   function makeGUI
      hFigure = figure('deleteFcn', @figureCloseCallback);
      hAxesRaw  = axes('position', [0.05  0.55  0.45 0.40]);
      title('Raw Data. Beam1');
      hAxesMath = axes('position', [0.05  0.05  0.45 0.40]);      
      title('Raw Data. Beam2');
      hAxesHist = axes('position', [0.55  0.05  0.45 0.40]);      
      title('Raw Data. Histogramm');
      
      hStartButton = uicontrol('Style', 'PushButton', ...
                               'String', 'Start Acquisition', ...
                               'units', 'normalized', ...
                               'callback', @startStopCallback, ...
                               'position', [0.70 0.84 0.10 0.06]);
      set(hStartButton, 'callback', @startStopCallback);

      hSaveButton = uicontrol('Style', 'PushButton', ...
                               'String', 'Save data', ...
                               'units', 'normalized', ...
                               'callback', @saveCallback, ...
                               'position', [0.80 0.84 0.10 0.06]);
      set(hSaveButton, 'callback', @saveCallback);
      
      
   end

  %%---------------------------------------------------   
   function startStopCallback(hObject, eventdata)
      if acquiringData
         if strcmp(timerObj.running, 'on')
            stop(timerObj);
         end
         acquiringData = false;
         set(hObject, 'string', 'Start Acquisition');
      else     
         acquiringData = true;
         set(hObject, 'string', 'Stop Acquisition');
         if strcmp(timerObj.running, 'off')
             start(timerObj);
         end
      end         
   end
  %%---------------------------------------------------   
   function saveCallback(hObject, eventdata)
      if acquiringData
      else     
          %yOut1 = [xData1' yData1'];
          %save('d:/yData1.txt','yOutSaved','-ascii');
          save('d:/yAVG.txt','yAVG','-ascii');
          save('d:/yHIST.txt','histData','-ascii');
      end         
   end


  %%---------------------------------------------------   
   function figureCloseCallback(hObject, eventdata)
      cleanupObjects();
   end

  %%---------------------------------------------------   
   function cleanupObjects()

      if isvalid(timerObj) 
         stop(timerObj); 
         delete(timerObj);
      end
      
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

