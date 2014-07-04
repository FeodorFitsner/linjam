/*
  ==============================================================================

    LinJam.h
    Created: 24 May 2014 5:03:01pm
    Author:  me

  ==============================================================================
*/
#ifndef _LINJAM_H_
#define _LINJAM_H_


#ifdef _WIN32
#  include "audioconfig.h" // TODO: reimplement audioconfig.cpp in this class (issue #27)
#endif // _WIN32

#include <ninjam/audiostream.h>
#include <ninjam/njclient.h>
#include <ninjam/njmisc.h>

#include "JuceHeader.h"

#include "Trace.h"
#include "LinJamConfig.h"
#include "MainContent.h"


class LinJam
{
  friend class LinJamConfig ;


public:

  // state methods
  static bool Initialize(NJClient*     njClient , MainContent* mainCcontent ,
                         const String& args                                 ) ;
  static void Connect() ;
  static void Disconnect() ;
  static void Shutdown() ;

  // NJClient runtime routines
  static void DriveClient() ;
  static void UpdateGuiHighPriority() ;
  static void UpdateGuiLowPriority() ;

  // getters/setters
  static bool IsAgreed() ;

  // GUI event handlers
  static void      AddLocalChannel(   String channel_name) ;
  static void      RemoveLocalChannel(Identifier channel_id) ;
  static void      SendChat(          String chat_text) ;


  static LinJamConfig* Config ;


private:

  static NJClient*          Client ;
  static MainContent*       Gui ;
  static audioStreamer*     Audio ;
  static bool               IsAudioEnabled ;
  static float              GuiBeatOffset ;
  static File               SessionDir ;
  static int                PrevStatus ;
  static String             PrevRecordingTime ;


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
  static void HandleChatCommand(  String chat_text) ;
  static bool IsRoomFull() ;

  // state helpers
  static bool  InitializeAudio() ;
  static void  ConfigureInitialChannels() ;
  static bool  PrepareSessionDirectory() ;
  static void  ConfigureNinjam() ;
  static void  CleanSessionDir() ;

  // server event handlers
  static ValueTree GetOrAddRemoteUser(   Identifier user_name , int   user_idx ,
                                         float      volume    , float pan      ,
                                         bool       is_muted                   ) ;
  static ValueTree GetOrAddRemoteChannel(Identifier user_id      , int   user_idx    ,
                                         String     channel_name , int   channel_idx ,
                                         float      volume       , float pan         ,
                                         bool       is_rcv       , bool  is_muted    ,
                                         bool       is_solo      , int   sink_n      ,
                                         bool       is_stereo                        ) ;

  // NJClient config helpers
  static char* GetChannelName(          int channel_idx) ;
  static int   GetVacantLocalChannelIdx() ;
  static int   GetLocalChannelIdx(      Identifier channel_id) ;
  static int   GetRemoteUserIdx(        Identifier user_id) ;
  static int   GetRemoteChannelIdx(     int user_idx , Identifier channel_id) ;
  static void  ConfigureMasterChannel(  Identifier a_key) ;
  static void  ConfigureMetroChannel(   Identifier a_key) ;
  static void  ConfigureLocalChannel(   ValueTree channel_store , Identifier a_key) ;
  static void  ConfigureRemoteChannel(  ValueTree  user_store , ValueTree channel_store ,
                                        Identifier a_key) ;


  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LinJam) ;
} ;

#endif // _LINJAM_H_
