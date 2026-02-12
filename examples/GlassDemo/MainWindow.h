#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include "FadeStack.h"
#include "PlayerPage.h"
#include "SettingsPage.h"
#include "PreviewPage.h"
#include "QtLiquidGlass/QtLiquidGlass.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void toggleMiniMode(bool mini);
    void updateGlass(QtLiquidGlass::Options opts, QtLiquidGlass::Material mat);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void setupUi();
    void setupConnections();
    void animateResize(int w, int h);

    int m_glassId = -1;
    int lastPageIndex = 0;
    
    FadeStack* m_stack;
    PlayerPage* m_playerPage;
    SettingsPage* m_settingsPage;
    PreviewPage* m_previewPage;
    
    QtLiquidGlass::Options m_currentOpts;
    QtLiquidGlass::Material m_currentMat = QtLiquidGlass::Material::Sidebar;
};

#endif // MAINWINDOW_H
