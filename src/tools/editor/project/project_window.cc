#include "tools/editor/project/project_window.h"
#include "tools/editor/application/editor_application.h"

#include <QLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QSettings>
#include <QFileDialog>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int ProjectWindow::kProjectWindowWidth_ = 640;
    const int ProjectWindow::kProjectWindowHeight_ = 320;
    const QString ProjectWindow::kSettingsRecentFolder_ = "Project.Recent";

    //--------------------------------------------------------------------------
    ProjectWindow::ProjectWindow(Project* project, QWidget* parent) :
      QDialog(parent),
      project_(project),
      recent_folder_("")
    {
      setObjectName(QStringLiteral("ProjectWindow"));

      int w = kProjectWindowWidth_;
      int h = kProjectWindowHeight_;

      setMinimumSize(w, h);
      setMaximumSize(w, h);

      QVBoxLayout* main_layout = new QVBoxLayout(this);
      main_layout->setObjectName(QStringLiteral("MainProjectLayout"));
      main_layout->setStretch(0, 1);

      QVBoxLayout* recent_layout = new QVBoxLayout(main_layout->widget());
      recent_layout->setObjectName(QStringLiteral("RecentProjectLayout"));
      main_layout->addLayout(recent_layout);

      QLabel* recent_label = 
        new QLabel(QStringLiteral("Recent projects"), recent_layout->widget());

      recent_layout->addWidget(recent_label);

      QListWidget* recent_projects = new QListWidget(recent_layout->widget());
      recent_projects->setObjectName(QStringLiteral("RecentProjectsWidget"));

      QHBoxLayout* buttons_layout = new QHBoxLayout(main_layout->widget());
      buttons_layout->setObjectName(QStringLiteral("ProjectButtonsLayout"));

      QPushButton* button_new = 
        new QPushButton(QStringLiteral("New"), buttons_layout->widget());
      button_new->setObjectName(QStringLiteral("ProjectButtonNew"));

      QPushButton* button_open = 
        new QPushButton(QStringLiteral("Open"), buttons_layout->widget());
      button_open->setObjectName(QStringLiteral("ProjectButtonOpen"));

      QPushButton* button_cancel = 
        new QPushButton(QStringLiteral("Cancel"), buttons_layout->widget());
      button_new->setObjectName(QStringLiteral("ProjectButtonCancel"));

      buttons_layout->addWidget(button_new);
      buttons_layout->addWidget(button_open);
      buttons_layout->addWidget(button_cancel);

      main_layout->addLayout(buttons_layout);

      recent_layout->addWidget(recent_projects);

      connect(
        button_new, &QPushButton::clicked, this, &ProjectWindow::NewProject);

      connect(
        button_open, &QPushButton::clicked, this, &ProjectWindow::OpenProject);

      connect(button_cancel, &QPushButton::clicked, this, &QDialog::reject);

      QSettings& settings = EditorApplication::Instance()->GlobalSettings();
      recent_folder_ = settings.value(kSettingsRecentFolder_, "").toString();
    }

    //--------------------------------------------------------------------------
    void ProjectWindow::NewProject()
    {
      
    }

    //--------------------------------------------------------------------------
    void ProjectWindow::OpenProject()
    {
      bool valid;
      QString dir = ShowFolderDialog(&valid);
    }
    
    //--------------------------------------------------------------------------
    QString ProjectWindow::ShowFolderDialog(bool* valid)
    {
      bool is_valid = true;
      QString dir = QFileDialog::getExistingDirectory(
        this, 
        QStringLiteral("Open project"),
        recent_folder_,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

      if (dir.isEmpty() == true)
      {
        is_valid = false;
      }
      else
      {
        recent_folder_ = dir;
        QSettings& settings = EditorApplication::Instance()->GlobalSettings();
        settings.setValue(kSettingsRecentFolder_, recent_folder_);
      }

      if (valid != nullptr)
      {
        *valid = is_valid;
      }

      return dir;
    }
  }
}