#if DEBUG

#include "Trace.h"


// enable debug features
// #define DEBUG_EXIT_IMMEDIAYELY
// #define DEBUG_AUTOLOGIN_CHANNEL "localhost:2049"
#define DEBUG_AUTOLOGIN_CHANNEL "ninbot.com:2052"
// #define DEBUG_AUTOLOGIN_CHANNEL "ninjamer.com:2051"



/* state */

#define DEBUG_TRACE_LINJAM_INIT Trace::TraceState("initializing") ;

#define DEBUG_TRACE_CONNECT                                                  \
  Trace::TraceState((!IsAgreed())? "connecting to " + host :                 \
                                   "joining "       + host + " as " + login) ;

#define DEBUG_TRACE_LICENSE                                          \
  Trace::TraceState((IsAgreed())? "agreeing to license" :            \
                                  "prompting for license agreement") ;

#ifdef CLEAN_SESSION
#  define DEBUG_TRACE_CLEAN_SESSION                                            \
  File thisfile     = File::getSpecialLocation(File::currentExecutableFile) ;  \
  File thisdir      = thisfile.getParentDirectory() ;                          \
  String sessiondir = SessionDir.getFullPathName() ;                           \
  if (!SessionDir.isDirectory() || !SessionDir.isAChildOf(thisdir))            \
      Trace::TraceError("session directory '" + sessiondir + "' is invalid") ; \
  else Trace::TraceState("cleaning session directory '" + sessiondir + "'") ;
#else // CLEAN_SESSION
#  define DEBUG_TRACE_CLEAN_SESSION                                               \
  String sessiondir = SessionDir.getFullPathName() ;                              \
  Trace::TraceState("cleaning session directory '" + sessiondir + "' (disabled)") ;
#endif // CLEAN_SESSION


/* audio */

