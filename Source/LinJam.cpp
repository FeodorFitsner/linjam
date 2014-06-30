/*
  ==============================================================================

    Linjam.cpp
    Created: 24 May 2014 5:03:01pm
    Author:  me

  ==============================================================================
*/


#include "LinJam.h"
#include "Channel.h"
#include "Constants.h"


class LinJamApplication ;


/* LinJam public class variables */

LinJamConfig* LinJam::Config ;


/* LinJam private class variables */

audioStreamer* LinJam::Audio          = nullptr ; // Initialize()
NJClient*      LinJam::Client         = nullptr ; // Initialize()
MainContent*   LinJam::Gui            = nullptr ; // Initialize()
float          LinJam::GuiBeatOffset ;            // InitializeAudio()
File           LinJam::SessionDir ;               // PrepareSessionDirectory()
int            LinJam::PrevStatus ;               // Initialize()
bool           LinJam::IsAudioEnabled = false ;   // TODO: use Client->IsAudioRunning() instead ?? (issue #11)


/* LinJam public class methods */

/* state methods */

bool LinJam::Initialize(NJClient* client , MainContent* mainCcontent ,
                        const String& args)
{
DEBUG_TRACE_LINJAM_INIT

  Client = client ;
  Gui    = mainCcontent ;


// TODO: parse command line args for autojoin (issue #9)

/* TODO:  (issue #19)
  audio_config =>
    win =>
      -noaudiocfg
      -jesusonic <path to jesusonic root dir>
    mac =>
      -audiostr device_name[,output_device_name]
    nix =>
      -audiostr "option value [option value ...]"
        ALSA audio options are:
          in hw:0,0    -- set input device
          out hw:0,0   -- set output device
          srate 48000  -- set samplerate
          nch 2        -- set channels
          bps 16       -- set bits/sample
          bsize 2048   -- set blocksize (bytes)
          nblock 16    -- set number of blocks */


  // load persistent configuration
  Config = new LinJamConfig() ; if (!Config->sanityCheck()) return false ;

  // configure audio , session directory , and NINJAM client
  if (InitializeAudio())         ConfigureAudio() ;  else   return false ;
  if (PrepareSessionDirectory()) ConfigureNinjam() ; else   return false ;

  // initialize networking
  PrevStatus = NJClient::NJC_STATUS_DISCONNECTED ; JNL::open_socketlib() ;

  return true ;
}

void LinJam::Connect()
{
  Client->Disconnect() ;

  String host         =      Config->currentHost.toString() ;
  String login        =      Config->currentLogin.toString() ;
  String pass         =      Config->currentPass.toString() ;
  bool   is_anonymous = bool(Config->currentIsAnonymous.getValue()) ;

  if (is_anonymous) { login = "anonymous:" + login ; pass  = "" ; }

DEBUG_TRACE_CONNECT

  Gui->statusbar->setStatusL(GUI::CONNECTING_STATUS_TEXT + host) ;
  Client->Connect(host.toRawUTF8() , login.toRawUTF8() , pass.toRawUTF8()) ;
  IsAudioEnabled = true ;
}

void LinJam::Disconnect() { IsAudioEnabled = false ; Client->Disconnect() ; }

void LinJam::Shutdown()
{
  JNL::close_socketlib() ; delete Audio ;
  CleanSessionDir() ; delete Config ;
}


/* NJClient runtime routines */

void LinJam::DriveClient()
{
  int client_status = Client->GetStatus() ;
  if (client_status != PrevStatus) HandleStatusChanged(PrevStatus = client_status) ;
  if (client_status == NJClient::NJC_STATUS_OK && Client->HasUserInfoChanged())
    HandleUserInfoChanged() ;
  if (client_status >= NJClient::NJC_STATUS_OK) while (!Client->Run()) ;
}

