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
      * @brief Used to mask out different playback functionality
      */
      enum PlaybackFlags : int
      {
        kPlayButton = 0 << 1,
        kStopButton = 1 << 1,
        kNextButton = 2 << 1
      };

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
      * @param[in] flags The mask to enable buttons with
      *
      * @see PlaybackControls::PlaybackFlags
      *
      * @author Daniel Konings
      */
      void SetEnabled(int flags);

      /**
      * @brief Sets if all playback should be disabled, disregarding flags
      *        that are currently set
      *
      * @param[in] disabled Should all controls be disabled?
      */
      void DisableAll(bool disabled);

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

      /**
      * @brief Updates the style sheets of every button
      */
      void UpdateStyleSheets();

    private:

      /**
      * @brief All buttons that are contained within the controls
      */
      QPushButton* buttons_[PlaybackButton::kCount];

      bool disabled_; //!< Are playback controls disabled?
      bool play_toggled_; //!< Is the play button toggled to a pause button?
      int enable_flags_; //!< The flags for which buttons are enabled

      static const int kButtonSize_; //!< The size of each individual button
      static const int kFontSize_; //!< The font size in buttons
      static const int kPauseFontSize_; //!< The font size for the pause button
    };
  }
}