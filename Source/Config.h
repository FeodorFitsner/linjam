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

#ifndef __JUCE_HEADER_21AE3B746DB36E0C__
#define __JUCE_HEADER_21AE3B746DB36E0C__

//[Headers]     -- You can add your own extra header files here --

#include "MainContent.h"
#include "JuceHeader.h"

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class Config  : public Component,
                public ValueListener,
                public ButtonListener
{
public:
    //==============================================================================
    Config (ValueTree audio_store, ValueTree client_store, ValueTree subscriptions_store);
    ~Config();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

  void valueChanged(Value& a_value) ;

    //[/UserVariables]

    //==============================================================================
    ScopedPointer<TabbedComponent> configTabs;
    ScopedPointer<TextButton> dismissButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Config)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_21AE3B746DB36E0C__