void LinJam::UpdateGUI()
{
#ifndef UPDATE_GUI
  return ;
#endif // UPDATE_GUI

  // loop progress
  int    sample_n , n_samples ; Client->GetPosition(&sample_n , &n_samples) ;
  int    bpi               = Client->GetBPI() ;
  float  bpm               = Client->GetActualBPM() ;
  double linear_progress   = (sample_n + GuiBeatOffset) / n_samples ;
  int    beat_n            = ((int)(bpi * linear_progress) % bpi) + 1 ;
  double discrete_progress = (float)beat_n / bpi ;
  Gui->loop->updateBeat(beat_n) ;
  Gui->loop->loopProgress = discrete_progress ; // linear_progress ;
  Gui->statusbar->setStatusR(String(bpi) + " bpi @ " + String(bpm) + " bpm") ;

  // master VU
  ValueTree metro_store = Config->getChannel(GUI::MASTERS_IDENTIFIER , CONFIG::METRO_KEY) ;
  double    master_vu   = VAL2DB(Client->GetOutputPeak()) ;
  double    metro_vu    = (bool(metro_store[CONFIG::MUTE_IDENTIFIER]))? VAL2DB(0.0) :
                          (discrete_progress * GUI::VU_RANGE) - GUI::VU_OFFSET ;
  Gui->mixer->updateChannelVU(GUI::MASTERS_IDENTIFIER , CONFIG::MASTER_KEY , master_vu) ;
  Gui->mixer->updateChannelVU(GUI::MASTERS_IDENTIFIER , CONFIG::METRO_KEY  , metro_vu) ;

  // local VU
  int channel_n = -1 ; int channel_idx ;
  while (~(channel_idx = Client->EnumLocalChannels(++channel_n)))
    Gui->mixer->updateChannelVU(GUI::LOCALS_IDENTIFIER                          ,
                                String(GetChannelName(channel_idx))             ,
                                VAL2DB(Client->GetLocalChannelPeak(channel_idx))) ;

  // remote VU
  int user_idx = -1 ; char* user_name ;
  while (user_name = Client->GetUserState(++user_idx))
  {
#ifdef DEBUG_DUPLICATE_CHANNEL_NAMES_VU_BUG
DBG("user[" + String(user_idx) + "]=" + String(Config->encodeUserId(user_name , user_idx))) ;
#endif // DEBUG_DUPLICATE_CHANNEL_NAMES_VU_BUG

    Identifier user_id = Config->encodeUserId(user_name , user_idx) ; channel_n = -1 ;
    while (~(channel_idx = Client->EnumUserChannels(user_idx , ++channel_n)))
    {
      char*  channel_name = Client->GetUserChannelState(user_idx , channel_idx) ;
      String channel_id   = String(Config->encodeChannelId(channel_name , channel_idx)) ;
      float  channel_vu   = VAL2DB(Client->GetUserChannelPeak(user_idx , channel_idx)) ;
      Gui->mixer->updateChannelVU(user_id , channel_id , channel_vu) ;

#ifdef DEBUG_DUPLICATE_CHANNEL_NAMES_VU_BUG
DBG("user[" + String(user_idx) + "]=" + String(user_id) + " channel[" + String(channel_n) + "/" + String(channel_idx) + "]="+ channel_id) ;
#endif // DEBUG_DUPLICATE_CHANNEL_NAMES_VU_BUG
    }
  }
}


/* getters/setters */

bool LinJam::IsAgreed() { return bool(Config->currentIsAgreed.getValue()) ; }


/* GUI and server event handlers */

void LinJam::AddLocalChannel(String channel_name)
{
DEBUG_TRACE_ADD_LOCAL_CHANNEL_FAIL

  // ensure that we do not exceed our maximum number of local channels
  int channel_idx = GetVacantLocalChannelIdx() ; if (!(~channel_idx)) return ;

  // ensure that a channel with this name does not already exist
  Identifier channel_id = Config->encodeChannelId(channel_name , channel_idx + 1) ;
  if (Config->doesChannelExist(GUI::LOCALS_IDENTIFIER , String(channel_id))) return ;

  // create stored config for this channel
  ValueTree  channel_store = Config->createChannel(GUI::LOCALS_IDENTIFIER    ,
                                                   channel_name              ,
                                                   channel_idx               ,
                                                   CONFIG::DEFAULT_VOLUME    ,
                                                   CONFIG::DEFAULT_PAN       ,
                                                   CONFIG::DEFAULT_IS_XMIT   ,
                                                   CONFIG::DEFAULT_IS_MUTE   ,
                                                   CONFIG::DEFAULT_IS_SOLO   ,
                                                   CONFIG::DEFAULT_SOURCE_N  ,
                                                   CONFIG::DEFAULT_IS_STEREO ) ;
  if (!channel_store.isValid()) return ;

  // create local Channel GUI and configure NJClient input channel
  Gui->mixer->addChannel(GUI::LOCALS_GUI_ID , channel_store) ;
  ConfigureLocalChannel(channel_store , CONFIG::INIT_LOCAL_IDENTIFIER) ;

DEBUG_TRACE_ADD_LOCAL_CHANNEL
}

void LinJam::RemoveLocalChannel(Identifier channel_id)
{
DEBUG_TRACE_REMOVE_LOCAL_CHANNEL

  Client->DeleteLocalChannel(GetLocalChannelIdx(channel_id)) ;
  Client->NotifyServerOfChannelChange() ;
}

ValueTree LinJam::AddRemoteChannel(ValueTree user_store   , Channels* user_channels ,
                                   String    channel_name , int       channel_idx   ,
                                   float     volume       , float     pan           ,
                                   bool      is_rcv       , bool      is_muted      ,
                                   bool      is_solo      , int       sink_n        ,
                                   bool      is_stereo                              )
{
  Identifier user_id  =     user_store.getType() ;
  int        user_idx = int(user_store[CONFIG::USERIDX_IDENTIFIER]) ;

  // test for pre-existing storage for this channel
  Identifier channel_id    = Config->encodeChannelId(channel_name , channel_idx + 1) ;
  ValueTree  channel_store = Config->getChannel(user_id , channel_id) ;

  if (!channel_store.isValid())
  {
#ifdef KLUDGE_SET_INITIAL_REMOTE_GAIN_TO_ZERO
    volume = 0.0f ;
#endif // KLUDGE_SET_INITIAL_REMOTE_GAIN_TO_ZERO

    // get or create stored config for this channel
    channel_store = Config->createChannel(user_id   , String(channel_id) , channel_idx ,
                                          volume    , pan                , is_rcv      ,
                                          is_muted  , is_solo            , sink_n      ,
                                          is_stereo                                    ) ;
    if (!channel_store.isValid()) return ValueTree::invalid ;

#ifdef KLUDGE_SET_INITIAL_REMOTE_GAIN_TO_ZERO
    LinJam::ConfigureRemoteChannel(user_store , channel_store , CONFIG::VOLUME_IDENTIFIER) ;
#endif // KLUDGE_SET_INITIAL_REMOTE_GAIN_TO_ZERO
  }

  // create remote user and channel GUI
  if (!user_channels->findChildWithID(StringRef(String(channel_id))))
    Gui->mixer->addChannel(String(user_id) , channel_store) ;

DEBUG_TRACE_ADD_REMOTE_CHANNEL

  return channel_store ;
}