#define DEBUG_TRACE_AUDIO_INIT_WIN                                                   \
  audioStreamer::Interface if_n = (audioStreamer::Interface)win_interface_n ;        \
  String type = (if_n == audioStreamer::WIN_AUDIO_ASIO)? CLIENT::ASIO_DEVICE_TYPE :  \
                (if_n == audioStreamer::WIN_AUDIO_KS)  ? CLIENT::KS_DEVICE_TYPE   :  \
                (if_n == audioStreamer::WIN_AUDIO_DS)  ? CLIENT::DS_DEVICE_TYPE   :  \
                (if_n == audioStreamer::WIN_AUDIO_WAVE)? CLIENT::WAVE_DEVICE_TYPE :  \
                                                         CLIENT::NFG_DEVICE_TYPE  ;  \
  String dbg = "initializing " + type + " audiostreamer =>" ;                        \
  switch ((int)if_n)                                                                 \
  {                                                                                  \
    case audioStreamer::WIN_AUDIO_ASIO: dbg = dbg                           +        \
          "\n\t" + CONFIG::ASIO_DRIVER_KEY  + " => " + String(asio_driver)  +        \
          "\n\t" + CONFIG::ASIO_INPUT0_KEY  + " => " + String(asio_input0)  +        \
          "\n\t" + CONFIG::ASIO_INPUT1_KEY  + " => " + String(asio_input1)  +        \
          "\n\t" + CONFIG::ASIO_OUTPUT0_KEY + " => " + String(asio_output0) +        \
          "\n\t" + CONFIG::ASIO_OUTPUT1_KEY + " => " + String(asio_output1) ;        \
    break ;                                                                          \
    case audioStreamer::WIN_AUDIO_KS: dbg = dbg                                +     \
          "\n\t" + CONFIG::KS_SAMPLERATE_KEY + " => " + String(ks_sample_rate) +     \
          "\n\t" + CONFIG::KS_BITDEPTH_KEY   + " => " + String(ks_bit_depth)   +     \
          "\n\t" + CONFIG::KS_NBLOCKS_KEY    + " => " + String(ks_n_buffers)   +     \
          "\n\t" + CONFIG::KS_BLOCKSIZE_KEY  + " => " + String(ks_buffer_size) ;     \
    break ;                                                                          \
    case audioStreamer::WIN_AUDIO_DS: dbg = dbg                                 +    \
          "\n\t" + CONFIG::DS_INPUT0_KEY     + " => " + String(ds_device[0][0]) +    \
          "\n\t" + CONFIG::DS_INPUT1_KEY     + " => " + String(ds_device[0][1]) +    \
          "\n\t" + CONFIG::DS_INPUT2_KEY     + " => " + String(ds_device[0][2]) +    \
          "\n\t" + CONFIG::DS_INPUT3_KEY     + " => " + String(ds_device[0][3]) +    \
          "\n\t" + CONFIG::DS_OUTPUT0_KEY    + " => " + String(ds_device[1][0]) +    \
          "\n\t" + CONFIG::DS_OUTPUT1_KEY    + " => " + String(ds_device[1][1]) +    \
          "\n\t" + CONFIG::DS_OUTPUT2_KEY    + " => " + String(ds_device[1][2]) +    \
          "\n\t" + CONFIG::DS_OUTPUT3_KEY    + " => " + String(ds_device[1][3]) +    \
          "\n\t" + CONFIG::DS_SAMPLERATE_KEY + " => " + String(ds_sample_rate)  +    \
          "\n\t" + CONFIG::DS_BITDEPTH_KEY   + " => " + String(ds_bit_depth)    +    \
          "\n\t" + CONFIG::DS_NBLOCKS_KEY    + " => " + String(ds_n_buffers)    +    \
          "\n\t" + CONFIG::DS_BLOCKSIZE_KEY  + " => " + String(ds_buffer_size)  ;    \
    break ;                                                                          \
    case audioStreamer::WIN_AUDIO_WAVE: dbg = dbg                                  + \
          "\n\t" + CONFIG::WAVE_INPUT_KEY      + " => " + String(wave_device[0])   + \
          "\n\t" + CONFIG::WAVE_OUTPUT_KEY     + " => " + String(wave_device[1])   + \
          "\n\t" + CONFIG::WAVE_SAMPLERATE_KEY + " => " + String(wave_sample_rate) + \
          "\n\t" + CONFIG::WAVE_BITDEPTH_KEY   + " => " + String(wave_bit_depth)   + \
          "\n\t" + CONFIG::WAVE_NBLOCKS_KEY    + " => " + String(wave_n_buffers)   + \
          "\n\t" + CONFIG::WAVE_BLOCKSIZE_KEY  + " => " + String(wave_buffer_size) ; \
    break ;                                                                          \
  }                                                                                  \
  Trace::TraceClient("detected platform is _WIN32") ;                                \
  if (TRACE_AUDIO_INIT_VB) Trace::TraceClient(dbg) ;

#define DEBUG_TRACE_AUDIO_INIT_MAC                                               \
  String type = CLIENT::CA_DEVICE_TYPE ;                                         \
  Trace::TraceClient("detected platform is _MAC") ;                              \
  if (TRACE_AUDIO_INIT_VB)                                                       \
    Trace::TraceClient("initializing "      + type   + " audiostreamer =>"     + \
        "\n\t" + CONFIG::MAC_DEVICE_KEY     + " => " + mac_device_name         + \
        "\n\t" + CONFIG::MAC_NINPUTS_KEY    + " => " + String(mac_n_inputs)    + \
        "\n\t" + CONFIG::MAC_SAMPLERATE_KEY + " => " + String(mac_sample_rate) + \
        "\n\t" + CONFIG::MAC_BITDEPTH_KEY   + " => " + String(mac_bit_depth)   ) ;

#define DEBUG_TRACE_AUDIO_INIT_NIX                                                   \
  audioStreamer::Interface if_n = (audioStreamer::Interface)nix_interface_n ;        \
  String type = (if_n == audioStreamer::NIX_AUDIO_JACK) ? CLIENT::JACK_DEVICE_TYPE : \
                (if_n == audioStreamer::NIX_AUDIO_ALSA) ? CLIENT::ALSA_DEVICE_TYPE : \
                                                          CLIENT::NFG_DEVICE_TYPE  ; \
  Trace::TraceClient("detected platform is *NIX") ;

