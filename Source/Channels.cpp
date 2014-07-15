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

#include "LinJam.h"
#include "Constants.h"
#include "ChannelConfig.h"

//[/Headers]

#include "Channels.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

#if DEBUG
#  include "./Trace/TraceChannels.h"
#endif // DEBUG

//[/MiscUserDefs]

//==============================================================================
Channels::Channels ()
{
    addAndMakeVisible (channelsLabel = new Label ("channelsLabel",
                                                  TRANS("s")));
    channelsLabel->setFont (Font (12.00f, Font::plain));
    channelsLabel->setJustificationType (Justification::centredBottom);
    channelsLabel->setEditable (false, false, false);
    channelsLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    channelsLabel->setColour (Label::textColourId, Colours::grey);
    channelsLabel->setColour (Label::outlineColourId, Colour (0x00000000));
    channelsLabel->setColour (TextEditor::textColourId, Colour (0x00000000));
    channelsLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (expandButton = new TextButton ("expandButton"));
    expandButton->setButtonText (TRANS("+"));


    //[UserPreSize]

  this->channelsLabel->setAlwaysOnTop(true) ;
  this->expandButton ->setAlwaysOnTop(true) ;

    //[/UserPreSize]

    setSize (67, 276);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

Channels::~Channels()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    channelsLabel = nullptr;
    expandButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..

  deleteAllChildren() ;

    //[/Destructor]
}

