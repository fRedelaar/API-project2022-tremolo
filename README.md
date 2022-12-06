
# Tremolo VST Project for Audio Processing and Indexing

This repository contains code for the Seminar Audio Processing and Indexing 2022/2023 project at Leiden University. For this project we have created a virtual Tremolo audio effect that can be used for digital audio workstations (DAW) such as FL studio. 

Some audio samples before and after the effect can be [found here](https://fredelaar.github.io/API-project2022-tremolo/). This project is built using C++ with the [JUCE framework](https://github.com/juce-framework/JUCE).

## Screenshot
![Plugin made by our team](plugin-team.PNG?raw=true)


## Getting Started

To get started and use the Tremolo VST, you should download the project as a ZIP or clone the project. You should also have installed a DAW on your computer such as FL studio. A fully-functional, free trial version with unlimited time for FL Studio can be found [here](https://www.image-line.com/fl-studio-download/). The download is available for Windows 8, 10 or 11, and MacOS 10.13.6 (High Sierra) or later.


### Prerequisites

Requirements for the software and other tools to build, test and push 
- [JUCE framework, version 7](https://github.com/juce-framework/JUCE)
- [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/)
- [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)
- [Steinberg VST 3 Audio Plug-Ins SDK](https://www.steinberg.net/developers/) 
- C++ 17 


### Installing

A step by step series of examples that tell you how to get a development environment running

#### Installing and testing the JUCE framework

1. Download and install all prerequisities.
2. Download the code from [JUCE framework](https://github.com/juce-framework/JUCE) as a zip file. 
3. Open the JUCE-master file you have just downloaded in an explorer. Navigate to
   
```../JUCE-master/extras/Projucer/Builds/VisualStudios2022```

4. Double click "Projucer.sln". This will open Visual Studios 2022.
5. After loading, go to Build > Build Solution in Visual Studios. After building, navigate to: 

```../JUCE-master/extras/Projucer/Builds/VisualStudios2022/x64/Debug/App```

6. Double click "Projucer.exe". Now the JUCE Framework is installed! 

#### Editing/Building the audio plugin
1. Download/clone this repository 
2. Navigate to:

```API_Tremolo/Builds/VisualStudio2022```

3. Double click "API_Tremolo.sln". This should open Visual Studios. 
4. Build the application. The plugin can be found as an .exe file named "API_Tremolo.exe" here:

```API_Tremolo/Builds/VisualStudio2022/x64/Debug/Standalone Plugin```

The plugin can also be found as .dll file named "API_Tremolo.dll" here:

```API_Tremolo/Builds/VisualStudio2022/x64/Debug/VST```


### Running the audio plugin in a DAW like FL Studio
1. Open FL Studio
2. **[... JESSE TO DO ...]** 


### Troubleshooting
* Make sure to have C++ as language standard in visual studios 2022. This can be configured by right mouse click on the folder "API_Tremelo_SharedCode" in the solution explorer in visual studios. Then, In Configuration Properties > General, the C++ Language standard can be adjusted. 

* The SDK must be selected **[MICHAEL TO DO]**
## Built With

  - [JUCE framework](https://github.com/juce-framework/JUCE)



## Authors

  - [Jesse de Gans](https://github.com/jessedegans)
  - [Felicia Redelaar](https://github.com/fRedelaar)
  - [Michael de Rooij](https://github.com/michaael2000)



<!---
## License (if possible)

This project is licensed under the [CC0 1.0 Universal](LICENSE.md) Creative Commons License - see the [LICENSE.md] LICENSE.md) file for details

## Acknowledgments (if possible)

  - Hat tip to anyone whose code is used
-->