void LinJam::SendChat(String chat_text)
{
DEBUG_TRACE_CHAT_OUT

  if ((chat_text = chat_text.trim()).isEmpty()) return ;

  if (chat_text.startsWith("/")) HandleChatCommand(chat_text) ;
  else Client->ChatMessage_Send(CLIENT::CHATMSG_TYPE_MSG.toRawUTF8() , chat_text.toRawUTF8()) ;
}


/* LinJam class private class methods */

/* NJClient callbacks */

int LinJam::OnLicense(int user32 , char* license_text)
{
#ifdef DEBUG_AUTOLOGIN_NYI
return true ;
#endif // DEBUG_AUTOLOGIN_NYI

  ValueTree server         = Config->getCurrentServer() ;
  bool should_always_agree = server.isValid() &&
                             bool(server.getProperty(CONFIG::AGREE_IDENTIFIER)) ;
  bool is_agreed           = IsAgreed() || should_always_agree ;

  if (!is_agreed) Gui->license->setLicenseText(CharPointer_UTF8(license_text)) ;
  else            Config->currentIsAgreed = false ;

DEBUG_TRACE_LICENSE

  return is_agreed ;
}

void LinJam::OnChatmsg(int user32 , NJClient* instance , const char** parms , int nparms)
{
  if (!parms[0]) return ;

  String chat_type = String(CharPointer_UTF8(parms[CLIENT::CHATMSG_TYPE_IDX])) ;
  String chat_user = String(CharPointer_UTF8(parms[CLIENT::CHATMSG_USER_IDX])).upToFirstOccurrenceOf("@", false , false) ;
  String chat_text = String(CharPointer_UTF8(parms[CLIENT::CHATMSG_MSG_IDX])) ;
  bool is_topic_msg = (!chat_type.compare(CLIENT::CHATMSG_TYPE_TOPIC)) ;
  bool is_bcast_msg = (!chat_type.compare(CLIENT::CHATMSG_TYPE_MSG)) ;
  bool is_priv_msg  = (!chat_type.compare(CLIENT::CHATMSG_TYPE_PRIVMSG)) ;
  bool is_join_msg  = (!chat_type.compare(CLIENT::CHATMSG_TYPE_JOIN)) ;
  bool is_part_msg  = (!chat_type.compare(CLIENT::CHATMSG_TYPE_PART)) ;

DEBUG_TRACE_CHAT_IN

  if (is_topic_msg)
  {
    if (chat_text.isEmpty()) return ;

    if (chat_user.isEmpty()) chat_text = "Topic is: "                   + chat_text ;
    else                     chat_text = chat_user + " sets topic to: " + chat_text ;
    chat_user = GUI::SERVER_NICK ;

    Gui->chat->setTopic(chat_text) ;
  }
  else if (is_bcast_msg)
    { if (chat_user.isEmpty() || chat_text.isEmpty()) return ; }
  else if (is_priv_msg)
  {
    if (chat_user.isEmpty() || chat_text.isEmpty()) return ;

    chat_user += " (whispers)" ;
  }
  else if (is_join_msg || is_part_msg)
  {
    if (chat_user.isEmpty()) return ;

    chat_text = chat_user + " has " + ((is_join_msg)? "joined" : "left") + " the jam" ;
    chat_user = GUI::SERVER_NICK ;
  }
  Gui->chat->addChatLine(chat_user , chat_text) ;
}

void LinJam::OnSamples(float** input_buffer  , int n_input_channels  ,
                       float** output_buffer , int n_output_channels ,
                       int     n_samples     , int sample_rate)
{
  if (!IsAudioEnabled)
  {
    // clear all output buffers
    uint8 n_bytes = n_samples * sizeof(float) ;
    for (int ch_n = 0 ; ch_n < n_output_channels ; ++ch_n)
      memset(output_buffer[ch_n] , 0 , n_bytes) ;
  }
  else Client->AudioProc(input_buffer  , n_input_channels  ,
                         output_buffer , n_output_channels ,
                         n_samples     , sample_rate       ) ;
}


/* NJClient runtime helpers */

