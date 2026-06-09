#include "SettingsPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QClipboard>
#include <QTimer>

SettingsPage::SettingsPage(QWidget *parent) : QWidget(parent) {
    m_materials = {"Sidebar", "Sheet", "Hud", "WindowBackground", "Popover", "Menu", "FullscreenUI", "ControlCenter", "Widgets", "Inspector", "Titlebar", "Tooltip", "Frosted", "Clear Glass", "Chromatic"};
    m_materialIndex = 0;
    m_appearances = {"Light", "Dark", "Auto"};
    m_appearanceIndex = 2;
    
    setupUi();
    updateCodeSnippet();
}

void SettingsPage::setMaterial(QtLiquidGlass::Material mat) {
    m_materialIndex = static_cast<int>(mat);
    if (m_materialIndex < 0 || m_materialIndex >= m_materials.size()) m_materialIndex = 0;
    
    materialLabel->setText(m_materials[m_materialIndex]);
    updateCodeSnippet();
}

void SettingsPage::setupUi() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    QHBoxLayout *header = new QHBoxLayout();
    QPushButton *backBtn = new QPushButton("Back"); 
    backBtn->setFixedSize(60, 32);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setFocusPolicy(Qt::NoFocus);
    backBtn->setStyleSheet("QPushButton { color: rgba(255,255,255,0.8); background: rgba(255,255,255,0.1); border-radius: 5px; font-size: 12px; border: none; } QPushButton:hover { background: rgba(255,255,255,0.2); color: white; }");
    connect(backBtn, &QPushButton::clicked, this, &SettingsPage::backClicked);
    header->addWidget(backBtn);
    
    header->addStretch();
    
    QLabel *title = new QLabel("Liquid Glass Settings");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: white;");
    header->addWidget(title);
    header->addStretch(); 
    
    mainLayout->addLayout(header);
    mainLayout->addSpacing(10);

    auto createGroup = [](const QString& title) -> QWidget* {
        QWidget* group = new QWidget();
        group->setStyleSheet("QWidget { background-color: rgba(0, 0, 0, 0.3); border-radius: 10px; color: white; } QLabel { background: transparent; border: none; }");
        QVBoxLayout* l = new QVBoxLayout(group);
        l->setContentsMargins(10, 10, 10, 10); 
        QLabel* t = new QLabel(title);
        t->setStyleSheet("font-weight: bold; font-size: 12px; color: rgba(255,255,255,0.6); margin-bottom: 5px; background: transparent; border: none;");
        l->addWidget(t);
        return group;
    };

    const QString checkStyle = "QCheckBox { background:transparent; color: white; spacing: 10px; } QCheckBox::indicator { width: 18px; height: 18px; border-radius: 4px; border: none; rgba(255,255,255,0.2);  } QCheckBox::indicator:checked { background: #00E5FF; border: none; }";

    QHBoxLayout *groupsLayout = new QHBoxLayout();
    groupsLayout->setSpacing(10);

    QWidget* appearanceGroup = createGroup("APPEARANCE");
    QVBoxLayout* appLayout = qobject_cast<QVBoxLayout*>(appearanceGroup->layout());
    
    QHBoxLayout *appearanceLayout = new QHBoxLayout();
    QLabel* lblMat = new QLabel("Material:");
    lblMat->setStyleSheet("background: transparent; border: none;");
    appearanceLayout->addWidget(lblMat);
    appearanceLayout->addStretch();
    
    QPushButton *previewMaterial = new QPushButton("Preview"); 
    previewMaterial->setFixedSize(60, 30);
    previewMaterial->setCursor(Qt::PointingHandCursor);
    previewMaterial->setFocusPolicy(Qt::NoFocus);
    previewMaterial->setStyleSheet("QPushButton { color: rgba(255,255,255,0.8); background: rgba(255,255,255,0.1); border-radius: 5px; font-size: 12px; border: none; } QPushButton:hover { background: rgba(255,255,255,0.2); color: white; }");
    connect(previewMaterial, &QPushButton::clicked, this, &SettingsPage::previewMaterial);
    appearanceLayout->addWidget(previewMaterial);
    appLayout->addLayout(appearanceLayout);

    QHBoxLayout *carouselLayout = new QHBoxLayout();
    carouselLayout->setSpacing(5);
    
    QPushButton *prevBtn = new QPushButton(u8"\u2039"); 
    prevBtn->setFixedSize(30, 32);
    prevBtn->setCursor(Qt::PointingHandCursor);
    prevBtn->setFocusPolicy(Qt::NoFocus);
    prevBtn->setAttribute(Qt::WA_MacShowFocusRect, false);
    prevBtn->setStyleSheet("QPushButton { background: transparent; border-radius: 5px; color: rgba(255,255,255,0.8); font-size: 24px; border: none; outline: none; } QPushButton:hover { background: rgba(255,255,255,0.1); color: white; }");
    connect(prevBtn, &QPushButton::clicked, [this]() { cycleMaterial(-1); });
    
    QPushButton *nextBtn = new QPushButton(u8"\u203A"); 
    nextBtn->setFixedSize(30, 32);
    nextBtn->setCursor(Qt::PointingHandCursor);
    nextBtn->setFocusPolicy(Qt::NoFocus);
    nextBtn->setAttribute(Qt::WA_MacShowFocusRect, false);
    nextBtn->setStyleSheet("QPushButton { background: transparent; border-radius: 5px; color: rgba(255,255,255,0.8); font-size: 24px; border: none; outline: none; } QPushButton:hover { background: rgba(255,255,255,0.1); color: white; }");
    connect(nextBtn, &QPushButton::clicked, [this]() { cycleMaterial(1); });
    
    materialLabel = new QLabel(m_materials[m_materialIndex]);
    materialLabel->setAlignment(Qt::AlignCenter);
    materialLabel->setStyleSheet("background: rgba(255,255,255,0.05); border-radius: 5px; color: white; font-weight: bold; border: none; ");
    materialLabel->setFixedHeight(32);
    
    carouselLayout->addWidget(prevBtn);
    carouselLayout->addWidget(materialLabel, 1); 
    carouselLayout->addWidget(nextBtn);
    
    appLayout->addLayout(carouselLayout);
    
    QLabel* lblTint = new QLabel("Tint Color (Hex):");
    lblTint->setStyleSheet("background: transparent; border: none;");
    appLayout->addWidget(lblTint);

    tintInput = new QLineEdit();
    tintInput->setPlaceholderText("#RRGGBB or clear");
    tintInput->setStyleSheet("QLineEdit { background: rgba(255,255,255,0.1);  border: none; border-radius: 5px; padding: 5px; min-height: 32px; color: white; } QLineEdit:focus { border: 1px solid #00E5FF; }");
    connect(tintInput, &QLineEdit::editingFinished, this, &SettingsPage::emitChange);
    appLayout->addWidget(tintInput);
    
    appLayout->addStretch(); 
    groupsLayout->addWidget(appearanceGroup);
    
    QWidget* propGroup = createGroup("PROPERTIES");
    QVBoxLayout* propLayout = qobject_cast<QVBoxLayout*>(propGroup->layout());
    
    QLabel* lblRad = new QLabel("Corner Radius:");
    lblRad->setStyleSheet("background: transparent; border: none;");
    propLayout->addWidget(lblRad);

    radiusSlider = new QSlider(Qt::Horizontal);
    radiusSlider->setRange(0, 50);
    radiusSlider->setValue(16);
    radiusSlider->setStyleSheet(
        "QSlider {background:transparent;}"
        "QSlider::groove:horizontal { height: 4px; background:transparent; border-radius: 2px; }"
        "QSlider::handle:horizontal { background: white; width: 16px; height: 16px; margin: -6px 0; border-radius: 8px; }"
        "QSlider::sub-page:horizontal { background: #00E5FF; border-radius: 2px; }"
    );
    connect(radiusSlider, &QSlider::valueChanged, this, &SettingsPage::emitChange);
    propLayout->addWidget(radiusSlider);
    
    propLayout->addSpacing(10);
    
    opaqueCheck = new QCheckBox("Opaque BG");
    opaqueCheck->setStyleSheet(checkStyle);
    connect(opaqueCheck, &QCheckBox::toggled, this, &SettingsPage::emitChange);
    propLayout->addWidget(opaqueCheck);

    propLayout->addSpacing(5);

    QLabel* lblAppearance = new QLabel("Appearance:");
    lblAppearance->setStyleSheet("background: transparent; border: none;");
    propLayout->addWidget(lblAppearance);

    QHBoxLayout *appearanceCarousel = new QHBoxLayout();
    appearanceCarousel->setSpacing(5);

    QPushButton *prevAppBtn = new QPushButton(u8"\u2039");
    prevAppBtn->setFixedSize(30, 28);
    prevAppBtn->setCursor(Qt::PointingHandCursor);
    prevAppBtn->setFocusPolicy(Qt::NoFocus);
    prevAppBtn->setStyleSheet("QPushButton { background: transparent; border-radius: 5px; color: rgba(255,255,255,0.8); font-size: 24px; border: none; } QPushButton:hover { background: rgba(255,255,255,0.1); color: white; }");
    connect(prevAppBtn, &QPushButton::clicked, [this]() { cycleAppearance(-1); });

    QPushButton *nextAppBtn = new QPushButton(u8"\u203A");
    nextAppBtn->setFixedSize(30, 28);
    nextAppBtn->setCursor(Qt::PointingHandCursor);
    nextAppBtn->setFocusPolicy(Qt::NoFocus);
    nextAppBtn->setStyleSheet("QPushButton { background: transparent; border-radius: 5px; color: rgba(255,255,255,0.8); font-size: 24px; border: none; } QPushButton:hover { background: rgba(255,255,255,0.1); color: white; }");
    connect(nextAppBtn, &QPushButton::clicked, [this]() { cycleAppearance(1); });

    appearanceLabel = new QLabel(m_appearances[m_appearanceIndex]);
    appearanceLabel->setAlignment(Qt::AlignCenter);
    appearanceLabel->setStyleSheet("background: rgba(255,255,255,0.05); border-radius: 5px; color: white; font-weight: bold; border: none;");
    appearanceLabel->setFixedHeight(28);

    appearanceCarousel->addWidget(prevAppBtn);
    appearanceCarousel->addWidget(appearanceLabel, 1);
    appearanceCarousel->addWidget(nextAppBtn);
    propLayout->addLayout(appearanceCarousel);

    propLayout->addSpacing(5);

    hoveredCheck = new QCheckBox("Hovered State");
    hoveredCheck->setStyleSheet(checkStyle);
    connect(hoveredCheck, &QCheckBox::toggled, this, &SettingsPage::emitChange);
    propLayout->addWidget(hoveredCheck);

    propLayout->addStretch();
    groupsLayout->addWidget(propGroup);

    QWidget* windowGroup = createGroup("WINDOW");
    QVBoxLayout* windowLayout = qobject_cast<QVBoxLayout*>(windowGroup->layout());

    fullSizeTitlebarCheck = new QCheckBox("Full-Size Titlebar");
    fullSizeTitlebarCheck->setStyleSheet(checkStyle);
    fullSizeTitlebarCheck->setChecked(true);
    connect(fullSizeTitlebarCheck, &QCheckBox::toggled, this, &SettingsPage::emitChange);
    windowLayout->addWidget(fullSizeTitlebarCheck);

    backgroundDragCheck = new QCheckBox("Background Drag");
    backgroundDragCheck->setStyleSheet(checkStyle);
    backgroundDragCheck->setChecked(true);
    connect(backgroundDragCheck, &QCheckBox::toggled, this, &SettingsPage::emitChange);
    windowLayout->addWidget(backgroundDragCheck);

    windowLayout->addStretch();
    groupsLayout->addWidget(windowGroup);
    
    mainLayout->addLayout(groupsLayout);

    QWidget* codeGroup = createGroup("C++ CONFIGURATION");
    codeGroup->setFixedHeight(80); 
    QVBoxLayout* groupLayout = qobject_cast<QVBoxLayout*>(codeGroup->layout());
    
    QHBoxLayout* codeLayout = new QHBoxLayout();
    
    codeDisplay = new QTextEdit();
    codeDisplay->setReadOnly(true);
    codeDisplay->setLineWrapMode(QTextEdit::NoWrap);
    codeDisplay->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    codeDisplay->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    codeDisplay->setFixedHeight(34); 
    codeDisplay->setStyleSheet("QTextEdit { background: #1E1E1E; color: #D4D4D4; font-family: \"Menlo\", \"Consolas\", monospace; padding: 5px; border: 1px solid #333; border-radius: 6px; }");
    
    copyBtn = new QPushButton("Copy Config");
    copyBtn->setFixedSize(100, 30); 
    copyBtn->setCursor(Qt::PointingHandCursor);
    copyBtn->setToolTip("Copy full code to clipboard");
    copyBtn->setFocusPolicy(Qt::NoFocus);
    copyBtn->setAttribute(Qt::WA_MacShowFocusRect, false);
    copyBtn->setStyleSheet("QPushButton { background: rgba(255,255,255,0.1); border-radius: 5px; border: none; font-size: 14px; color: white; } QPushButton:hover { background: rgba(255,255,255,0.2); }");
    connect(copyBtn, &QPushButton::clicked, this, &SettingsPage::copyToClipboard);
    
    codeLayout->addWidget(codeDisplay);
    codeLayout->addWidget(copyBtn);
    
    groupLayout->addLayout(codeLayout);
    
    mainLayout->addWidget(codeGroup);
}