//==============================================================================
void Channels::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0xff101010));
    g.fillRoundedRectangle (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0), 10.000f);

    g.setColour (Colours::white);
    g.drawRoundedRectangle (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0), 10.000f, 1.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void Channels::resized()
{
    channelsLabel->setBounds (4, 4, getWidth() - 8, 12);
    expandButton->setBounds (getWidth() - 15, 0, 15, 16);
    //[UserResized] Add your own custom resize handling here..

  // position expand button
  int expand_btn_x = getWidth() - GUI::EXPAND_BTN_W ;
  int expand_btn_y = 0 ;
  int expand_btn_w = GUI::EXPAND_BTN_W ;
  int expand_btn_h = GUI::EXPAND_BTN_H ;
  this->expandButton->setBounds(expand_btn_x , expand_btn_y , expand_btn_w , expand_btn_h) ;

  // resize this container
  int n_channels = getNumChannels() ;
  int channels_w = GUI::MIXERGROUP_W((n_channels)? n_channels : 1) ;
  int channels_h = GUI::MIXERGROUP_H ;
  setSize(channels_w , channels_h) ;

  // shift child channels
  for (int channel_n = 0 ; channel_n < n_channels ; ++channel_n)
  {
    int channel_x = GUI::MIXERGROUP_W(channel_n) ;
    getChildComponent(channel_n)->setTopLeftPosition(channel_x , GUI::CHANNEL_Y) ;
  }

  // update mixer layout
  Mixer* mixer = (Mixer*)getParentComponent() ; if (mixer) mixer->resized() ;

    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

/* Channels class public class methods */

bool Channels::addChannel(ValueTree channel_store)
{
DEBUG_TRACE_ADD_CHANNEL_GUI_FAIL

  // ensure GUI for this channel does not already exist
  Identifier channel_id = channel_store.getType() ;
  if (!channel_store.isValid() || getChannel(channel_id)) return false ;

  // hide stereo pair 'phantom' channels
  if (bool(channel_store[CONFIG::IS_STEREO_ID])  &&
      int( channel_store[CONFIG::SOURCE_N_ID]) % 2) return true ;

  // create channel GUI
  Channel* channel = newChannel(channel_store) ;
  addChildAndSetID(channel , String(channel_id)) ;
  channel->toFront(false) ;

  // resize and shift channel slices
  resized() ;

  return true ;
}

void Channels::renameChannel(Identifier channel_id)
{
DEBUG_TRACE_RENAME_CHANNEL

  Channel* channel = getChannel(channel_id) ; if (!channel) return ;

  String new_name = channel->configStore[CONFIG::CHANNELNAME_ID].toString() ;
  channel->nameLabel->setText(new_name , juce::dontSendNotification) ;
}

void Channels::removeChannel(Identifier channel_id)
{
DEBUG_TRACE_REMOVE_CHANNEL

  // destroy channel , resize , and shift channel slices
  Component* channel = getChannel(channel_id) ;
  if (channel) { delete channel ; resized() ; }
}

int Channels::getNumChannels()
{
  return getNumChildComponents() - GUI::N_STATIC_CHANNELS_CHILDREN ;
}

void Channels::updateChannelVU(Identifier channel_id , double vu)
{
  Channel* channel = getChannel(channel_id) ;
  if (channel) channel->updateChannelVU(vu) ;
}


/* Channels class protected class methods */

Channel* Channels::getChannel(Identifier channel_id)
{
  return (Channel*)findChildWithID(StringRef(String(channel_id))) ;
}


/* MasterChannels , LocalChannels , RemoteChannels classes public class methods */

MasterChannels::MasterChannels()
{
  this->channelsLabel->setText(GUI::MASTERS_LABEL_TEXT , juce::dontSendNotification) ;
  this->expandButton->setVisible(false) ;
}

LocalChannels::LocalChannels()
{
  this->channelsLabel->setText(GUI::LOCALS_LABEL_TEXT , juce::dontSendNotification) ;
  this->expandButton->setColour(TextButton::buttonColourId   , Colour(0xff004000)) ;
  this->expandButton->setColour(TextButton::buttonOnColourId , Colour(0xff008000)) ;
  this->expandButton->setColour(TextButton::textColourOnId   , Colour(0xff00ff00)) ;
  this->expandButton->setColour(TextButton::textColourOffId  , Colour(0xff00ff00)) ;
  this->expandButton->addListener(this) ;
}

RemoteChannels::RemoteChannels(ValueTree user_store)
{
  this->channelsLabel->setText(String(user_store.getType()) , juce::dontSendNotification) ;
  this->expandButton->setColour(TextButton::buttonColourId   , Colour(0xff404000)) ;
  this->expandButton->setColour(TextButton::buttonOnColourId , Colour(0xff808000)) ;
  this->expandButton->setColour(TextButton::textColourOnId   , Colour(0xffffff00)) ;
  this->expandButton->setColour(TextButton::textColourOffId  , Colour(0xffffff00)) ;
  this->expandButton->addListener(this) ;
  this->isExpanded = false ;
}


/* MasterChannels , LocalChannels , RemoteChannels classes private class methods */

void LocalChannels::buttonClicked(Button* a_button)
{
  if (a_button == this->expandButton)
  {
    Component*     mixer         = getParentComponent() ;
    Component*     mainContent   = mixer->getParentComponent() ;
    ChannelConfig* channelConfig = new ChannelConfig(LinJam::Config->newChannel()) ;
    channelConfig->setSize(GUI::CHANNEL_CONFIG_W , GUI::CHANNEL_CONFIG_H) ;

    int modalX = mixer->getX() + getX() + this->expandButton->getX() + GUI::EXPAND_BTN_XC ;
    int modalY = mixer->getY() + getY() + this->expandButton->getY() + GUI::EXPAND_BTN_YC ;

    Rectangle<int> modalRect = Rectangle<int>(modalX , modalY , 1 , 1) ;
    CallOutBox&    modalBox  = CallOutBox::launchAsynchronously(channelConfig ,
                                                                modalRect     ,
                                                                mainContent   ) ;
  }
}

void RemoteChannels::buttonClicked(Button* a_button)
{
  if (a_button == this->expandButton) toggleExpandChannels() ;
}

void RemoteChannels::toggleExpandChannels()
{
  this->isExpanded = !this->isExpanded ;

// TODO: (issue #45)
DBG("toggleExpandChannels() this->isExpanded=" + String(this->isExpanded)) ;
}

Channel* MasterChannels::newChannel(ValueTree channel_store)
{
  return new MasterChannel(channel_store) ;
}

Channel* LocalChannels::newChannel(ValueTree channel_store)
{
  return new LocalChannel(channel_store) ;
}

Channel* RemoteChannels::newChannel(ValueTree channel_store)
{
  return new RemoteChannel(channel_store) ;
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Channels" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="67" initialHeight="276">
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="10" fill="solid: ff101010" hasStroke="1"
               stroke="1, mitered, butt" strokeColour="solid: ffffffff"/>
  </BACKGROUND>
  <LABEL name="channelsLabel" id="11f182b0c62d16d1" memberName="channelsLabel"
         virtualName="" explicitFocusOrder="0" pos="4 4 8M 12" bkgCol="0"
         textCol="ff808080" outlineCol="0" edTextCol="0" edBkgCol="0"
         labelText="s" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12"
         bold="0" italic="0" justification="20"/>
  <TEXTBUTTON name="expandButton" id="e6ac05f3ca896afc" memberName="expandButton"
              virtualName="" explicitFocusOrder="0" pos="15R 0 15 16" buttonText="+"
              connectedEdges="0" needsCallback="0" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
