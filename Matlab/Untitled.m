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


connectToInstrument();


  %%---------------------------------------------------   
   function connectToInstrument
      try
         % THE FOLLOWING TWO LINES SHOULD BE CHANGED TO SUIT YOUR HARDWARE
         interfaceObj = visa('tek', 'USB0::0x0699::0x0405::C025165::0::INSTR'); 
         
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
         rethrow(lasterror);
      end
   end




end % of ScopeMath_TwoBeams