void SettingsPage::cycleMaterial(int delta) {
    m_materialIndex += delta;
    if (m_materialIndex < 0) m_materialIndex = m_materials.size() - 1;
    if (m_materialIndex >= m_materials.size()) m_materialIndex = 0;
    
    materialLabel->setText(m_materials[m_materialIndex]);
    emitChange();
}

void SettingsPage::cycleAppearance(int delta) {
    m_appearanceIndex += delta;
    if (m_appearanceIndex < 0) m_appearanceIndex = m_appearances.size() - 1;
    if (m_appearanceIndex >= m_appearances.size()) m_appearanceIndex = 0;

    appearanceLabel->setText(m_appearances[m_appearanceIndex]);
    emitChange();
}

void SettingsPage::emitChange() {
    QtLiquidGlass::Options opts;
    opts.cornerRadius = radiusSlider->value();
    opts.tintColor = tintInput->text();
    opts.opaque = opaqueCheck->isChecked();
    opts.titlebarStyle = fullSizeTitlebarCheck->isChecked()
        ? QtLiquidGlass::TitlebarStyle::TransparentFullSize
        : QtLiquidGlass::TitlebarStyle::Preserve;
    opts.dragBehavior = backgroundDragCheck->isChecked()
        ? QtLiquidGlass::WindowDragBehavior::MovableByWindowBackground
        : QtLiquidGlass::WindowDragBehavior::Preserve;
    opts.appearance = static_cast<QtLiquidGlass::AdaptiveAppearance>(m_appearanceIndex);
    opts.interaction = hoveredCheck->isChecked() ? QtLiquidGlass::InteractionState::Hovered : QtLiquidGlass::InteractionState::Normal;

    QtLiquidGlass::Material mat = static_cast<QtLiquidGlass::Material>(m_materialIndex);
    
    emit glassSettingsChanged(opts, mat);
    updateCodeSnippet();
}

