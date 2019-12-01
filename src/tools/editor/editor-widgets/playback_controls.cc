#include "tools/editor/editor-widgets/playback_controls.h"

#include "tools/editor/application/styling.h"

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
      play_toggled_(false)
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

      connect(buttons_[PlaybackButton::kPlay], &QPushButton::released, this, [=]()
      {
        TogglePlayButton();
      });
    }

    //--------------------------------------------------------------------------
    void PlaybackControls::SetEnabled(bool enabled)
    {

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

        style = "background: " + 
          Styling::GetStyleColorCSS(Styling::ColorRole::kBlueButton);
      }
      else
      {
        text = GetButtonText(PlaybackButton::kPlay);
        font_size = kFontSize_;

        style = "";
      }

      QFont font;
      font.setPointSize(font_size);

      play_button->setText(QString::fromWCharArray(text));
      play_button->setFont(font);
      play_button->setStyleSheet(style);
    }
  }
}