#define DEBUG_TRACE_AUDIO_INIT_JACK                                                 \
  if (TRACE_AUDIO_INIT_VB)                                                          \
    Trace::TraceClient("initializing "     + type   + " audiostreamer =>"   +       \
        "\n\t" + CONFIG::JACK_NAME_KEY     + " => " + jack_name             +       \
        "\n\t" + CONFIG::JACK_NINPUTS_KEY  + " => " + String(jack_n_inputs) +       \
        "\n\t" + CONFIG::JACK_NOUTPUTS_KEY + " => " + String(jack_n_outputs)) ;     \
  if (!Audio) Trace::TraceState("could not connect to JACK - falling back to ALSA") ;

#define DEBUG_TRACE_AUDIO_INIT_ALSA                                     \
  if (TRACE_AUDIO_INIT_VB)                                              \
    Trace::TraceClient("initializing " + type   + " audiostreamer =>" + \
        "\n" + CONFIG::ALSA_CONFIG_KEY + " => " + alsa_config         ) ;

#define DEBUG_TRACE_AUDIO_INIT                                                           \
  if (!Audio) Trace::TraceError("error opening audio device using " + type) ;            \
  else Trace::TraceState("opened audio device using " + type     + " audiostreamer - " + \
                         String(Audio->m_srate)       + "Hz "                          + \
                         String(Audio->m_bps)         + "bps "                         + \
                         String(Audio->m_innch)       + "in -> "                       + \
                         String(Audio->m_outnch)      + "out "                         ) ;


/* network */

static int PrevStatus = -9 ;
#define DEBUG_TRACE_STATUS_CHANGED                                                      \
  int    prev_status   = PrevStatus ;                                                   \
  int    curr_status   = (PrevStatus = int(Status.getValue())) ;                        \
  String prev_msg      = Trace::Status2String(prev_status) ;                            \
  String curr_msg      = Trace::Status2String(curr_status) ;                            \
  String host_name     = Client->GetHostName() ;                                        \
  char*  client_error  = Client->GetErrorStr() ;                                        \
  String changed_msg   = prev_msg + " -> " + curr_msg ;                                 \
  String connected_msg = "connected to host: " + host_name ;                            \
  String error_msg     = "Error: " + CharPointer_UTF8(client_error) ;                   \
  if      (curr_status == LINJAM_STATUS_AUDIOERROR) Trace::TraceClient(curr_msg) ;      \
  else if (prev_status == LINJAM_STATUS_AUDIOERROR) Trace::TraceState(curr_msg) ;       \
  else                                              Trace::TraceState(changed_msg) ;    \
  if      (curr_status == NJClient::NJC_STATUS_OK)  Trace::TraceServer(connected_msg) ; \
  if      (client_error[0])                         Trace::TraceServer(error_msg) ;


/* channels */

#define DEBUG_TRACE_INITIAL_CHANNELS                                               \
  int n_chs = Config->localChannels.getNumChildren() ;                             \
  if (!n_chs) Trace::TraceConfig("no stored channels found") ;                     \
  else Trace::TraceConfig("restoring " + String(n_chs) + " stored local channels") ;

#define DEBUG_TRACE_ADD_LOCAL_CHANNEL                                                    \
  String channel_name = channel_store[CONFIG::CHANNEL_NAME_ID].toString() ;              \
  int    ch_idx       = int(channel_store[CONFIG::CHANNEL_IDX_ID]) ;                     \
  int    source       = int(channel_store[CONFIG::SOURCE_N_ID]) ;                        \
  bool   stereo       = int(channel_store[CONFIG::STEREO_ID]) != CONFIG::MONO ;          \
  String type         = (!stereo)? "mono" : "stereo" ;                                   \
  bool   is_new       = ch_idx == CONFIG::DEFAULT_CHANNEL_IDX ;                          \
  bool   exists       = IsConfiguredChannel(ch_idx) ;                                    \
  bool   valid_source = source >= 0 && source < GetNumAudioSources() ;                   \
  int    n_vacant     = GetNumVacantChannels() ;                                         \
  bool   no_free_chs  = (!stereo && n_vacant < 1 || stereo && n_vacant < 2) ;            \
  String dbg          = "adding " + String((is_new)? "new" : "stored") + " local " +     \
                        type + " input[" + String(source) + "] '" + channel_name + "'" ; \
  if (exists)                                                                            \
    Trace::TraceError("channel already exists " + dbg) ;                                 \
  else if (!valid_source)                                                                \
    Trace::TraceError("non-existent source " + dbg) ;                                    \
  else if (no_free_chs)                                                                  \
  {                                                                                      \
    Trace::TraceError("insufficient free channels " + dbg) ;                             \
    if (TRACE_LOCAL_CHANNELS_VB)                                                         \
      DBG(Trace::DumpStoredChannels() + Trace::DumpClientChannels()) ;                 \
  }                                                                                      \
  else Trace::TraceEvent(dbg) ;