void LinJam::HandleStatusChanged(int client_status)
{
DEBUG_TRACE_CONNECT_STATUS
#ifdef DEBUG_AUTOLOGIN_NYI
if (!Gui->statusbar->getStatusL().compare(GUI::DISCONNECTED_STATUS_TEXT))
{ Config->setCurrentServer(DEBUG_STATIC_CHANNEL , "nobody" , "" , true) ;
  Connect() ; }
#endif // DEBUG_AUTOLOGIN_NYI

  // server config
  if (client_status == NJClient::NJC_STATUS_OK) Config->setServer() ;

  // GUI state
  switch (client_status)
  {
    case NJClient::NJC_STATUS_DISCONNECTED: Gui->login     ->toFront(true)  ;
                                            Gui->background->toBehind(Gui->login) ;   break ;
    case NJClient::NJC_STATUS_INVALIDAUTH:  (IsAgreed())?
                                            Gui->login     ->toFront(true)  :
                                            Gui->license   ->toFront(true)  ;
                                            (IsAgreed())?
                                            Gui->background->toBehind(Gui->login)   :
                                            Gui->background->toBehind(Gui->license) ; break ;
    case NJClient::NJC_STATUS_CANTCONNECT:  Gui->login     ->toFront(true)  ;
                                            Gui->background->toBehind(Gui->login) ;   break ;
    case NJClient::NJC_STATUS_OK:           Gui->chat      ->toFront(true)  ;
                                            Gui->mixer     ->toFront(false) ;
                                            Gui->loop      ->toFront(false) ;
                                            Gui->background->toBehind(Gui->chat) ;    break ;
    case NJClient::NJC_STATUS_PRECONNECT:   Gui->login     ->toFront(true)  ;
                                            Gui->background->toBehind(Gui->login) ;   break ;
    default:                                                                          break ;
  }

  // status indicator
  String host = Client->GetHostName() ; String status_text ;
  switch (client_status)
  {
    case NJClient::NJC_STATUS_DISCONNECTED:
      status_text = GUI::DISCONNECTED_STATUS_TEXT ;                    break ;
    case NJClient::NJC_STATUS_INVALIDAUTH:
      status_text = (IsAgreed())? ((IsRoomFull())?
                    GUI::ROOM_FULL_STATUS_TEXT :
                    GUI::INVALID_AUTH_STATUS_TEXT) :
                    GUI::PENDING_LICENSE_STATUS_TEXT ;                 break ;
    case NJClient::NJC_STATUS_CANTCONNECT:
      status_text = GUI::FAILED_CONNECTION_STATUS_TEXT ;               break ;
    case NJClient::NJC_STATUS_OK:
      status_text = GUI::CONNECTED_STATUS_TEXT + host ;                break ;
    case NJClient::NJC_STATUS_PRECONNECT:
      status_text = GUI::IDLE_STATUS_TEXT ;                            break ;
    default:
      status_text = GUI::UNKNOWN_STATUS_TEXT + String(client_status) ; break ;
  }
  Gui->statusbar->setStatusL(status_text) ;
}

void LinJam::HandleUserInfoChanged()
{
#ifndef UPDATE_REMOTES
  return ;
#endif // UPDATE_REMOTES
DEBUG_TRACE_REMOTE_CHANNELS_VB

  // initialize dictionary for pruning GUI elements for parted users
  ValueTree active_users = ValueTree("active-users") ;

  // fetch remote user states from server
  int user_idx = -1 ; char* user_c_name ; float volume ; float pan ; bool is_muted ;
  while (user_c_name = Client->GetUserState(++user_idx , &volume , &pan , &is_muted))
  {
// TODO: we are adding remote users directly to the root node for now for simplicity
//           mostly because Trace::SanitizeConfig() does not yet handle nested lists
//           but for clarity there should be a <remote-channels> tree (issue #33)
    Identifier user_id   = Config->encodeUserId(String(user_c_name) , user_idx) ;
    String     user_name = String(user_id) ;
    if (bool(Config->shouldHideBots.getValue()) &&
        NETWORK::KNOWN_BOTS.contains(user_id)) continue ;

    // get or add remote user storage
    ValueTree user_store = Config->getOrCreateUser(user_name , user_idx ,
                                                   volume    , pan      , is_muted) ;

    // get or add remote user channels GUI
    Channels* user_channels = Gui->mixer->getOrAddRemoteChannels(user_store) ;

    // list this user as active for GUI pruning
    ValueTree active_channels = ValueTree(user_store.getType()) ;

    int channel_n = -1 ; int channel_idx ;
    while (~(channel_idx = Client->EnumUserChannels(user_idx , ++channel_n)))
    {
      // load remote channel state
      bool  is_rcv ;   float volume ; float pan ;
      bool  is_muted ; bool is_solo ; int   sink_channel ; bool  is_stereo ;
      char* channel_c_name = Client->GetUserChannelState(user_idx      , channel_idx ,
                                                         &is_rcv                     ,
                                                         &volume       , &pan        ,
                                                         &is_muted     , &is_solo    ,
                                                         &sink_channel , &is_stereo  ) ;
      String channel_name = String(channel_c_name) ;

      // store , configure , and create GUI for this remote channel
      ValueTree channel_store = AddRemoteChannel(user_store   , user_channels ,
                                                 channel_name , channel_idx   ,
                                                 volume       , pan           ,
                                                 is_rcv       , is_muted      ,
                                                 is_solo      , sink_channel  ,
                                                 is_stereo) ;
      if (!channel_store.isValid()) continue ;

      // add channel to GUI prune list
      active_channels.setProperty(channel_store.getType() , -1 , nullptr) ;
    }
    // add user to GUI prune list
    active_users.addChild(active_channels , -1 , nullptr) ;
  }
  // prune user and channel GUIs
  Gui->mixer->pruneRemotes(active_users) ;
}

