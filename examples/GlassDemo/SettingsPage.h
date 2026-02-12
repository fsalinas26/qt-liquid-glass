#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include "QtLiquidGlass/QtLiquidGlass.h"

class SettingsPage : public QWidget {
    Q_OBJECT
public:
    explicit SettingsPage(QWidget *parent = nullptr);
    void setMaterial(QtLiquidGlass::Material mat);

signals:
    void backClicked();
    void previewMaterial();
    void glassSettingsChanged(QtLiquidGlass::Options opts, QtLiquidGlass::Material mat);

private:
    void setupUi();
    void emitChange();
    void cycleMaterial(int delta);
    void cycleAppearance(int delta);
    void updateCodeSnippet();
    void copyToClipboard();

    QSlider *radiusSlider;
    QLineEdit *tintInput;
    QCheckBox *opaqueCheck;
    QCheckBox *hoveredCheck;

    QLabel *materialLabel;
    int m_materialIndex = 0;
    QStringList m_materials;

    QLabel *appearanceLabel;
    int m_appearanceIndex = 2; // Auto
    QStringList m_appearances;

    QTextEdit *codeDisplay;
    QPushButton *copyBtn;
};

#endif // SETTINGSPAGE_H
