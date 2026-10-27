#include "PreviewPage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

PreviewPage::PreviewPage(QWidget *parent) : QWidget(parent) {
    m_materials = {"Sidebar", "Sheet", "Hud", "WindowBackground", "Popover", "Menu", "FullscreenUI", "ControlCenter", "Widgets", "Inspector", "Titlebar", "Tooltip", "Frosted", "ClearGlass", "Chromatic"};
    setupUi();
}

void PreviewPage::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 40, 0, 40); // Top margin for carousel
    
    // Top Carousel Container
    QWidget* carouselContainer = new QWidget();
    carouselContainer->setAttribute(Qt::WA_TranslucentBackground);
    carouselContainer->setStyleSheet("background: rgba(0,0,0,0.2); border-radius: 20px;");
    carouselContainer->setFixedSize(300, 50);
    
    QHBoxLayout* carouselLayout = new QHBoxLayout(carouselContainer);
    carouselLayout->setContentsMargins(10, 0, 10, 0);
    carouselLayout->setSpacing(10);
    
    QPushButton *prevBtn = new QPushButton(u8"\u2039"); 
    prevBtn->setFixedSize(30, 30);
    prevBtn->setCursor(Qt::PointingHandCursor);
    prevBtn->setStyleSheet("QPushButton { background: transparent; color: rgba(255,255,255,0.6); font-size: 24px; border: none; font-weight: 300; } QPushButton:hover { color: white; }");
    connect(prevBtn, &QPushButton::clicked, [this]() { cycleMaterial(-1); });
    
    QPushButton *nextBtn = new QPushButton(u8"\u203A"); 
    nextBtn->setFixedSize(30, 30);
    nextBtn->setCursor(Qt::PointingHandCursor);
    nextBtn->setStyleSheet("QPushButton { background: transparent; color: rgba(255,255,255,0.6); font-size: 24px; border: none; font-weight: 300; } QPushButton:hover { color: white; }");
    connect(nextBtn, &QPushButton::clicked, [this]() { cycleMaterial(1); });
    
    m_materialLabel = new QLabel(m_materials[m_materialIndex]);
    m_materialLabel->setAlignment(Qt::AlignCenter);
    m_materialLabel->setStyleSheet("color: white; font-weight: 600; font-size: 15px; background: transparent; border: none;");
    
    carouselLayout->addWidget(prevBtn);
    carouselLayout->addWidget(m_materialLabel, 1);
    carouselLayout->addWidget(nextBtn);
    
    // Align container to top center
    mainLayout->addWidget(carouselContainer, 0, Qt::AlignTop | Qt::AlignHCenter);
    
    mainLayout->addStretch();
    
    m_hint = new QLabel("Click anywhere to return", this);
    m_hint->setStyleSheet("color: rgba(255,255,255,0.4); font-size: 13px; background: transparent;");
    m_hint->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_hint, 0, Qt::AlignBottom | Qt::AlignHCenter);
}

void PreviewPage::setMaterial(QtLiquidGlass::Material mat) {
    m_materialIndex = static_cast<int>(mat);
    if (m_materialIndex < 0 || m_materialIndex >= m_materials.size()) m_materialIndex = 0;
    m_materialLabel->setText(m_materials[m_materialIndex]);
}

void PreviewPage::cycleMaterial(int delta) {
    m_materialIndex += delta;
    if (m_materialIndex < 0) m_materialIndex = m_materials.size() - 1;
    if (m_materialIndex >= m_materials.size()) m_materialIndex = 0;
    
    m_materialLabel->setText(m_materials[m_materialIndex]);
    
    emit materialChanged(static_cast<QtLiquidGlass::Material>(m_materialIndex));
}

void PreviewPage::mousePressEvent(QMouseEvent *event) {
    emit clicked();
    QWidget::mousePressEvent(event);
}

