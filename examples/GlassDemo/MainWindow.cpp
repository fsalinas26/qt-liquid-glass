#include "MainWindow.h"
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowFlags(Qt::Window);

    // Transparent background so the glass effect shows through
    setAutoFillBackground(false);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::transparent);
    setPalette(pal);

    setupUi();
    resize(620, 240);

    QtLiquidGlass::Options opts;
    opts.cornerRadius = 16.0;
    m_glassId = QtLiquidGlass::addGlassEffect(this, QtLiquidGlass::Material::Sidebar, opts);
}

MainWindow::~MainWindow() {
    if (m_glassId >= 0) {
        QtLiquidGlass::remove(m_glassId);
    }
}

void MainWindow::updateGlass(QtLiquidGlass::Options opts, QtLiquidGlass::Material mat) {
    m_currentOpts = opts;
    m_currentMat = mat;

    if (m_glassId >= 0) {
        QtLiquidGlass::configure(m_glassId, opts);
        // Re-add to switch material; replaces the existing view automatically
        m_glassId = QtLiquidGlass::addGlassEffect(this, mat, opts);
    }
}

void MainWindow::setupUi() {
    QWidget *central = new QWidget(this);
    
    central->setAutoFillBackground(false);
    QPalette pal = central->palette();
    pal.setColor(QPalette::Window, Qt::transparent);
    central->setPalette(pal);
    
    setCentralWidget(central);
    
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    
    m_stack = new FadeStack(central);
    m_playerPage = new PlayerPage();
    m_settingsPage = new SettingsPage();
    m_previewPage = new PreviewPage();

    m_stack->addWidget(m_playerPage);
    m_stack->addWidget(m_settingsPage);
    m_stack->addWidget(m_previewPage);
    
    layout->addWidget(m_stack, 1);

    // Setup logic interactions
    setupConnections();
}

void MainWindow::setupConnections() {
    connect(m_stack, &FadeStack::setLastPage, [this](int index) {
        lastPageIndex = index;
    });

    connect(m_playerPage, &PlayerPage::settingsClicked, [this]() {
        m_stack->fadeTo(1); 
        if (height() < 500) animateResize(620, 500);
    });
    
    connect(m_playerPage, &PlayerPage::miniModeToggled, this, &MainWindow::toggleMiniMode);
    
    connect(m_playerPage, &PlayerPage::previewClicked, [this]() {
        m_previewPage->setMaterial(m_currentMat); 
        m_stack->fadeTo(2); 
    });

    connect(m_settingsPage, &SettingsPage::backClicked, [this]() {
        m_stack->fadeTo(0); 
        
        if (m_playerPage->isMiniMode()) {
            animateResize(320, 240);
        } else {
            animateResize(620, 240);
        }
    });
    
    connect(m_settingsPage, &SettingsPage::glassSettingsChanged, this, &MainWindow::updateGlass);
    
    connect(m_settingsPage, &SettingsPage::previewMaterial, [this]() {
        m_previewPage->setMaterial(m_currentMat);
        m_stack->fadeTo(2); 
    });

    connect(m_previewPage, &PreviewPage::clicked, [this]() {
        m_stack->fadeTo(lastPageIndex); 
    });
    
    connect(m_previewPage, &PreviewPage::materialChanged, [this](QtLiquidGlass::Material mat) {
        updateGlass(m_currentOpts, mat);
        m_settingsPage->setMaterial(mat); 
    });
}

void MainWindow::toggleMiniMode(bool mini) {
    if (mini) {
        animateResize(320, 240);
    } else {
        animateResize(620, 240);
    }
}

void MainWindow::animateResize(int w, int h) {
    if (width() == w && height() == h) return;
    
    QPropertyAnimation* anim = new QPropertyAnimation(this, "size");
    anim->setDuration(450);
    anim->setStartValue(size());
    anim->setEndValue(QSize(w, h));
    anim->setEasingCurve(QEasingCurve::OutExpo);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
