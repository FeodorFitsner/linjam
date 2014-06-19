/*
  ==============================================================================

    LinJam.h
    Created: 24 May 2014 5:03:01pm
    Author:  me

  ==============================================================================
*/
#ifndef LINJAM_H_INCLUDED
#define LINJAM_H_INCLUDED

#include <ninjam/audiostream.h>
#include <ninjam/njclient.h>
#include <ninjam/njmisc.h>

#include "../JuceLibraryCode/JuceHeader.h"

#include "LinJamConfig.h"
#include "MainComponent.h"


class LinJam
{
  friend class LinJamConfig ;


public:

  // state methods
  static bool Initialize(NJClient* client , MainContentComponent* contentComponent ,
                         const String& args) ;
  static void Connect() ;
  static void Disconnect() ;
  static void Shutdown() ;

  // NJClient runtime routines
  static void DriveClient() ;
  static void UpdateGUI() ;

  // helpers
  static void AddLocalChannel(String channel_id) ;
  static void SendChat(       String chat_text) ;


  static LinJamConfig* Config ;


private:

  static audioStreamer*        Audio ;
  static MainContentComponent* Gui ;
  static NJClient*             Client ;
  static int                   GuiBeatOffset ;
  static File                  SessionDir ;
  static int                   PrevStatus ;
  static bool                  IsAudioEnabled ;


  // getters/setters
  static bool IsAgreed() ;

  // NJClient callbacks
  static int  OnLicense(int user32            , char* license_text) ;
  static void OnChatmsg(int user32            , NJClient* instance ,
                        const char** parms    , int nparms)        ;
  static void OnSamples(float** input_buffer  , int n_input_channels  ,
                        float** output_buffer , int n_output_channels ,
                        int n_samples         , int sample_rate)      ;


  // NJClient runtime helpers
  static void HandleStatusChanged(int status) ;
  static void HandleUserInfoChanged() ;
  static bool IsRoomFull() ;

  // helpers
  static bool InitializeAudio() ;
  static void ConfigureAudio() ;
  static bool PrepareSessionDirectory() ;
  static void ConfigureNinjam() ;
  static void CleanSessionDir() ;
  static void HandleChatCommand(     String chat_text) ;
  static void AddChannel(            Identifier mixergroup_id , Identifier channel_id ,
                                     int        channel_idx) ;
  static bool ConfigureChannelByName(Identifier channel_id                             ,
                                     bool       should_set_volume    , float volume    ,
                                     bool       should_set_pan       , float pan       ,
                                     bool       should_set_is_xmit   , bool  is_xmit   ,
                                     bool       should_set_is_muted  , bool  is_muted  ,
                                     bool       should_set_is_solo   , bool  is_solo   ,
                                     bool       should_set_source_n  , int   source_n  ,
                                     bool       should_set_bit_depth , int   bit_depth ,
                                     bool       should_set_is_stereo , bool  is_stereo ) ;
  static bool ConfigureLocalChannel(int  channel_idx          , String name      ,
                                    bool should_set_volume    , float  volume    ,
                                    bool should_set_pan       , float  pan       ,
                                    bool should_set_is_xmit   , bool   is_xmit   ,
                                    bool should_set_is_muted  , bool   is_muted  ,
                                    bool should_set_is_solo   , bool   is_solo   ,
                                    bool should_set_source_n  , int    source_n  ,
                                    bool should_set_bit_depth , int    bit_depth ,
                                    bool should_set_is_stereo , bool   is_stereo ) ;


  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LinJam) ;
} ;

#endif  // LINJAM_H_INCLUDED