void LinJam::HandleChatCommand(String chat_text)
{
  String command        = chat_text.upToFirstOccurrenceOf(" " , false , false) ;
  bool is_me_command    = (!command.compare(CLIENT::CHATMSG_CMD_ME)) ;
  bool is_pm_command    = (!command.compare(CLIENT::CHATMSG_CMD_MSG)) ;
  bool is_admin_command = (!command.compare(CLIENT::CHATMSG_CMD_ADMIN)) ;
  bool is_user_command  = (!command.compare(CLIENT::CHATMSG_CMD_TOPIC) ||
                           !command.compare(CLIENT::CHATMSG_CMD_KICK)  ||
                           !command.compare(CLIENT::CHATMSG_CMD_BPM)   ||
                           !command.compare(CLIENT::CHATMSG_CMD_BPI)    ) ;

#ifdef BUGGY_CHAT_COMMANDS // (issue #19)
  Gui->chat->addChatLine(GUI::SERVER_NICK , "commands disabled") ; return ;
#endif // CHAT_COMMANDS_BUGGY

  if      (is_me_command)
  {
    String msg = String(Client->GetUserName()) + " " + chat_text ;
    Client->ChatMessage_Send(CLIENT::CHATMSG_TYPE_MSG.toRawUTF8() , msg.toRawUTF8()) ;
  }
  else if (is_user_command)
  {
    String msg = chat_text.substring(1) ;
    Client->ChatMessage_Send(CLIENT::CHATMSG_TYPE_ADMIN.toRawUTF8() , msg.toRawUTF8()) ;
  }
  else if (is_admin_command)
  {
    String msg = chat_text.substring(6).trim() ;
    Client->ChatMessage_Send(CLIENT::CHATMSG_TYPE_ADMIN.toRawUTF8() , msg.toRawUTF8()) ;
  }
  else if (is_pm_command)
  {
    String to_user = chat_text.substring(4).trim() ;
    to_user        = to_user.upToFirstOccurrenceOf(StringRef(" ") , false , false) ;
    String msg     = to_user.fromFirstOccurrenceOf(StringRef(" ") , false , false).trim() ;

    if (to_user.isEmpty() || msg.isEmpty())
      Gui->chat->addChatLine(GUI::SERVER_NICK , GUI::INVALID_PM_MSG) ;
    else // if (does_user_exist(to_user)) // TODO: this safe yea ? // (issue #19)
    {
      Client->ChatMessage_Send(CLIENT::CHATMSG_TYPE_PRIVMSG.toRawUTF8() , msg.toRawUTF8()) ;
      Gui->chat->addChatLine("(PM -> " + to_user + ")" , msg) ;
    }
  }
  else Gui->chat->addChatLine(GUI::SERVER_NICK , GUI::UNKNOWN_COMMAND_MSG) ;
}

bool LinJam::IsRoomFull()
{
  String err = String(CharPointer_UTF8(Client->GetErrorStr())) ;
  return (err.isNotEmpty() && !err.compare(CLIENT::SERVER_FULL_STATUS)) ;
}


/* state helpers */

bool LinJam::InitializeAudio()
{
        int   interface_n   = int(Config->audioIfN  .getValue()) ;
        int   n_inputs      = int(Config->nInputs   .getValue()) ;
        int   n_outputs     = int(Config->nOutputs  .getValue()) ;
        int   bit_depth     = int(Config->bitDepth  .getValue()) ;
        int   sample_rate   = int(Config->sampleRate.getValue()) ;
  const char* jack_name     =     Config->jackName  .toString().toRawUTF8() ;
        char* config_string = "" ;
#ifdef _WIN32
  audioStreamer::WinAudioIf if_n = (audioStreamer::WinAudioIf)interface_n ;
  Audio = CreateConfiguredStreamer(CLIENT::WIN_INI_FILE , if_n , OnSamples) ;

DEBUG_TRACE_AUDIO_INIT_WIN
#else // _WIN32
#  ifdef _MAC
  Audio = create_audioStreamer_CoreAudio(&config_string , sample_rate ,
                                         n_inputs , bit_depth , OnSamples) ;

DEBUG_TRACE_AUDIO_INIT_MAC
#  else // _MAC
  switch (interface_n)
  {
    case 0: // JACK
      Audio = create_audioStreamer_JACK(jack_name , n_inputs , n_outputs ,
                                        OnSamples , Client) ;

DEBUG_TRACE_AUDIO_INIT_JACK

      if (Audio) break ;
    case 1: // ALSA
    default:
      Audio = create_audioStreamer_ALSA(config_string , OnSamples) ;

DEBUG_TRACE_AUDIO_INIT_ALSA
  }
#  endif // _MAC
#endif // _WIN32

  if (Audio) GuiBeatOffset = Audio->m_srate * (CLIENT::GUI_DRIVER_IVL * 0.002) ;

DEBUG_TRACE_AUDIO_INIT

  return (!!Audio) ;
}

