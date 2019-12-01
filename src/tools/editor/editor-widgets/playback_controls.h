#pragma once

#include <QWidget>

class QPushButton;

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief A view that contains play, pause, stop and step-through-frames
    *        buttons.
    *
    * @author Daniel Konings
    */
    class PlaybackControls : public QWidget
    {

      Q_OBJECT;

    private:

      /**
      * @brief The different playback buttons that exist
      */
      enum PlaybackButton : int
      {
        kNone = -1,
        kFirst = 0,
        kPlay = kFirst,
        kStop,
        kNextFrame,
        kCount,
        kIncludingPause,
        kPause = kCount
      };

    public:

      /**
      * @brief Construct through a parent
      *
      * @param[in] parent The parent widget of the controls, default = nullptr
      */
      PlaybackControls(QWidget* parent = nullptr);

      /**
      * @brief Sets whether the playback controls are enabled. This is useful
      *        for disabling the playback controls when the builder is still
      *        building items.
      *
      * @param[in] enabled Should the playback controls be enabled?
      *
      * @author Daniel Konings
      */
      void SetEnabled(bool enabled);

    protected:

      /**
      * @brief Retrieves the text of a specific playback button
      *
      * @param[in] button The button to retrieve the text for
      *
      * @return The button text
      */
      static const wchar_t* GetButtonText(PlaybackButton button);

      /**
      * @brief Toggles the play button between play and pause
      *
      * @param[in] state Optional value to force a state,
      *                  default = PlaybackButton::kCount
      *
      * @remarks The only valid arguments for state are
      *          PlaybackButton::kPause and PlaybackButton::kPlay
      */
      void TogglePlayButton(PlaybackButton state = PlaybackButton::kNone);

    private:

      /**
      * @brief All buttons that are contained within the controls
      */
      QPushButton* buttons_[PlaybackButton::kCount];

      bool play_toggled_; //!< Is the play button toggled to a pause button?

      static const int kButtonSize_; //!< The size of each individual button
      static const int kFontSize_; //!< The font size in buttons
      static const int kPauseFontSize_; //!< The font size for the pause button
    };
  }
}