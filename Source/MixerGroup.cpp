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

//[Headers] You can add your own extra header files here...

#include "Constants.h"
                            #include "LinJam.h"

//[/Headers]

#include "MixerGroup.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MixerGroup::MixerGroup ()
{
    setName ("MixerGroup");

    //[UserPreSize]
/*
//new Channel(LinJam::Config->getChannelConfig(CONFIG::MASTERS_IDENTIFIER , CONFIG::MASTER_IDENTIFIER))
// new Channels(GUI::MASTER_MIXERGROUP_GUI_ID)
*/
    //[/UserPreSize]

    setSize (68, 284);


    //[Constructor] You can add your own custom stuff here..
/*
  this->remotesContainer = this->remotesViewport->getViewedComponent() ;
  this->remotesViewport->toFront(false) ;
  this->remotesContainer->toFront(false) ;
*/
    //[/Constructor]
}

MixerGroup::~MixerGroup()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
/*
  remotesContainer->deleteAllChildren() ;
  remotesContainer = nullptr ;
*/
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MixerGroup::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::green);

    g.setColour (Colour (0xff2aa56d));
    g.fillRoundedRectangle (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0), 10.000f);

    g.setColour (Colours::white);
    g.drawRoundedRectangle (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0), 10.000f, 1.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MixerGroup::resized()
{
    //[UserResized] Add your own custom resize handling here..
/*
this->remotesContainer = this->remotesViewport->getViewedComponent() ;
this->remotesContainer->setBounds (getWidth() / 6, getHeight() / 6 , getWidth() / 4, getHeight() / 4);
*/
DBG("MixerGroup::resized() deferred") ;
/*
  // local and remote channels
  int channels_x = 0 ;
  int channels_y = GUI::MIXERGROUP_Y ;
  int channels_w = 0 ;
  int channels_h = GUI::MIXERGROUP_H ;
  int n_groups   = getNumChildComponents() ; Channels* channels ;
  for (int group_n = 1 ; group_n < n_groups ; ++group_n) // first child is master
  {
    channels    = (Channels*)getChildComponent(group_n) ;
    channels_x += GUI::PAD + channels_w ;
    channels_w  = GUI::MIXERGROUP_W(channels->getNumChannels()) ;
    channels->setBounds(channels_x , channels_y , channels_w , channels_h) ;
  }
*/
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

Channels* MixerGroup::addChannelsGroup(String channels_id)
{
DBG("MixerGroup::addChannelsGroup(" + String(getComponentID()) + ") channels_id=" + channels_id) ;

  Channels* channels = new Channels(channels_id) ;
  addChildAndSetID(channels , channels_id) ;
  channels->toFront(false) ; //resized() ;

  return channels ;
}

void MixerGroup::addChannel(Identifier channels_id , ValueTree channel_store)
{
DBG("MixerGroup::addChannel(" + String(getComponentID()) + ") channels_id=" + String(channels_id) + " channel_id=" + String(channel_store.getType())) ;

  Channels* channels = (Channels*)findChildWithID(StringRef(String(channels_id))) ;
  if (!channels || !channel_store.isValid()) return ;

  channels->addChannel(channel_store) ; resized() ;
  channels->setSize(GUI::CHANNELS_W(channels->getNumChannels()) , GUI::CHANNELS_H) ;
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MixerGroup" componentName="MixerGroup"
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="68" initialHeight="284">
  <BACKGROUND backgroundColour="ff008000">
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="10" fill="solid: ff2aa56d" hasStroke="1"
               stroke="1, mitered, butt" strokeColour="solid: ffffffff"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