#define DEBUG_TRACE_INSTANTIATE_LOCAL_CHANNEL                                    \
  Trace::TraceState("instantiating local " + type + " " +                        \
                    String(channel_store.getType()) + " '" + channel_name + "'") ;

#define DEBUG_TRACE_CONFIGURE_LOCAL_CHANNEL                                      \
  String ch_name   = channel_store[CONFIG::CHANNEL_NAME_ID].toString() ;         \
         ch_name   = Config->makeStereoName(ch_name , stereo_status) ;           \
  String ch_id     = String(channel_store.getType()) ;                           \
  bool   is_new    = !IsConfiguredChannel(channel_idx) ;                         \
  bool   is_stereo = stereo_status != CONFIG::MONO ;                             \
  String pairidx   = (stereo_status == CONFIG::MONO)                    ?        \
                     String(pair_idx) + ((!is_new)? " (deleting)" : "") :        \
                     (stereo_status == CONFIG::STEREO_L)                ?        \
                     String(pair_idx) + " (pending)"                    :        \
                     String(pair_idx)                                   ;        \
  String type      = (!is_stereo)   ? "mono" : "stereo" ;                        \
  String prev_type = (is_stereo)    ? "mono" : "stereo" ;                        \
  String dbg       = String((is_new)? "new"  : "existing") + " local " +         \
                     type + " " + ch_id + " '" + ch_name + "'" ;                 \
  String pann   = String(pan) + ((stereo_status == CONFIG::MONO)       ? "" :    \
                  " (" + String(ComputeStereoPan(pan , stereo_status)) +         \
                  " faux-stereo)") ;                                             \
  String stereo = String(is_stereo)                                    +         \
                  ((stereo_status == CONFIG::MONO)    ? " (MONO)"      :         \
                   (stereo_status == CONFIG::STEREO_L)? " (STEREO_L)" :          \
                   (stereo_status == CONFIG::STEREO_R)? " (STEREO_R)" : "") ;    \
  if (stereo_status == CONFIG::STEREO_L) dbg += " and its stereo pair" ;         \
  if (TRACE_LOCAL_CHANNELS_VB)  dbg +=                                           \
    ((should_set_name)?      "\n  name        => " + ch_name             : "") + \
    ((should_init_all)?      "\n  channel_idx => " + String(channel_idx) : "") + \
    ((should_set_stereo)?    "\n  pair_idx    => " + pairidx             : "") + \
    ((should_set_volume)?    "\n  volume      => " + String(volume)      : "") + \
    ((should_set_pan)?       "\n  pan         => " + pann                : "") + \
    ((should_set_is_xmit)?   "\n  is_xmit     => " + String(is_xmit)     : "") + \
    ((should_set_is_muted)?  "\n  is_muted    => " + String(is_muted)    : "") + \
    ((should_set_is_solo)?   "\n  is_solo     => " + String(is_solo)     : "") + \
    ((should_set_source_n)?  "\n  source_n    => " + String(source_n)    : "") + \
    ((should_set_bit_depth)? "\n  bit_depth   => " + String(bit_depth)   : "") + \
    ((should_set_stereo)?    "\n  is_stereo   => " + stereo              : "") ; \
  if (should_set_stereo && !should_init_all)                                     \
    Trace::TraceClient("converting existing local " + prev_type + " " +          \
                       ch_id + " '" + ch_name + "' to " + type) ;                \
  Trace::TraceClient("configuring " + dbg) ;

