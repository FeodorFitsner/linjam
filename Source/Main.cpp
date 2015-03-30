/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/


// NOTE: in order to avoid "multiple definitions of WinMain()" compiler error
//         arrange that "windows.h" be included before "JuceHeader.h" in all contexts
//       also arrange to include "JuceHeader.h" before any "*Component.h"
#include "LinJam.h" // includes "windows.h" and "JuceHeader.h"


class LinJamApplication : public JUCEApplication , NJClient , MultiTimer
{
public:

  LinJamApplication() {}

  void initialise(const String& command_line) override
  {
    this->mainWindow          = new MainWindow() ;
    MainContent* main_content = (MainContent*)this->mainWindow->mainContent ;

    if (!LinJam::Initialize(this , main_content , this , command_line))
    {
      LinJam::Shutdown() ; quit() ;
    }
  }

  void anotherInstanceStarted(const String& command_line) override
  {
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
    LinJam::Shutdown() ; quit() ;
  }

  void shutdown() override
  {
    LinJam::Shutdown() ;

    this->mainWindow = nullptr ;
  }

  void         systemRequestedQuit()        override { this->quit() ; }
  const String getApplicationName()         override { return ProjectInfo::projectName ; }
  const String getApplicationVersion()      override { return ProjectInfo::versionString ; }
  bool         moreThanOneInstanceAllowed() override { return false ; }


  /*
      This class implements the desktop window that contains an instance of
      our MainContentComponent class.
  */
  class MainWindow : public DocumentWindow
  {
    friend class LinJamApplication ;


  public:

    MainWindow() : DocumentWindow(GUI::APP_NAME             ,
                                  Colour(0xff202020)        ,
                                  DocumentWindow::allButtons)
    {
      // config button (managed and handled by MainContent)
      this->configButton = new TextButton("configButton") ;
      Component::addAndMakeVisible(this->configButton) ;

      // main content (title managed by MainContent)
      this->mainContent = new MainContent(this , this->configButton) ;
      this->mainContent->setComponentID(GUI::CONTENT_GUI_ID) ;
      setContentOwned(this->mainContent , true) ;

      // this main desktop window
#ifdef _MAC
      setTitleBarButtonsRequired(DocumentWindow::allButtons , true) ;
#endif // _MAC
      setTitleBarHeight(GUI::TITLEBAR_H) ;
//      setIcon(const Image &imageToUse) ;
      centreWithSize(getWidth() , getHeight()) ;
      setVisible(true) ;
    }

    ~MainWindow()
    {
      this->mainContent  = nullptr ;
      this->configButton = nullptr ;
    }

    void closeButtonPressed() { JUCEApplication::getInstance()->systemRequestedQuit() ; }


  private:

    ScopedPointer<MainContent> mainContent ;
    ScopedPointer<TextButton>  configButton ;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
  } ;


private:

  ScopedPointer<MainWindow> mainWindow ;


  void timerCallback(int timer_id) override { LinJam::HandleTimer(timer_id) ; }


  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LinJamApplication)
} ;


START_JUCE_APPLICATION(LinJamApplication)
