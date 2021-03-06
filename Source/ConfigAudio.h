/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_85571ED1262E7F00__
#define __JUCE_HEADER_85571ED1262E7F00__

//[Headers]     -- You can add your own extra header files here --

#include "JuceHeader.h"

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ConfigAudio  : public Component,
                     public ButtonListener,
                     public ComboBoxListener,
                     public SliderListener
{
public:
    //==============================================================================
    ConfigAudio (ValueTree config_store);
    ~ConfigAudio();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

  ValueTree configStore ;


  void broughtToFront() ;
  void loadParams() ;
  void enableComponents() ;
  void setConfig(Identifier a_key , var a_value) ;
  void restoreDefaults() ;
#if _WIN32
  bool queryAsioDevices() ;
  bool queryKernelstreamingDevices() ;

  bool queryDirectsoundDevices() ;
  bool queryWaveDevices() ;
#else // _WIN32
#  ifdef _MAC
  bool queryCoreaudioDevices() ;
#  else // _MAC
  bool queryJackServers() ;
  bool queryAlsaDevices() ;
#  endif // _MAC
#endif // _WIN32


#if _WIN32
static BOOL CALLBACK DSEnumProc(LPGUID  lpGUID      , LPCTSTR lpszDesc ,
                                LPCTSTR lpszDrvName , LPVOID  device_names) ;
#endif // _WIN32

    //[/UserVariables]

    //==============================================================================
    ScopedPointer<GroupComponent> ioGroup;
    ScopedPointer<GroupComponent> formatGroup;
    ScopedPointer<GroupComponent> buffersGroup;
    ScopedPointer<GroupComponent> routingGroup;
    ScopedPointer<TextButton> defaultsButton;
    ScopedPointer<TextButton> asioButton;
    ScopedPointer<Label> modeLabel;
    ScopedPointer<ComboBox> modeComboBox;
    ScopedPointer<Label> driverLabel;
    ScopedPointer<ComboBox> driverComboBox;
    ScopedPointer<Label> sourceLabel;
    ScopedPointer<ComboBox> sourceComboBox;
    ScopedPointer<Label> sinkLabel;
    ScopedPointer<ComboBox> sinkComboBox;
    ScopedPointer<Label> bitdepthLabel;
    ScopedPointer<ToggleButton> bps16Button;
    ScopedPointer<ToggleButton> bps24Button;
    ScopedPointer<ToggleButton> bps32Button;
    ScopedPointer<Label> samplerateLabel;
    ScopedPointer<ToggleButton> kHz44Button;
    ScopedPointer<ToggleButton> kHz48Button;
    ScopedPointer<ToggleButton> kHz96Button;
    ScopedPointer<Label> buffersLabel;
    ScopedPointer<Slider> nBuffersSlider;
    ScopedPointer<Label> xLabel;
    ScopedPointer<ComboBox> bufferComboBox;
    ScopedPointer<Label> bytesLabel;
    ScopedPointer<Label> nixConfigLabel;
    ScopedPointer<TextEditor> nixConfigText;
    ScopedPointer<Label> nSourcesLabel;
    ScopedPointer<Slider> nSourcesSlider;
    ScopedPointer<Label> nSinksLabel;
    ScopedPointer<Slider> nSinksSlider;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigAudio)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_85571ED1262E7F00__