#define DEBUG_TRACE_REMOVE_LOCAL_CHANNEL                                       \
  String type = (int(channel_store[CONFIG::STEREO_ID]) == CONFIG::MONO)?       \
                "mono" : "stereo" ;                                            \
  Trace::TraceEvent("destroying local " + type + " channel["                 + \
                    channel_store[CONFIG::CHANNEL_IDX_ID].toString() + "] '" + \
                    String(channel_store.getType()) + "'"                    ) ;

#if TRACE_DUMP_FREE_INPUTS
#  define DEBUG_TRACE_DUMP_FREE_INPUTS_VB                                            \
  SortedSet<int> monos   = FreeAudioSources ;     int n_monos   = monos.size() ;     \
  SortedSet<int> stereos = FreeAudioSourcePairs ; int n_stereos = stereos.size() ;   \
  String dump = String(n_monos)   + " FreeAudioSources     = [" ;                    \
  for (int i = 0 ; i < n_monos ; ++i)   dump += String(monos[i])   + " " ;           \
  Trace::TraceVerbose(dump.trim() + "]") ;                                           \
  dump        = String(n_stereos) + " FreeAudioSourcePairs = [" ;                    \
  for (int i = 0 ; i < n_stereos ; ++i) dump += String(stereos[i]) + " " ;           \
  Trace::TraceVerbose(dump.trim() + "]") ;
#else // TRACE_DUMP_FREE_INPUTS
#  define DEBUG_TRACE_DUMP_FREE_INPUTS_VB ;
#endif // TRACE_DUMP_FREE_INPUTS

#define DEBUG_TRACE_REMOTE_CHANNELS                                           \
    String hidden  = (hide_bots && NETWORK::KNOWN_BOTS.contains(u_id))?       \
                     " (bot hidden)" : "" ;                                   \
    String dbg = "NJClient remote user[" + String(u_idx) + "] =>" + hidden +  \
        "\n  user_name   => "   + String(u_name)                  +           \
        "\n  user_volume => "   + String(u_vol)                   +           \
        "\n  user_pan    => "   + String(u_pan)                   +           \
        "\n  user_mute   => "   + String(u_mute) ;                            \
    int ch_n = -1 ; int ch_idx ;                                              \
    while (~(ch_idx = LinJam::Client->EnumUserChannels(u_idx , ++ch_n)))      \
    {                                                                         \
      bool ch_rcv ;  float ch_vol ;  float ch_pan ; bool ch_mute ;            \
      bool ch_solo ; int   ch_sink ; bool  ch_stereo ;                        \
      String ch_name = LinJam::GetRemoteChannelClientName(u_idx , ch_idx) ;   \
      LinJam::Client->GetUserChannelState(u_idx    , ch_idx   , &ch_rcv   ,   \
                                          &ch_vol  , &ch_pan  , &ch_mute  ,   \
                                          &ch_solo , &ch_sink , &ch_stereo) ; \
      dbg += "\n  found remote channel[" + String(ch_n)   + "] =>" +          \
             "\n    channel_idx    => "  + String(ch_idx)          +          \
             "\n    channel_name   => "  + String(ch_name)         +          \
             "\n    channel_volume => "  + String(ch_vol)          +          \
             "\n    channel_pan    => "  + String(ch_pan)          +          \
             "\n    is_rcv         => "  + String(ch_rcv)          +          \
             "\n    channel_mute   => "  + String(ch_mute)         +          \
             "\n    is_solo        => "  + String(ch_solo)         +          \
             "\n    sink_n         => "  + String(ch_sink)         +          \
             "\n    is_stereo      => "  + String(ch_stereo) ;                \
    }                                                                         \
    Trace::TraceState(dbg) ;

