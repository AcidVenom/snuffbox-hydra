#include "tools/editor/editor-widgets/playback_controls.h"

#include "tools/editor/application/styling.h"
#include "tools/editor/application/editor_application.h"

#include <foundation/containers/function.h>

#include <QHBoxLayout>
#include <QPushButton>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int PlaybackControls::kButtonSize_ = 30;
    const int PlaybackControls::kFontSize_ = 16;
    const int PlaybackControls::kPauseFontSize_ = 8;

    //--------------------------------------------------------------------------
    PlaybackControls::PlaybackControls(QWidget* parent) :
      QWidget(parent),
      disabled_(false),
      play_toggled_(false),
      enable_flags_(0)
    {
      memset(buttons_, 0, sizeof(QPushButton*) * PlaybackButton::kCount);

      setObjectName(QStringLiteral("PlaybackControls"));

      QHBoxLayout* main_layout = new QHBoxLayout(this);
      main_layout->setObjectName(QStringLiteral("PlaybackLayout"));
      main_layout->setContentsMargins(0, 0, 0, 0);
      main_layout->setSpacing(0);

      QFont button_font;
      button_font.setPointSize(kFontSize_);

      main_layout->addStretch();
      for (
        PlaybackButton i = PlaybackButton::kFirst; 
        i != PlaybackButton::kCount; 
        i = static_cast<PlaybackButton>(static_cast<int>(i + 1)))
      {
        QPushButton* button = new QPushButton(this);
        button->setText(QString::fromWCharArray(GetButtonText(i)));
        button->setFont(button_font);
        button->setFixedSize(kButtonSize_, kButtonSize_);

        main_layout->addWidget(button);

        buttons_[i] = button;
      }
      main_layout->addStretch();

      setLayout(main_layout);
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

      SetEnabled(PlaybackFlags::kPlayButton);

      auto ConnectButton = [&](
        PlaybackButton button, 
        const foundation::Function<void()>& cb)
      {
        connect(buttons_[button], &QPushButton::released, this, cb);
      };

      ConnectButton(PlaybackButton::kPlay, [=]()
      {
        EditorApplication* app = EditorApplication::Instance();
        EditorApplication::EditorStates desired =
          app->state() == EditorApplication::EditorStates::kPlaying ?
          EditorApplication::EditorStates::kPaused :
          EditorApplication::EditorStates::kPlaying;
          
        app->SwitchState(desired);

        TogglePlayButton();

        int mask = PlaybackFlags::kPlayButton | PlaybackFlags::kStopButton;

        if (desired == EditorApplication::EditorStates::kPaused)
        {
          mask |= PlaybackFlags::kNextButton;
        }

        SetEnabled(mask);
      });

      ConnectButton(PlaybackButton::kStop, [=]()
      {
        EditorApplication::Instance()->SwitchState(
          EditorApplication::EditorStates::kEditing);

        TogglePlayButton(PlaybackButton::kPlay);
        SetEnabled(PlaybackFlags::kPlayButton);
      });

      ConnectButton(PlaybackButton::kNextFrame, [=]()
      {
        EditorApplication::Instance()->SwitchState(
          EditorApplication::EditorStates::kFrame);
      });
    }

    //--------------------------------------------------------------------------
    void PlaybackControls::SetEnabled(int flags)
    {
      bool play_enabled = 
        (flags & PlaybackFlags::kPlayButton) == PlaybackFlags::kPlayButton;

      bool stop_enabled =
        (flags & PlaybackFlags::kStopButton) == PlaybackFlags::kStopButton;

      bool next_frame_enabled =
        (flags & PlaybackFlags::kNextButton) == PlaybackFlags::kNextButton;

      if (disabled_ == true)
      {
        play_enabled = stop_enabled = next_frame_enabled = false;
      }

      buttons_[PlaybackButton::kPlay]->setEnabled(play_enabled);
      buttons_[PlaybackButton::kStop]->setEnabled(stop_enabled);
      buttons_[PlaybackButton::kNextFrame]->setEnabled(next_frame_enabled);

      enable_flags_ = flags;

      UpdateStyleSheets();
    }

    //--------------------------------------------------------------------------
    void PlaybackControls::DisableAll(bool disabled)
    {
      if (
        disabled == disabled_ || 
        EditorApplication::Instance()->state() != 
        EditorApplication::EditorStates::kEditing)
      {
        return;
      }

      disabled_ = disabled;
      SetEnabled(enable_flags_);
    }

    //--------------------------------------------------------------------------
    const wchar_t* PlaybackControls::GetButtonText(PlaybackButton button)
    {
      static const wchar_t* button_texts[PlaybackButton::kIncludingPause];
      static bool initialized = false;

      if (initialized == false)
      {
        button_texts[PlaybackButton::kPlay] = L"\u25B6";
        button_texts[PlaybackButton::kStop] = L"\u2BC0";
        button_texts[PlaybackButton::kNextFrame] = L"\u2B72";
        button_texts[PlaybackButton::kPause] = L"\u2590\u0020\u258C";

        initialized = true;
      }

      return button_texts[button];
    }

    //--------------------------------------------------------------------------
    void PlaybackControls::TogglePlayButton(PlaybackButton state)
    {
      bool toggled = play_toggled_ == false;

      if (state == PlaybackButton::kPause || state == PlaybackButton::kPlay)
      {
        toggled = state != PlaybackButton::kPlay;
      }

      QPushButton* play_button = buttons_[PlaybackButton::kPlay];
      play_toggled_ = toggled;

      const wchar_t* text = nullptr;
      int font_size = -1;
      QString style;

      if (play_toggled_ == true)
      {
        text = GetButtonText(PlaybackButton::kPause);
        font_size = kPauseFontSize_;
      }
      else
      {
        text = GetButtonText(PlaybackButton::kPlay);
        font_size = kFontSize_;
      }

      QFont font;
      font.setPointSize(font_size);

      play_button->setText(QString::fromWCharArray(text));
      play_button->setFont(font);

      UpdateStyleSheets();
    }

    //--------------------------------------------------------------------------
    void PlaybackControls::UpdateStyleSheets()
    {
      EditorApplication* app = EditorApplication::Instance();

      QString on_style = "background: %0;";
      on_style = on_style.arg(
        Styling::GetStyleColorCSS(Styling::ColorRole::kBlueButton));

      QString off_style = "";

      QColor disabled_col = Styling::GetStyleColor(Styling::ColorRole::kButton);
      disabled_col = disabled_col.dark(150);

      QColor disabled_text_col = disabled_col.light(200);

      QString disabled_style = "background: %0; color: %1;";
      disabled_style = disabled_style.arg(
        Styling::GetCSSColor(disabled_col),
        Styling::GetCSSColor(disabled_text_col));

      for (int i = 0; i < PlaybackButton::kCount; ++i)
      {
        QPushButton* button = buttons_[i];
        
        QString style = button->isEnabled() ? off_style : disabled_style;
        if (i == PlaybackButton::kPlay)
        {
          if (
            app->state() != EditorApplication::EditorStates::kEditing && 
            button->isEnabled())
          {
            style = on_style;
          }
        }

        button->setStyleSheet(style);
      }
    }
  }
}