void LinJam::ConfigureAudio()
{
  // configure master channels
  ValueTree master_store = Config->masterChannels.getChildWithName(GUI::MASTER_CHANNEL_GUI_ID) ;
  ValueTree metro_store  = Config->masterChannels.getChildWithName(GUI::METRO_CHANNEL_GUI_ID) ;
  float master_volume    = float(master_store.getProperty(CONFIG::VOLUME_IDENTIFIER)) ;
  float master_pan       = float(master_store.getProperty(CONFIG::PAN_IDENTIFIER)) ;
  bool  is_master_muted  = bool( master_store.getProperty(CONFIG::MUTE_IDENTIFIER)) ;
  float metro_volume     = float(metro_store .getProperty(CONFIG::VOLUME_IDENTIFIER)) ;
  float metro_pan        = float(metro_store .getProperty(CONFIG::PAN_IDENTIFIER)) ;
  bool  is_metro_muted   = bool( metro_store .getProperty(CONFIG::MUTE_IDENTIFIER)) ;
  int   metro_source     = int(  metro_store .getProperty(CONFIG::SOURCE_N_IDENTIFIER)) ;
  bool  is_metro_stereo  = bool( metro_store .getProperty(CONFIG::STEREO_IDENTIFIER)) ;

  // configure master and metro channels
  Client->config_mastervolume        = (float)DB2VAL(master_volume) ;
  Client->config_masterpan           = master_pan ;
  Client->config_mastermute          = is_master_muted ;
  Client->config_metronome           = (float)DB2VAL(metro_volume) ;
  Client->config_metronome_pan       = metro_pan ;
  Client->config_metronome_mute      = is_metro_muted ;
  Client->config_metronome_channel   = metro_source ;
  Client->config_metronome_stereoout = is_metro_stereo ;

  // add master and metro channel GUI mixers
  Gui->mixer->addChannel(GUI::MASTERS_GUI_ID , master_store) ;
  Gui->mixer->addChannel(GUI::MASTERS_GUI_ID , metro_store) ;

  // configure input channels
  for (int channel_n = 0 ; channel_n < Config->localChannels.getNumChildren() ; ++channel_n)
  {
    ValueTree  channel_store = Config->localChannels.getChild(channel_n) ;

    // create local Channel GUI and configure NJClient input channel
    Gui->mixer->addChannel(GUI::LOCALS_GUI_ID , channel_store) ;
    ConfigureLocalChannel(channel_store , CONFIG::INIT_LOCAL_IDENTIFIER) ;
  }
}

bool LinJam::PrepareSessionDirectory()
{
  File this_binary = File::getSpecialLocation(File::currentExecutableFile) ;
  File this_dir    = this_binary.getParentDirectory() ;
  SessionDir       = File(this_dir.getFullPathName() + CONFIG::SESSIONDIR) ;

  SessionDir.createDirectory() ; CleanSessionDir() ;

  bool does_session_dir_exist = SessionDir.isDirectory() ;
  if (does_session_dir_exist) Client->SetWorkDir(SessionDir.getFullPathName().toRawUTF8()) ;

  return does_session_dir_exist ;
}

void LinJam::ConfigureNinjam()
{
  int       should_save_audio     = int( Config->shouldSaveAudio    .getValue()) ;
  bool      should_save_log       = bool(Config->shouldSaveLog      .getValue()) ;
  int       debug_level           = int( Config->debugLevel         .getValue()) ;
  int       should_auto_subscribe = int( Config->shouldAutoSubscribe.getValue()) ;
  ValueTree subscriptions         = Config->autoSubscribeUsers ;

  Client->LicenseAgreementCallback = OnLicense ;
  Client->ChatMessage_Callback     = OnChatmsg ;
  Client->config_savelocalaudio    = should_save_audio ;
  Client->config_debug_level       = debug_level ;
  Client->config_autosubscribe     = should_auto_subscribe ;
  if (should_save_audio && should_save_log)
    Client->SetLogFile((SessionDir.getFullPathName() + CONFIG::LOGFILE).toRawUTF8()) ;
  for (int user_n = 0 ; user_n < subscriptions.getNumChildren() ; ++user_n)
  {
    WDL_String user_name_wdl(subscriptions.getChild(user_n).getType().getCharPointer()) ;
    Client->config_autosubscribe_userlist.insert(user_name_wdl.Get()) ;
  }
}

void LinJam::CleanSessionDir()
{
  bool should_save_audio = bool(Config->shouldSaveAudio.getValue()) ;
  if (should_save_audio) return ;

DEBUG_TRACE_CLEAN_SESSION

  File this_binary = File::getSpecialLocation(File::currentExecutableFile) ;
  File this_dir    = this_binary.getParentDirectory() ;
  if (!SessionDir.isDirectory() || !SessionDir.isAChildOf(this_dir)) return ;

  // NOTE: the *.ninjam directories created when save_loca_audio == -1 (delete ASAP)
  //           are not being deleted implicitly as they presumably should be (issue #32)
  DirectoryIterator session_dir_iter(SessionDir , false , "*.*" , File::findFilesAndDirectories) ;
  while (session_dir_iter.next()) session_dir_iter.getFile().deleteRecursively() ;
}


/* NJClient config helpers */

char* LinJam::GetChannelName(int channel_idx)
{ return Client->GetLocalChannelInfo(channel_idx , NULL , NULL , NULL) ; }

int LinJam::GetVacantLocalChannelIdx()
{
  int channel_idx = -1 ; while (GetChannelName(++channel_idx)) ;
  bool is_vacant_slot = (channel_idx < Client->GetMaxLocalChannels()) ;

  return (is_vacant_slot)? channel_idx : -1 ;
}

int LinJam::GetLocalChannelIdx(Identifier channel_id)
{
  // find local channel
  int channel_n = -1 ; int channel_idx ;
  while (~(channel_idx = LinJam::Client->EnumLocalChannels(++channel_n)))
  {
    char*  channel_c_name = GetChannelName(channel_idx) ;
    String channel_name   = String(Config->encodeChannelId(channel_c_name , channel_idx)) ;
    if (!String(channel_id).compare(channel_name)) break ;
  }

  return channel_idx ;
}

int LinJam::GetRemoteUserIdx(Identifier user_id)
{
  // find remote user
  int user_idx = Client->GetNumUsers() ; char* user_c_name ;
  while ((user_c_name = Client->GetUserState(--user_idx)))
  {
    String user_name = String(Config->encodeUserId(user_c_name , user_idx)) ;
    if (!String(user_id).compare(user_name)) break ;
  }

  return user_idx ;
}

