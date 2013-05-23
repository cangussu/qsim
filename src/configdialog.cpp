#include "configdialog.h"
#include <QSettings>

ConfigDialog::ConfigDialog(QWidget *parent)
    : QDialog(parent),
      m_first(true)
{
    ui.setupUi(this);
    QSettings settings("ETW", "Qsim");
    
    if( settings.value("potential", false).toBool() )
        ui.draw_potential->setCheckState( Qt::Checked  );
    else
        ui.draw_potential->setCheckState( Qt::Unchecked );
}

void ConfigDialog::on_draw_potential_stateChanged(int state)
{
    QSettings settings("ETW", "Qsim");
    bool val = (state == Qt::Checked);
    settings.setValue("potential", val );
}

int ConfigDialog::exec()
{
    return QDialog::exec();
}

ConfigDialog::~ConfigDialog()
{
}

void ConfigDialog::draw(int state)
{
    /// @todo implement me
}
