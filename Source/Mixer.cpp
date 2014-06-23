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
#include "Trace.h"
#include "LinJam.h"

//[/Headers]

#include "Mixer.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
Mixer::Mixer ()
{
    setName ("Mixer");
    addAndMakeVisible (mastersViewport = new Viewport ("mastersViewport"));
    mastersViewport->setScrollBarsShown (false, true);
    mastersViewport->setViewedComponent (new MixerGroup());

    addAndMakeVisible (localsViewport = new Viewport ("localsViewport"));
    localsViewport->setScrollBarsShown (false, true);
    localsViewport->setViewedComponent (new MixerGroup());

    addAndMakeVisible (remotesViewport = new Viewport ("remotesViewport"));
    remotesViewport->setScrollBarsShown (false, true);
    remotesViewport->setViewedComponent (new MixerGroup());


    //[UserPreSize]

  this->mastersMixerGroup = (MixerGroup*)this->mastersViewport->getViewedComponent() ;
  this->localsMixerGroup  = (MixerGroup*)this->localsViewport->getViewedComponent() ;
  this->remotesMixerGroup = (MixerGroup*)this->remotesViewport->getViewedComponent() ;
  this->masterChannels    = addChannelsGroup(GUI::MASTER_MIXERGROUP_GUI_ID) ;
  this->localChannels     = addChannelsGroup(GUI::LOCAL_MIXERGROUP_GUI_ID) ;
  this->mastersViewport  ->setComponentID(GUI::MASTER_MIXERGROUP_GUI_ID  + "-viewport") ;
  this->localsViewport   ->setComponentID(GUI::LOCAL_MIXERGROUP_GUI_ID   + "-viewport") ;
  this->remotesViewport  ->setComponentID(GUI::REMOTES_MIXERGROUP_GUI_ID + "-viewport") ;
  this->mastersViewport  ->toFront(false) ;
  this->localsViewport   ->toFront(false) ;
  this->remotesViewport  ->toFront(false) ;
  this->mastersMixerGroup->toFront(false) ;
  this->localsMixerGroup ->toFront(false) ;
  this->remotesMixerGroup->toFront(false) ;

    //[/UserPreSize]

    setSize (622, 292);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

Mixer::~Mixer()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

//   this->masterChannels    = nullptr ;
//   this->localChannels     = nullptr ;
//   this->mastersMixerGroup = nullptr ; // may not need to delete this explicitly ?
//   this->localsMixerGroup = nullptr ;  // may not need to delete this explicitly ?
//   this->remotesMixerGroup = nullptr ; // may not need to delete this explicitly ?

    //[/Destructor_pre]

    mastersViewport = nullptr;
    localsViewport = nullptr;
    remotesViewport = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void Mixer::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::coral);

    g.setColour (Colours::chocolate);
    g.fillRoundedRectangle (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0), 10.000f);

    g.setColour (Colours::white);
    g.drawRoundedRectangle (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0), 10.000f, 1.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void Mixer::resized()
{
    mastersViewport->setBounds (getWidth() - 144, 4, 140, 284);
    localsViewport->setBounds (4, 4, 76, 284);
    remotesViewport->setBounds (84, 4, 76, 284);
    //[UserResized] Add your own custom resize handling here..

// set viewports to one GUI::PAD inset from this with one GUI::PAD separation
// set mixergroups to the same height as their viewport with constant zero offsets
// set channelgroups to one GUI::PAD inset from their mixergroup with one GUI::PAD separation
// TODO: declare the viewports explicitly (not from jucer)
    //masters = GUI::PAD + GUI::MASTERGROUP_W ;
//   this->mastersMixerGroup->setBounds (0 , 0 , 136 , 284) ;
//   this->localsMixerGroup ->setBounds (0 , 0 , 76  , 284) ;
//   this->remotesMixerGroup->setBounds (0 , 0 , 76  , 284) ;

  // master channels group
//   int masters_x = getWidth() - GUI::MASTERS_W - GUI::PAD ;
//   int masters_y = GUI::MIXERGROUP_Y ;
  int masters_w = GUI::MASTERGROUP_W ;
  int masters_h = GUI::MIXERGROUP_H ;
//   if (this->masterChannels != nullptr)
//     this->masterChannels->setBounds(master_x , master_y , master_w , master_h) ;

  // TODO: for the most part this could be refactored to use a general helper
  //         for all groups excepting that these master channels widths o/c would not
  //         be included in the running total from the left (channels_x) because the
  //         masters group is fixed to the right
  if (this->mastersMixerGroup != nullptr)
  {
    int channels_x = GUI::PAD ;
    int channels_y = GUI::CHANNELS_Y ;
    int channels_w = GUI::MASTERS_W + 1 ; // metro doesnt place correctly - not sure why
    int channels_h = GUI::CHANNELS_H ;
    masterChannels->setBounds(channels_x , channels_y , channels_w , channels_h) ;
    this->mastersMixerGroup->setSize(masters_w , masters_h) ;
  }
/*
  // remote channels group
  int remotes_x = GUI::PAD + GUI::MIXERGROUP_W(localChannels->getNumChannels()) ;
  int remotes_y = GUI::MIXERGROUP_Y ;
  int remotes_w = GUI::MIXERGROUP_W(remot->getNumChannels()) ;
  int remotes_h = GUI::MIXERGROUP_H ;
  if (this->remotesMixerGroup != nullptr)
    this->remotesMixerGroup->setBounds(remotes_x , remotes_y , remotes_w , remotes_h) ;
*/
  // local and remote channels
  int mixergroup_x  = 0 ;
  int mixergroup_y  = GUI::MIXERGROUP_Y ;
  int mixergroup_w  = 0 ;
  int mixergroup_h  = GUI::MIXERGROUP_H ;
  int n_mixergroups = getNumChildComponents() ;
DBG("Mixer::resized() n_mixergroups=" + String(n_mixergroups)) ;
  for (int mixergroup_n = 1 ; mixergroup_n < n_mixergroups ; ++mixergroup_n)
  {
    int channels_x = 0 ;
    int channels_y = GUI::CHANNELS_Y ;
    int channels_w = 0 ;
    int channels_h = GUI::CHANNELS_H ;

    // resize channels groups
    Viewport*   viewport   = (Viewport*)getChildComponent(mixergroup_n) ;
    MixerGroup* mixergroup = (MixerGroup*)viewport->getViewedComponent() ;
    int n_channelgroups    = mixergroup->getNumChildComponents() ;

DBG("Mixer::resized() mixergroup_n=" + String(mixergroup_n) + " viewport_id=" + String(viewport->getComponentID()) +
    " n_channelgroups=" + String(n_channelgroups)) ;
    for (int channelgroup_n = 0 ; channelgroup_n < n_channelgroups ; ++channelgroup_n)
    {
      Channels* channels = (Channels*)mixergroup->getChildComponent(channelgroup_n) ;
      channels_x        += GUI::PAD + channels_w ;
      channels_w         = GUI::CHANNELS_W(channels->getNumChannels()) ;
      channels->setBounds(channels_x , channels_y , channels_w , channels_h) ;
DBG("Mixer::resized() mixergroup_n=" + String(mixergroup_n) + " viewport_id=" + String(viewport->getComponentID()) +
    " channelgroup_n=" + String(channelgroup_n) + " channels_id=" + String(channels->getComponentID()) +
    " n_chanels=" + String(channels->getNumChannels())) ;
    }
    // resize mixer group
    mixergroup_x += GUI::PAD + mixergroup_w ;
    mixergroup_w  = GUI::PAD + channels_x + channels_w ;
    mixergroup->setSize(mixergroup_w , mixergroup_h) ;
    // TODO: should actually not be resizing viewport explicitly unless there is enough space
//     viewport  ->setTopLeftPosition(mixergroup_x , mixergroup_y) ;
    viewport->setBounds(mixergroup_x , mixergroup_y , mixergroup_w , mixergroup_h) ;
  }
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

/* Mixer class public methods */

Channels* Mixer::getRemoteChannelsGroup(Identifier user_id)
{
  Channels* a_component = (Channels*)this->remotesMixerGroup->findChildWithID(user_id) ;
  return (a_component)? a_component : addChannelsGroup(String(user_id)) ;
}

void Mixer::addChannel(Identifier channels_id , ValueTree channel_store)
{
if      (channels_id == GUI::MASTER_MIXERGROUP_IDENTIFIER)
     { DBG("Mixer::addChannel() channels_id=" + String(channels_id) + " channel_id=" + String(channel_store.getType()) + " - adding to masters") ; }
else if (channels_id == GUI::LOCAL_MIXERGROUP_IDENTIFIER)
     { DBG("Mixer::addChannel() channels_id=" + String(channels_id) + " channel_id=" + String(channel_store.getType()) + " - adding to locals") ; }
else { DBG("Mixer::addChannel() channels_id=" + String(channels_id) + " channel_id=" + String(channel_store.getType()) + " - adding to remotes") ; }
/*
  if (channels_id == GUI::MASTER_MIXERGROUP_IDENTIFIER ||
      channels_id == GUI::LOCAL_MIXERGROUP_IDENTIFIER)
  {
    Channels* channels = (Channels*)findChildWithID(StringRef(String(channels_id))) ;
    if (!channels || !channel_store.isValid()) return ;

    channels->addChannel(channel_store) ; resized() ;
    channels->setSize(GUI::MIXERGROUP_W(channels->getNumChannels()) , GUI::MIXERGROUP_H) ;
  }
*/
  if      (channels_id == GUI::MASTER_MIXERGROUP_IDENTIFIER)
       this->mastersMixerGroup->addChannel(channels_id , channel_store) ;
  else if (channels_id == GUI::LOCAL_MIXERGROUP_IDENTIFIER)
       this->localsMixerGroup ->addChannel(channels_id , channel_store) ;
  else this->remotesMixerGroup->addChannel(channels_id , channel_store) ;

  resized() ;
}

void Mixer::updateChannelVU(Identifier channels_id , String channel_id , double vu)
{
  if      (channels_id == GUI::MASTER_MIXERGROUP_IDENTIFIER)
    this->masterChannels->updateChannelVU(channel_id , vu) ;
  else if (channels_id == GUI::LOCAL_MIXERGROUP_IDENTIFIER)
    this->localChannels ->updateChannelVU(channel_id , vu) ;
  else
  {
    Channels* channels = (Channels*)this->remotesMixerGroup->findChildWithID(channels_id) ;
    if (channels) channels->updateChannelVU(channel_id , vu) ;
  }
}


/* Mixer class private methods */

Channels* Mixer::addChannelsGroup(String channels_id)
{
if      (!channels_id.compare(GUI::MASTER_MIXERGROUP_GUI_ID))
     { DBG("Mixer::addChannelsGroup() channels_id=" + String(channels_id) + " - adding to masters") ; }
else if (!channels_id.compare(GUI::LOCAL_MIXERGROUP_GUI_ID))
     { DBG("Mixer::addChannelsGroup() channels_id=" + String(channels_id) + " - adding to locals") ; }
else { DBG("Mixer::addChannelsGroup() channels_id=" + String(channels_id) + " - adding to remotes") ; }
/*
  Channels* channels ;
  if (mixer_group_id == GUI::MASTER_MIXERGROUP_GUI_ID ||
      mixer_group_id == GUI::LOCAL_MIXERGROUP_GUI_ID)
  {
    channels = new Channels(mixer_group_id) ;
    addChildAndSetID(channels , mixer_group_id) ;
    channels->toFront(true) ;

    resized() ;
  }
*/
  if      (!channels_id.compare(GUI::MASTER_MIXERGROUP_GUI_ID))
       return this->mastersMixerGroup->addChannelsGroup(channels_id) ;
  else if (!channels_id.compare(GUI::LOCAL_MIXERGROUP_GUI_ID))
       return this->localsMixerGroup ->addChannelsGroup(channels_id) ;
  else return this->remotesMixerGroup->addChannelsGroup(channels_id) ;
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Mixer" componentName="Mixer"
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="622" initialHeight="292">
  <BACKGROUND backgroundColour="ffff7f50">
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="10" fill="solid: ffd2691e" hasStroke="1"
               stroke="1, mitered, butt" strokeColour="solid: ffffffff"/>
  </BACKGROUND>
  <VIEWPORT name="mastersViewport" id="d6981e047b1d7ba0" memberName="mastersViewport"
            virtualName="" explicitFocusOrder="0" pos="144R 4 140 284" vscroll="0"
            hscroll="1" scrollbarThickness="18" contentType="2" jucerFile=""
            contentClass="MixerGroup" constructorParams=""/>
  <VIEWPORT name="localsViewport" id="985c4dea95c75a8d" memberName="localsViewport"
            virtualName="" explicitFocusOrder="0" pos="4 4 76 284" vscroll="0"
            hscroll="1" scrollbarThickness="18" contentType="2" jucerFile=""
            contentClass="MixerGroup" constructorParams=""/>
  <VIEWPORT name="remotesViewport" id="8715f19edadbe402" memberName="remotesViewport"
            virtualName="" explicitFocusOrder="0" pos="84 4 76 284" vscroll="0"
            hscroll="1" scrollbarThickness="18" contentType="2" jucerFile=""
            contentClass="MixerGroup" constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
