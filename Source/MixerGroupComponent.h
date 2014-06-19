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

#ifndef __JUCE_HEADER_ACDD14AF75BBA576__
#define __JUCE_HEADER_ACDD14AF75BBA576__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "ChannelComponent.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MixerGroupComponent  : public Component,
                             public ButtonListener
{
public:
    //==============================================================================
    MixerGroupComponent (String mixergroup_id);
    ~MixerGroupComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

  void addChannelComponent(ValueTree channel_store) ;
  void updateChannelVU(    String channel_id , float vu) ;

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Label> mixerSectionLabel;
    ScopedPointer<TextButton> addButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixerGroupComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_ACDD14AF75BBA576__
