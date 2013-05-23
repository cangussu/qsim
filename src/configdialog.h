#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "ui_configdialog.h"

class ConfigDialog : public QDialog
{
    Q_OBJECT

    public:
        ConfigDialog(QWidget *parent = 0);
        ~ConfigDialog();
        
        bool drawPotential() {  return ( ui.draw_potential->checkState() == Qt::Checked ); }

    public slots:
        void draw(int);
        void on_draw_potential_stateChanged(int state);
        int exec();
    
    private:

        Ui::ConfigDialog ui;
        bool m_first;
};

#endif