int LinJam::GetRemoteChannelIdx(int user_idx , Identifier channel_id)
{
  // find remote channel
  int channel_n = -1 ; int channel_idx ;
  while (~(channel_idx = Client->EnumUserChannels(user_idx , ++channel_n)))
  {
    char*  channel_c_name = Client->GetUserChannelState(user_idx , channel_idx) ;
    String channel_name   = String(Config->encodeChannelId(channel_c_name , channel_idx)) ;
    if (!String(channel_id).compare(channel_name)) break ;
  }

  return channel_idx ;
}

void LinJam::ConfigureMasterChannel(Identifier a_key)
{
  ValueTree channel_store = Config->getChannel(CONFIG::MASTERS_IDENTIFIER ,
                                               CONFIG::MASTER_IDENTIFIER) ;
  if (!channel_store.isValid()) return ;

  // load stored config for this channel
  float volume   = float(channel_store[CONFIG::VOLUME_IDENTIFIER]) ;
  float pan      = float(channel_store[CONFIG::PAN_IDENTIFIER]) ;
  bool  is_muted = bool( channel_store[CONFIG::MUTE_IDENTIFIER]) ;

  // configure NJClient master channel
  if      (ShouldSetVolume(a_key))  Client->config_mastervolume = (float)DB2VAL(volume) ;
  else if (ShouldSetPan(a_key))     Client->config_masterpan    = pan ;
  else if (ShouldSetIsMuted(a_key)) Client->config_mastermute   = is_muted ;
}

void LinJam::ConfigureMetroChannel(Identifier a_key)
{
  ValueTree channel_store = Config->getChannel(CONFIG::MASTERS_IDENTIFIER ,
                                               CONFIG::METRO_IDENTIFIER) ;
  if (!channel_store.isValid()) return ;

  // load stored config for this channel
  float volume    = float(channel_store[CONFIG::VOLUME_IDENTIFIER]) ;
  float pan       = float(channel_store[CONFIG::PAN_IDENTIFIER]) ;
  bool  is_muted  = bool( channel_store[CONFIG::MUTE_IDENTIFIER]) ;
  int   source_n  = int(  channel_store[CONFIG::SOURCE_N_IDENTIFIER]) ;
  bool  is_stereo = bool( channel_store[CONFIG::STEREO_IDENTIFIER]) ;

  // configure NJClient metro channel
  if      (ShouldSetVolume(a_key))   Client->config_metronome           = (float)DB2VAL(volume) ;
  else if (ShouldSetPan(a_key))      Client->config_metronome_pan       = pan ;
  else if (ShouldSetIsMuted(a_key))  Client->config_metronome_mute      = is_muted ;
  else if (ShouldSetSourceN(a_key))  Client->config_metronome_channel   = source_n ;
  else if (ShouldSetIsStereo(a_key)) Client->config_metronome_stereoout = is_stereo ;
}

void LinJam::ConfigureLocalChannel(ValueTree channel_store , Identifier a_key)
{
  if (!channel_store.isValid()) return ;

  // load stored config for this channel
  Identifier channel_id =       channel_store.getType() ;
  float      volume     = float(channel_store[CONFIG::VOLUME_IDENTIFIER]) ;
  float      pan        = float(channel_store[CONFIG::PAN_IDENTIFIER]) ;
  bool       is_xmit    = bool( channel_store[CONFIG::XMIT_IDENTIFIER]) ;
  bool       is_muted   = bool( channel_store[CONFIG::MUTE_IDENTIFIER]) ;
  bool       is_solo    = bool( channel_store[CONFIG::SOLO_IDENTIFIER]) ;
  int        source_n   = int(  channel_store[CONFIG::SOURCE_N_IDENTIFIER]) ;
  int        bit_depth  = int(  channel_store[CONFIG::BITDEPTH_IDENTIFIER]) ;
  bool       is_stereo  = bool( channel_store[CONFIG::STEREO_IDENTIFIER]) ;

  bool should_set_all       = (a_key == CONFIG::INIT_LOCAL_IDENTIFIER) ;
  bool should_set_name      = (should_set_all || ShouldSetName(a_key)) ;
  bool should_set_volume    = (should_set_all || ShouldSetVolume(a_key)) ;
  bool should_set_pan       = (should_set_all || ShouldSetPan(a_key)) ;
  bool should_set_is_xmit   = (should_set_all || ShouldSetIsXmit(a_key)) ;
  bool should_set_is_muted  = (should_set_all || ShouldSetIsMuted(a_key)) ;
  bool should_set_is_solo   = (should_set_all || ShouldSetIsSolo(a_key)) ;
  bool should_set_source_n  = (should_set_all || ShouldSetSourceN(a_key)) ;
  bool should_set_bit_depth = (should_set_all || ShouldSetBitDepth(a_key)) ;
  bool should_set_is_stereo = (should_set_all || ShouldSetIsStereo(a_key)) ;

DEBUG_TRACE_CONFIGURE_LOCAL_CHANNEL

  // find NJClient channel_idx for existing or new channel
  int channel_idx ; if (!(~(channel_idx = GetLocalChannelIdx(channel_id))) &&
                        !(~(channel_idx = GetVacantLocalChannelIdx()))      ) return ;

  // convert channel name from juce::String into non-const char*
  String name = String(channel_id) ; char* new_name = NULL ; WDL_String name_wdl ;
  if (should_set_name) { name_wdl.Set(name.toRawUTF8()) ; new_name = name_wdl.Get() ; }

  // configure NJClient local channel
  if (should_set_name || should_set_source_n || should_set_bit_depth || should_set_is_xmit)
    Client->SetLocalChannelInfo(channel_idx          , new_name  ,
                                should_set_source_n  , source_n  ,
                                should_set_bit_depth , bit_depth ,
                                should_set_is_xmit   , is_xmit   ) ;

  if (should_set_volume || should_set_pan || should_set_is_muted || should_set_is_solo)
    Client->SetLocalChannelMonitoring(channel_idx                                 ,
                                      should_set_volume   , (float)DB2VAL(volume) ,
                                      should_set_pan      , pan                   ,
                                      should_set_is_muted , is_muted              ,
                                      should_set_is_solo  , is_solo               ) ;
  Client->NotifyServerOfChannelChange() ;
}

