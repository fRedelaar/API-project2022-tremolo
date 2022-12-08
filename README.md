
# Tremolo VST Project for Audio Processing and Indexing

This repository contains code for the Seminar Audio Processing and Indexing 2022/2023 project at Leiden University. For this project we have created a virtual Tremolo audio effect that can be used for digital audio workstations (DAW) such as FL studio. 

Some audio samples before and after applying the effect can be [found here](https://fredelaar.github.io/API-project2022-tremolo/). This project is built using C++ with the [JUCE framework](https://github.com/juce-framework/JUCE).

## Screenshot
![Plugin made by our team](plugin-team.PNG?raw=true)


## Getting Startedf

To get started and use the Tremolo VST, you should download the project as a ZIP or clone the project. You should also have installed a DAW on your computer such as FL studio. A fully-functional, free trial version with unlimited time for FL Studio can be found [here](https://www.image-line.com/fl-studio-download/). The download is available for Windows 8, 10 or 11, and MacOS 10.13.6 (High Sierra) or later.

If you choose to build the plugin from the repository, you will need to have the necessary development tools and dependencies installed on your computer.

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


### Running the audio plugin in your DAW
1. To run our audio plugin in a digital audio workstation (DAW), you can either build the plugin from our GitHub repository or download a pre-built release.
2. Create a directory for the plugin and copy the API_Tremolo.dll into it.
3. Using your DAW's plugin manager you can now locate the folder you have just created which includes the API_Tremolo.dll

Please refer to your DAW's help pages on how to use the Plugin Manager:
- [FL Studio](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/basics_externalplugins.htm )
- [Ableton](https://help.ableton.com/hc/en-us/articles/209071729-Using-VST-plug-ins-on-Windows) (differs for your platform)
- [Cubase](https://steinberg.help/cubase_pro/v10.5/en/cubase_nuendo/topics/installing_and_managing_plugins/installing_and_managing_plugins_plugin_manager_installing_vst_plugins_c.html)
- [Logic Pro](https://support.apple.com/guide/logicpro/use-the-plug-in-manager-lgcp9e26ef17/mac) (Only possible if you are on mac and build the repo yourself)



### Troubleshooting
* Make sure to have C++ as language standard in visual studios 2022. This can be configured by right mouse click on the folder "API_Tremelo_SharedCode" in the solution explorer in visual studios. Then, In Configuration Properties > General, the C++ Language standard can be adjusted. 

* Make sure that the global path points towards the SDK you have installed. This can be done by opening the JUCE application, press "file" at the top left and go to "global paths". Here, under "VST (Legacy) SDK, set the correct path to the root folder for VST2 in your local files. In our case, this corresponds to: Windows (C:) > JUCE > VST2_SDK.

* When building the project, if it shows an error like the following: aeffectx.h file not found, we found the following workaround: In Visual Studio, in the solution explorer, right click on the project > properties > VC++ directories. Then under "include directories" press the arrow on the right and press "edit" and press "new line". Then select the path that leads to this root folder of the SDK: VST_SDK\VST2_SDK, which is probably part of your project folder.

## Built With

  - [JUCE framework](https://github.com/juce-framework/JUCE)



## Authors

  - [Jesse de Gans](https://github.com/jessedegans)
  - [Felicia Redelaar](https://github.com/fRedelaar)
  - [Michael de Rooij](https://github.com/michaael2000)
