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

#ifndef __JUCE_HEADER_CAE02453DB8909B0__
#define __JUCE_HEADER_CAE02453DB8909B0__

//[Headers]     -- You can add your own extra header files here --

#include "JuceHeader.h"
#include "MixerGroupComponent.h"

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MixerComponent  : public Component
{
public:
    //==============================================================================
    MixerComponent ();
    ~MixerComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

  void addMasterChannelComponent(String               channel_id) ;
  void addLocalChannelComponent( String               channel_id) ;
//   void addRemoteChannelComponent(String               gui_id              ,
//                                  String               user_gui_id) ; // TODO (issue #22)
  void updateChannelVU(          Identifier           mixergroup_id       ,
                                 String               channel_id          , float vu) ;
  void channelControlChanged(    MixerGroupComponent* mixerGroupComponent ,
                                 Identifier           channel_id          ,
                                 Identifier           config_key          ,
                                 var                  value) ;

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

  MixerGroupComponent* masterMixerGroupComponent ;
  MixerGroupComponent* localMixerGroupComponent ;

  ValueTree            getChannelConfig(      MixerGroupComponent* mixerGroupComponent ,
                                              Identifier           channel_id) ;
  MixerGroupComponent* addMixerGroupComponent(String               mixer_group_id) ;
  void                 addChannelComponent(   MixerGroupComponent* mixergroup      ,
                                              String               channel_id      ,
                                              bool                 is_xmit_enabled ,
                                              bool                 is_solo_enabled ,
                                              String               xmit_rcv_text) ;

    //[/UserVariables]

    //==============================================================================


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixerComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_CAE02453DB8909B0__
