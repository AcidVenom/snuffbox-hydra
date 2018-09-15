#pragma once

#include <QObject>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Used to store session data of the current project opened in the
    *        editor
    *
    * @author Daniel Konings
    */
    class Project : public QObject
    {
      
      Q_OBJECT;

    public:

      /**
      * @brief Construct through a QObject parent
      *
      * @param[in] parent The parent that contains this QObject
      */
      Project(QObject* parent = nullptr);
    };
  }
}