void LinJam::ConfigureRemoteUser(ValueTree user_store , Identifier a_key)
{
  if (!user_store.isValid()) return ;
  ValueTree channel_store = user_store.getChildWithName(CONFIG::MASTER_IDENTIFIER) ;
  if (!channel_store.isValid()) return ;

  // load stored config for this channel
  int   user_idx = int(  user_store   [CONFIG::USERIDX_IDENTIFIER]) ;
  float volume   = float(channel_store[CONFIG::VOLUME_IDENTIFIER]) ;
  float pan      = float(channel_store[CONFIG::PAN_IDENTIFIER]) ;
  bool  is_muted = bool( channel_store[CONFIG::MUTE_IDENTIFIER]) ;

DEBUG_TRACE_CONFIGURE_REMOTE_USER

  if (user_idx != GetRemoteUserIdx(user_id)) return ;

  Client->SetUserState(user_idx                                         ,
                       ShouldSetVolume(a_key)   , (float)DB2VAL(volume) ,
                       ShouldSetPan(a_key)      , pan                   ,
                       ShouldSetIsMuted(a_key)  , is_muted              ) ;
}

void LinJam::ConfigureRemoteChannel(ValueTree user_store , ValueTree channel_store ,
                                    Identifier a_key)
{
  if (!user_store.isValid() || !channel_store.isValid()) return ;

  // load stored config for this channel
  float volume    = float(channel_store[CONFIG::VOLUME_IDENTIFIER]) ;
  float pan       = float(channel_store[CONFIG::PAN_IDENTIFIER]) ;
  bool  is_rcv    = bool( channel_store[CONFIG::XMIT_IDENTIFIER]) ;
  bool  is_muted  = bool( channel_store[CONFIG::MUTE_IDENTIFIER]) ;
  bool  is_solo   = bool( channel_store[CONFIG::SOLO_IDENTIFIER]) ;
  int   sink_n    = int(  channel_store[CONFIG::SOURCE_N_IDENTIFIER]) ;
  bool  is_stereo = bool( channel_store[CONFIG::STEREO_IDENTIFIER]) ;

DEBUG_TRACE_CONFIGURE_REMOTE_CHANNEL

  Identifier user_id     = user_store.getType() ;
  Identifier channel_id  = channel_store.getType() ;
  int        user_idx    = LinJam::GetRemoteUserIdx(user_id) ;
  int        channel_idx = LinJam::GetRemoteChannelIdx(user_idx , channel_id) ;
  if (!(~user_idx) || !(~channel_idx)) return ;

  Client->SetUserChannelState(user_idx                 , channel_idx           ,
                              ShouldSetIsRcv(a_key)    , is_rcv                ,
                              ShouldSetVolume(a_key)   , (float)DB2VAL(volume) ,
                              ShouldSetPan(a_key)      , pan                   ,
                              ShouldSetIsMuted(a_key)  , is_muted              ,
                              ShouldSetIsSolo(a_key)   , is_solo               ,
                              ShouldSetSinkN(a_key)    , sink_n                ,
                              ShouldSetIsStereo(a_key) , is_stereo             ) ;
}

bool LinJam::ShouldSetName(Identifier a_key)     { return false ; }

bool LinJam::ShouldSetVolume(Identifier a_key)   { return (a_key == CONFIG::VOLUME_IDENTIFIER) ; }

bool LinJam::ShouldSetPan(Identifier a_key)      { return (a_key == CONFIG::PAN_IDENTIFIER) ; }

bool LinJam::ShouldSetIsXmit(Identifier a_key)   { return (a_key == CONFIG::XMIT_IDENTIFIER) ; }

bool LinJam::ShouldSetIsRcv(Identifier a_key)   { return (a_key == CONFIG::XMIT_IDENTIFIER) ; }

bool LinJam::ShouldSetIsMuted(Identifier a_key)  { return (a_key == CONFIG::MUTE_IDENTIFIER) ; }

bool LinJam::ShouldSetIsSolo(Identifier a_key)   { return (a_key == CONFIG::SOLO_IDENTIFIER) ; }

bool LinJam::ShouldSetSourceN(Identifier a_key)  { return (a_key == CONFIG::SOURCE_N_IDENTIFIER) ; }

bool LinJam::ShouldSetSinkN(Identifier a_key)  { return (a_key == CONFIG::SOURCE_N_IDENTIFIER) ; }

bool LinJam::ShouldSetBitDepth(Identifier a_key) { return (false && a_key == CONFIG::SAMPLERATE_IDENTIFIER) ; }

bool LinJam::ShouldSetIsStereo(Identifier a_key) { return (a_key == CONFIG::STEREO_IDENTIFIER) ; }