void SettingsPage::updateCodeSnippet() {
    QString matName = m_materials[m_materialIndex];
    QString tint = tintInput->text().isEmpty() ? "\"\"" : "\"" + tintInput->text() + "\"";
    QString radius = QString::number(radiusSlider->value(), 'f', 1);
    QString opaque = opaqueCheck->isChecked() ? "true" : "false";
    QString titlebar = fullSizeTitlebarCheck->isChecked() ? "TransparentFullSize" : "Preserve";
    QString drag = backgroundDragCheck->isChecked() ? "MovableByWindowBackground" : "Preserve";
    QString appearance = m_appearances[m_appearanceIndex];
    QString interaction = hoveredCheck->isChecked() ? "Hovered" : "Normal";

    QString html = QString(
        "<span style='color:#569CD6'>addGlassEffect</span>"
        "<span style='color:#D4D4D4'>(</span>"
        "<span style='color:#569CD6'>this</span>"
        "<span style='color:#D4D4D4'>, </span>"
        "<span style='color:#4EC9B0'>Material</span>"
        "<span style='color:#D4D4D4'>::</span>"
        "<span style='color:#4EC9B0'>%1</span>"
        "<span style='color:#D4D4D4'>, {</span>"
        "<span style='color:#B5CEA8'>%2</span>"
        "<span style='color:#D4D4D4'>, </span>"
        "<span style='color:#CE9178'>%3</span>"
        "<span style='color:#D4D4D4'>, </span>"
        "<span style='color:#569CD6'>%4</span>"
        "<span style='color:#D4D4D4'>, </span>"
        "<span style='color:#4EC9B0'>%5</span>"
        "<span style='color:#D4D4D4'>, </span>"
        "<span style='color:#4EC9B0'>%6</span>"
        "<span style='color:#D4D4D4'>, </span>"
        "<span style='color:#4EC9B0'>%7</span>"
        "<span style='color:#D4D4D4'>, </span>"
        "<span style='color:#4EC9B0'>%8</span>"
        "<span style='color:#D4D4D4'>});</span>"
    ).arg(matName, radius, tint, opaque, appearance, interaction, titlebar, drag);

    codeDisplay->setHtml(html);
}

