#pragma once

#include <QWidget>
#include <QMap>

class QVBoxLayout;

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Used to show a list of options, including searching functionality
    *        to filter them
    *
    * @author Daniel Konings
    */
    class SearcherWidget : public QWidget
    {

      Q_OBJECT;

    public:

      /**
      * @brief Construct a searcher widget in the application's main window
      */
      SearcherWidget();

      /**
      * @brief Adds an item that can be searched for in the searcher widget
      *        and sets an integer index for it
      *
      * @param[in] search_index The string to add as a search pattern
      *
      * @return The index that this search result will return when clicked
      */
      int AddItem(const QString& search_index);

      /**
      * @brief Shows the searcher widget, doing an initial update as if
      *        we're running without any search input
      */
      void Show();

      /**
      * @brief Closes this window and deletes it later
      */
      void CloseWindow();

    protected:

      /**
      * @brief Positions the window with tooltip-like behavior, making sure
      *        it always stays within the main window's bounds
      */
      void PositionWindow();

      /**
      * @brief Updates the searcher widget with a new search pattern
      *
      * @param[in] current The current search value, default = empty
      * @param[in] max_items The max items to show if we have a valid search,
      *                      default = SearcherWidget::kMaxItems_
      *
      * @remarks This does per-word matching, which is not ideal, after which
      *          it uses the fuzzy matching to determine the best search result
      */
      void Update(const QString& current = "", int max_items = kMaxItems_);

      /**
      * @brief Used to check whether the mouse is still inside the searcher
      *        rectangle, else we should close
      *
      * @remarks This happens in the case that the search box loses focus
      *          and puts focus on this widget again
      *
      * @param[in] evt The focus event
      */
      void focusInEvent(QFocusEvent* evt) override;

      /**
      * @brief Used to check whether the mouse is still inside the searcher
      *        rectangle, else we should close
      *
      * @param[in] evt The focus event
      */
      void focusOutEvent(QFocusEvent* evt) override;

      /**
      * @brief Closes this searcher window if not focussed
      */
      void CloseIfNotFocussed();

      /**
      * @brief Does fuzzy matching using Levenshtein distance to get a distance
      *        value to appropriately sort the search results with
      *
      * @param[in] a The first string
      * @param[in] b The second string
      *
      * @return The Levenshtein distance between the strings
      *
      * @remarks This matches everything in lowercase
      *
      * @see http://rosettacode.org/wiki/Levenshtein_distance#C.2B.2B
      */
      static int FuzzyMatch(const QString& a, const QString& b);

    signals:

      /**
      * @brief Emitted when an item has been selected
      *
      * @param[in] index The index of the selected item
      */
      void Selected(int index);

    private:

      QMap<QString, int> search_items_; //!< The items that can be searched for

      QWidget* results_; //!< The results frame
      QVBoxLayout* results_layout_; //!< The results layout of the widget

      static const QSize kSearcherSize_; //!< The size of the searcher dialog
      static const int kMaxItems_; //!< The maximum items to show after a search

      /**
      * @brief There can only be one instance of a searcher widget,
      *        keep a reference to close the other if it exists
      */
      static SearcherWidget* gOpenedSearcherWidget_;
    };
  }
}