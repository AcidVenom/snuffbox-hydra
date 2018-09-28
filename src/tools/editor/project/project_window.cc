#include "tools/editor/project/project_window.h"
#include "tools/editor/application/editor_application.h"
#include "tools/editor/windows/message_box.h"

#include <QLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QFlags>
#include <QLineEdit>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    class RecentProjectListWidgetItem : public QListWidgetItem
    {
    public:

      RecentProjectListWidgetItem(
        const QString& name,
        const QString& path, 
        QListWidget* view = nullptr) 
        :
        QListWidgetItem(view),
        path_(path)
      {
        if (view != nullptr)
        {
          QWidget* recent_project_widget = new QWidget(view);

          QVBoxLayout* layout = new QVBoxLayout(recent_project_widget);

          QLabel* name_widget = new QLabel(layout->widget());
          QLabel* path_widget = new QLabel(layout->widget());

          name_widget->setText(name);
          name_widget->setStyleSheet(
            "font-weight: bold; font-size: 14px;");

          path_widget->setText(path);

          layout->addWidget(name_widget);
          layout->addWidget(path_widget);

          recent_project_widget->setLayout(layout);

          view->setItemWidget(this, recent_project_widget);
          setSizeHint(QSize(256, 64));
        }
      }

      const QString& path() const
      {
        return path_;
      }

    private:

      QString path_;
    };

    //--------------------------------------------------------------------------
    const int ProjectWindow::kProjectWindowWidth_ = 640;
    const int ProjectWindow::kProjectWindowHeight_ = 320;

    const QString ProjectWindow::kSettingsRecentFolder_ = 
      "Project.RecentFolder";
    const QString ProjectWindow::kSettingsRecentlyOpened_ = 
      "Project.RecentlyOpened.%0";

    const int ProjectWindow::kMaxRecentlyOpened_ = 5;

    //--------------------------------------------------------------------------
    const int NewProjectDialog::kNewProjectDialogWidth_ = 480;
    const int NewProjectDialog::kNewProjectDialogHeight_ = 96;

    //--------------------------------------------------------------------------
    ProjectWindow::ProjectWindow(Project* project, QWidget* parent) :
      QDialog(parent),
      project_(project),
      recent_folder_(""),
      recent_projects_(nullptr)
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
        new QLabel(QStringLiteral("Recently opened"), recent_layout->widget());

      recent_layout->addWidget(recent_label);

      recent_projects_ = new QListWidget(recent_layout->widget());
      recent_projects_->setObjectName(QStringLiteral("RecentProjectsWidget"));

      recent_projects_->setStyleSheet(
        "QListView::item:selected { background-color: rgb(50, 100, 0); }");

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

      recent_layout->addWidget(recent_projects_);

      connect(
        button_new, 
        &QPushButton::clicked,
        this,
        &ProjectWindow::OnNewProject);

      connect(
        button_open, 
        &QPushButton::clicked, 
        this, 
        &ProjectWindow::OnOpenProject);

      connect(
        recent_projects_, 
        &QListWidget::itemDoubleClicked,
        this,
        &ProjectWindow::OnRecentClicked);

      connect(button_cancel, &QPushButton::clicked, this, &QDialog::reject);

      QSettings& settings = EditorApplication::Instance()->GlobalSettings();
      recent_folder_ = settings.value(kSettingsRecentFolder_, "").toString();

      LoadRecentlyOpened();
    }

    //--------------------------------------------------------------------------
    void ProjectWindow::OnNewProject()
    {
      bool valid;
      QString dir = ShowFolderDialog(&valid);

      if (valid == false)
      {
        return;
      }

      QString settings_path = dir + "/" + Project::kProjectFile;
      static QString error_title = QStringLiteral("Cannot create project");
      

      if (QFile::exists(settings_path) == true)
      {
        MessageBox::Show(
          QMessageBox::Icon::Critical, 
          error_title, 
          "There is already a project in this directory");

        return;
      }

      std::unique_ptr<NewProjectDialog> dialog(new NewProjectDialog(this));

      if (dialog->exec() == QDialog::Rejected)
      {
        return;
      }

      Project::Settings settings;
      settings.name = dialog->project_name();

      if (Project::CreateProject(dir, settings) == true)
      {
        AddRecentProject(dir);
        return;
      }

      MessageBox::Show(
        QMessageBox::Icon::Critical, 
        error_title, 
        "Could not create project settings file");
    }

    //--------------------------------------------------------------------------
    void ProjectWindow::OnOpenProject()
    {
      bool valid;
      QString dir = ShowFolderDialog(&valid);

      if (valid == false)
      {
        return;
      }

      OpenProject(dir);
    }

    //--------------------------------------------------------------------------
    void ProjectWindow::OnRecentClicked(QListWidgetItem* item)
    {
      RecentProjectListWidgetItem* recent = 
        static_cast<RecentProjectListWidgetItem*>(item);

      OpenProject(recent->path());
    }

    //--------------------------------------------------------------------------
    void ProjectWindow::LoadRecentlyOpened()
    {
      QSettings& settings = EditorApplication::Instance()->GlobalSettings();
      
      bool found = true;

      QString formatted;
      QString path;
      int count = 0;

      while (
        found == true && 
        count != kMaxRecentlyOpened_)
      {
        found = false;
        formatted = kSettingsRecentlyOpened_.arg(count);

        if (settings.contains(formatted) == true)
        {
          found = true;
          path = settings.value(formatted).toString();
          recent_project_paths_.push_back(path);
        }

        ++count;
      }

      RefreshRecentlyOpened();
    }

    //--------------------------------------------------------------------------
    void ProjectWindow::RefreshRecentlyOpened()
    {
      recent_projects_->clear();

      QString path;
      for (int i = recent_project_paths_.size() - 1; i >= 0; --i)
      {
        if (Project::Exists(recent_project_paths_.at(i)) == false)
        {
          recent_project_paths_.erase(recent_project_paths_.begin() + i);
        }
      }

      RecentProjectListWidgetItem* new_item = nullptr;

      auto GetProjectName = [](const QString& path)
      {
        Project::Settings settings = Project::GetProjectSettings(path);
        return settings.name;
      };

      for (int i = 0; i < recent_project_paths_.size(); ++i)
      {
        if (i >= kMaxRecentlyOpened_)
        {
          break;
        }

        const QString& full_path = recent_project_paths_.at(i);

        new_item = new RecentProjectListWidgetItem(
          GetProjectName(full_path), full_path, recent_projects_);

        recent_projects_->addItem(new_item);
      }
    }

    //--------------------------------------------------------------------------
    void ProjectWindow::OpenProject(const QString& dir)
    {
      Project& project = EditorApplication::Instance()->project();

      if (project.SetProjectPath(dir) == false)
      {
        MessageBox::Show(
          QMessageBox::Icon::Critical, 
          "Could not open project", 
          "There is no valid project in this directory");

        return;
      }

      AddRecentProject(dir);
      
      accept();
    }

    //--------------------------------------------------------------------------
    void ProjectWindow::AddRecentProject(const QString& path)
    {
      for (int i = 0; i < recent_project_paths_.size(); ++i)
      {
        if (recent_project_paths_.at(i) == path)
        {
          recent_project_paths_.erase(recent_project_paths_.begin() + i);
          break;
        }
      }

      recent_project_paths_.push_front(path);

      RefreshRecentlyOpened();
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

    //--------------------------------------------------------------------------
    void ProjectWindow::SaveRecentProjects()
    {
      QSettings& settings = EditorApplication::Instance()->GlobalSettings();

      int save_at = 0;
      for (
        int i = 0; 
        i < kMaxRecentlyOpened_ && i < recent_project_paths_.size(); 
        ++i)
      {
        const QString& path = recent_project_paths_.at(i);
        if (Project::Exists(path) == true)
        {
          settings.setValue(
            kSettingsRecentlyOpened_.arg(save_at),
            path);

          ++save_at;
        }
      }
      
      for (int i = kMaxRecentlyOpened_ - 1; i >= save_at; --i)
      {
        settings.remove(kSettingsRecentlyOpened_.arg(i));
      }
    }

    //--------------------------------------------------------------------------
    void ProjectWindow::closeEvent(QCloseEvent* evt)
    {
      SaveRecentProjects();
    }

    //--------------------------------------------------------------------------
    NewProjectDialog::NewProjectDialog(QWidget* parent) :
      QDialog(parent),
      project_name_("new-project"),
      button_create_(nullptr)
    {
      int w = kNewProjectDialogWidth_;
      int h = kNewProjectDialogHeight_;

      setMinimumSize(w, h);
      setMaximumSize(w, h);

      QVBoxLayout* main_layout = new QVBoxLayout(this);
      main_layout->setObjectName(QStringLiteral("NewProjectMainLayout"));

      error_ = new QLabel(main_layout->widget());
      error_->setObjectName(QStringLiteral("NewProjectErrorMessage"));
      
      error_->setText(QStringLiteral("Please specify a name"));
      error_->setStyleSheet("QLabel { color: red; }");
      error_->setVisible(false);

      main_layout->addWidget(error_);

      QLineEdit* input_box = new QLineEdit(main_layout->widget());
      input_box->setObjectName(QStringLiteral("NewProjectInputBox"));

      input_box->setText(project_name_);

      main_layout->addWidget(input_box);

      QHBoxLayout* button_layout = new QHBoxLayout(main_layout->widget());
      button_layout->setObjectName(QStringLiteral("NewProjectButtonLayout"));

      button_create_ = 
        new QPushButton(QStringLiteral("Create"), button_layout->widget());
      button_create_->setObjectName(QStringLiteral("NewProjectButtonCreate"));

      QPushButton* button_cancel = 
        new QPushButton(QStringLiteral("Cancel"), button_layout->widget());
      button_cancel->setObjectName(QStringLiteral("NewProjectButtonCancel"));

      button_layout->addWidget(button_create_);
      button_layout->addWidget(button_cancel);

      main_layout->addLayout(button_layout);

      connect(button_create_, &QPushButton::clicked, this, &QDialog::accept);
      connect(button_cancel, &QPushButton::clicked, this, &QDialog::reject);

      connect(
        input_box, 
        &QLineEdit::textChanged, 
        this, 
        &NewProjectDialog::OnNameChanged);
    }

    //--------------------------------------------------------------------------
    const QString& NewProjectDialog::project_name() const
    {
      return project_name_;
    }

    //--------------------------------------------------------------------------
    void NewProjectDialog::OnNameChanged(const QString& name)
    {
      project_name_ = name;
      bool valid = true;

      if (name.isEmpty() == true || name.startsWith(' ') == true)
      {
        valid = false;
      }

      button_create_->setEnabled(valid == true);
      error_->setVisible(valid == false);
    }
  }
}