#if TRACE_REMOTE_CHANNELS_VB
#  define DEBUG_TRACE_REMOTE_CHANNELS_VB                                          \
  bool has_bot   = NETWORK::KNOWN_HOSTS.contains(String(Client->GetHostName())) ; \
  bool hide_bots = has_bot && bool(Config->shouldHideBots.getValue()) ;           \
  Trace::TraceServer("user info changed - " +                                     \
                     String(Client->GetNumUsers()) + " users") ;                  \
  int u_idx = -1 ; String u_name ; float u_vol ; float u_pan ; bool u_mute ;      \
  while ((u_name = GetRemoteUserName(++u_idx)).isNotEmpty())                      \
  {                                                                               \
    Client->GetUserState(u_idx , &u_vol , &u_pan , &u_mute) ;                     \
    Identifier u_id = Config->encodeUserId(String(u_name) , u_idx) ;              \
    DEBUG_TRACE_REMOTE_CHANNELS                                                   \
  }
#else // TRACE_REMOTE_CHANNELS_VB
#  define DEBUG_TRACE_REMOTE_CHANNELS_VB ;
#endif // TRACE_REMOTE_CHANNELS_VB

#define DEBUG_TRACE_CONFIGURE_REMOTE_CHANNEL                                          \
  String user_name     = String(user_store.getType()) ;                               \
  String dbg           = "configuring remote " + String(channel_store.getType())  +   \
                         " '" + channel_store[CONFIG::CHANNEL_NAME_ID].toString() +   \
                         "' for user[" + String(user_idx) + "] '" + user_name + "'" ; \
  String pann          = String(pan) + ((stereo_status == CONFIG::MONO)? "" :         \
                         " (" + String(ComputeStereoPan(pan , stereo_status)) +       \
                         " faux-stereo)") ;                                           \
  String stereo        = String(is_stereo)                                   +        \
                         ((stereo_status == CONFIG::MONO)    ? " (MONO)"     :        \
                          (stereo_status == CONFIG::STEREO_L)? " (STEREO_L)" :        \
                          (stereo_status == CONFIG::STEREO_R)? " (STEREO_R)" : "") ;  \
 String pseudo_control = (should_set_is_rcv) ? "RCV"  :                               \
                         (should_set_is_solo)? "SOLO" : "" ;                          \
  bool is_master = (channel_idx == CONFIG::MASTER_CHANNEL_IDX) ;                      \
  if (!is_master)                                                                     \
  {                                                                                   \
    if (stereo_status == CONFIG::STEREO_L) dbg += " and its stereo pair" ;            \
    ValueTree store     = Config->getUserMasterChannel(user_store) ;                  \
    bool master_rcv_or  = !bool(store[CONFIG::IS_XMIT_RCV_ID]) ;                      \
    bool master_solo_or = bool(store[CONFIG::IS_SOLO_ID]) ;                           \
    is_rcv              = is_rcv  && !master_rcv_or ;                                 \
    is_solo             = is_solo || master_solo_or ;                                 \
    if (master_rcv_or)  dbg += " (master rcv override)" ;                             \
    if (master_solo_or) dbg += " (master solo override)" ;                            \
  }                                                                                   \
  if      (a_key == CONFIG::STEREO_ID && stereo_status == CONFIG::STEREO_R) ;         \
  else if (!(~user_idx))    Trace::TraceError("user index out of range "    + dbg) ;  \
  else if (!(~channel_idx)) Trace::TraceError("channel index out of range " + dbg) ;  \
  else if (TRACE_REMOTE_CHANNELS_VB) Trace::TraceClient(dbg                 +         \
      ((should_set_volume)?   "\n  volume    => " + String(volume)    : "") +         \
      ((should_set_pan)?      "\n  pan       => " + pann              : "") +         \
      ((should_set_is_rcv)?   "\n  is_rcv    => " + String(is_rcv)    : "") +         \
      ((should_set_is_muted)? "\n  is_muted  => " + String(is_muted)  : "") +         \
      ((should_set_is_solo)?  "\n  is_solo   => " + String(is_solo)   : "") +         \
      ((should_init_all)?     "\n  sink_n    => " + String(sink_n)    : "") +         \
      ((should_init_all)?     "\n  is_stereo => " + stereo            : "") ) ;       \
  if (is_master && !should_set_volume && !should_set_pan && !should_set_is_muted &&   \
     (should_set_is_rcv || should_set_is_solo))                                       \
    Trace::TraceClient("applying user master " + pseudo_control               +       \
                       " pseudo control over all " + user_name + " channels") ;


