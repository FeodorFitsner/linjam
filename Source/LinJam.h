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
#  include <windows.h>
#  include <stdio.h>
#  include <WDL/string.h>
#  include <WDL/ptrlist.h>
#  include <ninjam/njasiodrv/njasiodrv_if.h>
#endif // _WIN32

#include <ninjam/audiostream.h>
#include <ninjam/njclient.h>
#include <ninjam/njmisc.h>

#include "JuceHeader.h"

#include "LinJamConfig.h"
#include "MainContent.h"


class LinJam
{
  friend class LinJamApplication ;
  friend class LinJamConfig ;
  friend class Trace ;
  friend class License ; // TODO: needs access to config object (issue #)
  friend class Login ;   // TODO: needs access to config object (issue #)


public:

  // state methods
  static void Connect() ;
  static void Disconnect() ;
  static void Shutdown() ;

  // getters/setters
  static bool           IsAgreed() ;
  static SortedSet<int> GetFreeInputChannels() ;
  static SortedSet<int> GetFreeInputChannelPairs() ;

  // GUI event handlers
  static bool AddLocalChannel(   ValueTree channel_store) ;
  static void RemoveLocalChannel(ValueTree channel_store) ;
  static void SendChat(          String chat_text) ;
  static void ConfigDismissed() ;


private:

  static NJClient*      Client ;
  static MainContent*   Gui ;
  static LinJamConfig*  Config ;
  static audioStreamer* Audio ;
  static Value          IsAudioEnabled ;
  static SortedSet<int> FreeInputChannels ;
  static SortedSet<int> FreeInputChannelPairs ;
  static double         GuiBeatOffset ;
  static File           SessionDir ;
  static int            ClientStatus ;
  static String         PrevRecordingTime ;


  // initialization methods
  static bool Initialize(NJClient*     nj_client , MainContent* main_content ,
                         const String& args                                  ) ;
  static void InitializeAudio() ;
  static void ConfigureInitialChannels() ;
  static bool PrepareSessionDirectory() ;
  static void ConfigureNinjam() ;
  static void CleanSessionDir() ;

  // NJClient callbacks
  static int  OnLicense(int user32 , char* license_text) ;
  static void OnChatmsg(int          user32 , NJClient* instance ,
                        const char** parms  , int       nparms   ) ;
  static void OnSamples(float** input_buffer  , int n_input_channels  ,
                        float** output_buffer , int n_output_channels ,
                        int     n_samples     , int sample_rate       ) ;

  // NJClient runtime routines
  static void HandleTimer(int timer_id) ;
  static void PumpClient() ;
  static void UpdateGuiHighPriority() ;
  static void UpdateGuiLowPriority() ;
  static void HandleStatus(int client_status) ;
  static void HandleUserInfoChanged() ;
  static void UpdateLoopProgress() ;
  static void UpdateVuMeters() ;
  static void UpdateRecordingTime() ;

  // GUI event handlers
  static void ConfigureSubscriptions() ;

  // NJClient config helpers
  static int    GetNumInputChannels() ;
  static int    GetNumVacantChannels() ;
  static int    GetVacantLocalChannelIdx() ;
  static String GetStoredChannelName(      ValueTree channel_store) ;
  static String GetLocalChannelClientName( int channel_idx) ;
  static String GetRemoteUserName(         int user_idx) ;
  static String GetRemoteChannelClientName(int user_idx , int channel_idx) ;
  static bool   IsConfiguredChannel(       int channel_idx) ;
  static double AddDecibels(               double l_vu , double r_vu) ;
  static void   ComputePannedVus(          double pan , double* l_vu , double* r_vu) ;
  static void   ScalePannedMonoVus(        double  vu_mono , double  pan ,
                                           double* l_vu    , double* r_vu) ;
  static float  ComputeStereoPan(          float pan , int stereo_status) ;
  static void   UpdateRemoteUserState(     ValueTree user_store         , int user_idx ,
                                           bool      should_ignore_user                ) ;
  static void   ConfigureMasterChannel(    Identifier a_key) ;
  static void   ConfigureMetroChannel(     Identifier a_key) ;
  static void   ConfigureLocalChannel(     ValueTree  channel_store , Identifier a_key) ;
  static void   ConfigureRemoteChannel(    ValueTree  user_store    ,
                                           ValueTree  channel_store , Identifier a_key) ;


  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LinJam) ;
} ;

#endif // _LINJAM_H_