void SettingsPage::copyToClipboard() {
    QString matName = m_materials[m_materialIndex];
    QString tint = tintInput->text().isEmpty() ? "\"\"" : "\"" + tintInput->text() + "\"";
    QString radius = QString::number(radiusSlider->value(), 'f', 1);
    QString opaque = opaqueCheck->isChecked() ? "true" : "false";
    QString titlebar = fullSizeTitlebarCheck->isChecked() ? "TransparentFullSize" : "Preserve";
    QString drag = backgroundDragCheck->isChecked() ? "MovableByWindowBackground" : "Preserve";
    QString appearance = m_appearances[m_appearanceIndex];
    QString interaction = hoveredCheck->isChecked() ? "Hovered" : "Normal";

    QString fullCode = QString(
        "QtLiquidGlass::Options opts;\n"
        "opts.cornerRadius = %1;\n"
        "opts.tintColor = %2;\n"
        "opts.opaque = %3;\n"
        "opts.titlebarStyle = QtLiquidGlass::TitlebarStyle::%4;\n"
        "opts.dragBehavior = QtLiquidGlass::WindowDragBehavior::%5;\n"
        "opts.appearance = QtLiquidGlass::AdaptiveAppearance::%6;\n"
        "opts.interaction = QtLiquidGlass::InteractionState::%7;\n\n"
        "QtLiquidGlass::addGlassEffect(this, QtLiquidGlass::Material::%8, opts);"
    ).arg(radius, tint, opaque, titlebar, drag, appearance, interaction, matName);
    
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(fullCode);
    
    QString originalStyle = copyBtn->styleSheet();
    copyBtn->setStyleSheet("QPushButton { background: #00E5FF; border-radius: 5px; border: none; color: black; }");
    QTimer::singleShot(500, [this, originalStyle]() {
        copyBtn->setStyleSheet(originalStyle);
    });
}