/* subscriptions */

#define DEBUG_TRACE_SUBSCRIPTIONS                                                     \
  ValueTree             subs  = Config->subscriptions.createCopy() ;                  \
  std::set<std::string> users = Client->config_autosubscribe_userlist ;               \
  for (int user_n = 0 ; user_n < subs.getNumChildren() ; ++user_n)                    \
  {                                                                                   \
    String user_name = String(subs.getChild(user_n).getType()) ;                      \
    if (!users.count(user_name.toStdString()))                                        \
      Trace::TraceClient("ignoring remote user '" + user_name + "'") ;                \
  }                                                                                   \
  for (std::set<std::string>::iterator it = users.begin() ; it != users.end() ; ++it) \
  {                                                                                   \
    String user_name = String(*it) ;                                                  \
    if (subs.getChildWithName(Identifier(user_name)).isValid())                       \
      Trace::TraceClient("subscribing to remote user '" + user_name + "'") ;          \
  }                                                                                   \
  if (TRACE_DUMP_SUBSCRIPTIONS)                                                       \
  {                                                                                   \
    String dbg = "ignore_list =>" ; int n_users = subs.getNumChildren() ;             \
    for (int user_n = 0 ; user_n < n_users ; ++user_n)                                \
      dbg += "\n  " + String(subs.getChild(user_n).getType()) ;                       \
    Trace::TraceClient(dbg + ((!n_users)? " (none)" : "")) ;                          \
  }


/* chat */

#define DEBUG_TRACE_CHAT_IN                                                        \
  if (chat_user.compare(Config->server[CONFIG::LOGIN_ID].toString()))              \
    Trace::TraceEvent("incoming chat: " + String(parms[CLIENT::CHATMSG_TYPE_IDX])) ;

#define DEBUG_TRACE_CHAT_OUT                                                    \
  if ((chat_text = chat_text.trim()).isNotEmpty())                              \
    Trace::TraceEvent("outgoing chat: " + ((chat_text[0] == '/')?               \
                      chat_text.upToFirstOccurrenceOf(" " , false , false) :    \
                      CLIENT::CHATMSG_TYPE_MSG)) ;

#else // DEBUG

// state
#define DEBUG_TRACE_LINJAM_INIT               ;
#define DEBUG_TRACE_CONNECT                   ;
#define DEBUG_TRACE_LICENSE                   ;
#define DEBUG_TRACE_CLEAN_SESSION             ;
// audio
#define DEBUG_TRACE_AUDIO_INIT_WIN            ;
#define DEBUG_TRACE_AUDIO_INIT_MAC            ;
#define DEBUG_TRACE_AUDIO_INIT_JACK           ;
#define DEBUG_TRACE_AUDIO_INIT_ALSA           ;
#define DEBUG_TRACE_AUDIO_INIT                ;
// network
#define DEBUG_TRACE_CONNECT_STATUS            ;
// channels
#define DEBUG_TRACE_INITIAL_CHANNELS          ;
#define DEBUG_TRACE_ADD_LOCAL_CHANNEL         ;
#define DEBUG_TRACE_INSTANTIATE_LOCAL_CHANNEL ;
#define DEBUG_TRACE_CONFIGURE_LOCAL_CHANNEL   ;
#define DEBUG_TRACE_REMOVE_LOCAL_CHANNEL      ;
#define DEBUG_TRACE_DUMP_FREE_INPUTS_VB       ;
#define DEBUG_TRACE_REMOTE_CHANNELS           ;
#define DEBUG_TRACE_REMOTE_CHANNELS_VB        ;
#define DEBUG_TRACE_CONFIGURE_REMOTE_CHANNEL  ;
// subscriptions
#define DEBUG_TRACE_SUBSCRIPTIONS             ;
// chat
#define DEBUG_TRACE_CHAT_IN                   ;
#define DEBUG_TRACE_CHAT_OUT                  ;

#endif // DEBUG
