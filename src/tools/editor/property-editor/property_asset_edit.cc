#include "tools/editor/property-editor/property_asset_edit.h"

#include "tools/editor/application/editor_application.h"
#include "tools/editor/application/styling.h"

#include "tools/editor/editor-widgets/searcher_widget.h"

#include <engine/services/asset_service.h>

#include <QPushButton>
#include <QHBoxLayout>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const int PropertyAssetEdit::kSearchBoxWidth_ = 256;
    const int PropertyAssetEdit::kClearButtonWidth_ = 24;

    //--------------------------------------------------------------------------
    PropertyAssetEdit::PropertyAssetEdit(QWidget* parent) :
      QWidget(parent),
      input_(nullptr)
    {
      setObjectName(QStringLiteral("PropertyAssetEdit"));

      QHBoxLayout* main_layout = new QHBoxLayout(this);

      input_ = new QPushButton(this);
      input_->setFixedWidth(kSearchBoxWidth_);

      QPushButton* clear = new QPushButton(this);
      clear->setFixedWidth(kClearButtonWidth_);
      clear->setText("-");
      clear->setStyleSheet("background: " + 
        Styling::GetStyleColorCSS(Styling::ColorRole::kBlueButton));

      connect(
        input_, 
        &QPushButton::released, 
        this, 
        &PropertyAssetEdit::ShowSearcher);

      connect(
        clear,
        &QPushButton::released,
        this,
        &PropertyAssetEdit::OnClear);

      main_layout->addWidget(input_);
      main_layout->addWidget(clear);

      main_layout->setAlignment(Qt::AlignLeft);
      main_layout->setContentsMargins(0, 0, 0, 0);

      setLayout(main_layout);
    }

    //--------------------------------------------------------------------------
    void PropertyAssetEdit::set_asset(const engine::SerializableAsset& asset)
    {
      asset_ = asset;
      
      QString name = asset_.name;
      input_->setText(name.isEmpty() ? "(None)" : name);

      UpdateVisuals();
    }

    //--------------------------------------------------------------------------
    const engine::SerializableAsset& PropertyAssetEdit::asset() const
    {
      return asset_;
    }

    //--------------------------------------------------------------------------
    void PropertyAssetEdit::UpdateVisuals()
    {
      foundation::String name = asset_.name;

      if (name.empty() == true)
      {
        setStyleSheet("");
        return;
      }

      engine::AssetService* as = 
        EditorApplication::Instance()->GetService<engine::AssetService>();

      bool exists = as->Get(asset_.type, name) != nullptr;

      Styling::ColorRole cr = exists == true ? 
        Styling::ColorRole::kValid : Styling::ColorRole::kInvalid;

      setStyleSheet("background: " + Styling::GetStyleColorCSS(cr));
    }

    //--------------------------------------------------------------------------
    void PropertyAssetEdit::ShowSearcher()
    {
      compilers::AssetTypes type = asset_.type;
      if (type == compilers::AssetTypes::kCount)
      {
        return;
      }

      engine::AssetService* as = 
        EditorApplication::Instance()->GetService<engine::AssetService>();

      SearcherWidget* searcher = new SearcherWidget();

      foundation::Vector<const engine::IAsset*> registered = 
        as->GetRegistered(type);

      for (int i = 0; i < registered.size(); ++i)
      {
        const foundation::String& name = registered.at(i)->name();
        searcher->AddItem(name.c_str());
      }

      connect(searcher, &SearcherWidget::Selected, this, [=](int index)
      {
        const foundation::String& name = registered.at(index)->name();

        asset_.handle = as->Get(type, name);
        asset_.SetName(name);

        emit AssetChanged(name.c_str());
      });

      searcher->Show();
    }

    //--------------------------------------------------------------------------
    void PropertyAssetEdit::OnClear()
    {
      asset_.handle = nullptr;
      asset_.SetName("");

      emit AssetCleared();
    }
  }
}