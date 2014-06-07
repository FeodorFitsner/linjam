/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

// NOTE: arrange that "windows.h" be included before "JuceHeader.h" in all contexts
//         and arrange to include "JuceHeader.h" before any "*Component.h"
#include "LinJam.h" // includes "windows.h" and "JuceHeader.h"

#include "Constants.h"
#include "MainComponent.h"
#include "Trace.h"


//==============================================================================
class LinJamApplication : public JUCEApplication , public NJClient , MultiTimer
{
public:
    //==============================================================================
    LinJamApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName ; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString ; }
    bool moreThanOneInstanceAllowed() override       { return false ; }

    //==============================================================================
    void initialise (const String& args) override
    {
      this->mainWindow         = new MainWindow() ;
      this->contentComponent   = (MainContentComponent*)this->mainWindow      ->findChildWithID(GUI::CONTENT_GUI_ID) ;
      this->loginComponent     = (LoginComponent*)      this->contentComponent->findChildWithID(GUI::LOGIN_GUI_ID) ;
      this->licenseComponent   = (LicenseComponent*)    this->contentComponent->findChildWithID(GUI::LICENSE_GUI_ID) ;
      this->chatComponent      = (ChatComponent*)       this->contentComponent->findChildWithID(GUI::CHAT_GUI_ID) ;
      this->statusbarComponent = (StatusBarComponent*)  this->contentComponent->findChildWithID(GUI::STATUS_GUI_ID) ;

      if (!LinJam::Initialize(this , contentComponent , args)) initError() ;

      this->prev_status = NJClient::NJC_STATUS_PRECONNECT ;
      this->startTimer(CLIENT::CLIENT_DRIVER_ID , CLIENT::CLIENT_DRIVER_IVL) ;
//      this->startTimer(CLIENT::STATUS_POLL_ID ,   CLIENT::STATUS_POLL_IVL) ;
    }

    void initError()
    {
      this->statusbarComponent->setStatusL(GUI::AUDIO_INIT_ERROR_MSG.text) ;
    } // TODO: MB , prompt cfg ??

    void shutdown() override
    {
        // Add your application's shutdown code here..

        LinJam::Shutdown() ;
        this->mainWindow = nullptr ; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        this->quit() ;
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainContentComponent class.
    */
    class MainWindow    : public DocumentWindow
    {
    public:
        MainWindow()  : DocumentWindow (JUCEApplication::getInstance()->getApplicationName() ,
                                        Colours::lightgrey ,
                                        DocumentWindow::allButtons)
        {
            MainContentComponent* mainContentComponent = new MainContentComponent() ;
            setContentOwned(mainContentComponent , true) ;
            mainContentComponent->setComponentID(GUI::CONTENT_GUI_ID.text) ;
            centreWithSize(getWidth() , getHeight()) ;
            setVisible(true) ;
        }

        void closeButtonPressed()
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit() ;
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    void timerCallback(int timerId) override
    {
      switch (timerId)
      {
        case CLIENT::CLIENT_DRIVER_ID: driveClient() ;  break ;
//        case CLIENT::STATUS_POLL_ID:   handleStatus(status) ; break ;
        default:                                              break ;
      }
    }

    void driveClient()
    {
      int status = GetStatus() ;
      if (status != this->prev_status) handleStatus(this->prev_status = status) ;
      if (status < NJC_STATUS_OK || !this->Run()) return ;

//      while (this->Run()) ;
      if (status == NJC_STATUS_OK && HasUserInfoChanged()) ;//handleUserInfoChanged() ;// TODO: this fires repeatedly
    }

    void handleStatus(int status)
    {
//      if (status != this->prev_status) this->prev_status = status ; else return ;

//DEBUG_TRACE_CONNECT_STATUS // linux segfault

      // GUI state
      switch (status)
      {
        case NJC_STATUS_DISCONNECTED: this->loginComponent  ->toFront(true) ; break ;
        case NJC_STATUS_INVALIDAUTH:  (LinJam::IsAgreed)?
                                      this->loginComponent  ->toFront(true) :
                                      this->licenseComponent->toFront(true) ; break ;
        case NJC_STATUS_CANTCONNECT:  this->loginComponent  ->toFront(true) ; break ;
        case NJC_STATUS_OK:           this->chatComponent   ->toFront(true) ; break ;
        case NJC_STATUS_PRECONNECT:   this->loginComponent  ->toFront(true) ; break ;
        default:                                                              break ;
      }

      // status indicator
      String status_text ;
      switch (status)
      {
        case NJC_STATUS_DISCONNECTED:
          status_text = GUI::DISCONNECTED_STATUS_TEXT ;              break ;
        case NJC_STATUS_INVALIDAUTH:
          status_text = (LinJam::IsAgreed)? ((isRoomFull())?
                        GUI::ROOM_FULL_STATUS_TEXT :
                        GUI::INVALID_AUTH_STATUS_TEXT) :
                        GUI::PENDING_LICENSE_STATUS_TEXT ;           break ;
        case NJC_STATUS_CANTCONNECT:
          status_text = GUI::FAILED_CONNECTION_STATUS_TEXT ;         break ;
        case NJC_STATUS_OK:
#ifdef WIN32 // TODO: GetHostName() linux .so segfault (issue #15)
          status_text = GUI::CONNECTED_STATUS_TEXT + GetHostName() ; break ;
#else // WIN32
          status_text = GUI::CONNECTED_STATUS_TEXT ;                 break ;
#endif // WIN32
        case NJC_STATUS_PRECONNECT:
          status_text = GUI::IDLE_STATUS_TEXT ;                      break ;
        default:
          status_text = GUI::UNKNOWN_STATUS_TEXT + String(status) ;  break ;
      }
      this->statusbarComponent->setStatusL(status_text) ;
    }

    void handleUserInfoChanged()
    {
DEBUG_CHANNELS
    }


private:
    ScopedPointer<MainWindow> mainWindow ;
    MainContentComponent*     contentComponent ;
    LoginComponent*           loginComponent ;
    LicenseComponent*         licenseComponent ;
    ChatComponent*            chatComponent ;
    StatusBarComponent*       statusbarComponent ;

    int prev_status ;


    bool isRoomFull()
    {
#ifdef WIN32 // TODO: GetErrorStr() linux .so segfault (issue #15)
      String err = String(CharPointer_UTF8(GetErrorStr())) ;
      return (err.isNotEmpty() && !err.compare(CLIENT::SERVER_FULL_STATUS)) ;
#else // WIN32
      return true ;
/*
      if (GetErrorStr()[0]) // <-- segfault here
        return (!strcmp(GetErrorStr() , "room full")) ;
      else
        return false ;
*/
#endif
    }


/*
void rampEachRemoteUser()
{
  // remote users                                                              
  int user_n = -1 ; int ch_n = -1 ; int n_users = m_remoteusers.GetSize() ;           
  while (++user_n < n_users)                                                      
  {                                                                               
    float vol = 0.0f , pan = 0.0f ; bool mute = 0 ;                               
    String name = CharPointer_UTF8(GetUserState(user_n , &vol , &pan , &mute)) ;                     
    this->chatComponent->addChatLine("" , "") ;                                   
    this->chatComponent->addChatLine(                                             
        String("remote user ") + String(user_n) + String(":\n") ,                 
        String("name=")        + String(name)   +                                 
        String(" volume=")     + String(vol)    +                                 
        String(" pan=")        + String(pan)    +                                 
        String(" mute=")       + String(mute)                   ) ;               
    SetUserState(user_n , true , vol + 10.0f , true , 0.0f , true , false) ;

    // remote user channels                                                     
    int ch_idx ; ch_n = -1 ;                                                                   
    while ((ch_idx = EnumUserChannels(user_n , ++ch_n)) >= 0)                     
    {                                                                             
      float vol = 0.0f , pan = 0.0f ; bool sub = 0 , mute = 0 , solo = 0 ;        
      int out_ch = 0 ; bool stereo = 0 ;                                          
      String name = CharPointer_UTF8(GetUserChannelState(user_n , ch_idx  , &sub    ,               
                                                         &vol   , &pan    , &mute   ,               
                                                         &solo  , &out_ch , &stereo)) ;             
      this->chatComponent->addChatLine(                                           
          String("    remote channel ") + String(ch_n)     +                      
              String(" (")              + String(ch_idx)   + String("):\n") ,     
          String("    name=")           + String(name)     +                      
          String(" subscribed=")        + String(sub)      +                      
          String(" volume=")            + String(vol)      +                      
          String(" pan=")               + String(pan)      +                      
          String(" mute=")              + String(mute)     +                      
          String(" solo=")              + String(solo)     +                      
          String(" out_ch=")            + String(out_ch)   +                      
          String(" stereo=")            + String(stereo)                    ) ;   
      SetUserChannelState(user_n , ch_idx ,
			     true , true ,
			     true , vol + 10.0f ,
			     true , 0.0f ,
			     true , false ,
			     true , false ,
			     true , 0 ,
			     true , false) ;
    }
  }
}
*/
} ;

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (LinJamApplication)
