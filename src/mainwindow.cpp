#include "mainwindow.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_detectionClient(new DetectionClient(this))
    , m_imageProcessor(new ImageProcessor(this))
    , m_isProcessing(false)
    , m_webcamTimer(new QTimer(this))
    , m_confidenceThreshold(0.5)
    , m_iouThreshold(0.45)
    , m_showLabels(true)
    , m_showConfidence(true)
    , m_selectedModel("yolov5s")
{
    setupUI();
    
    // Connect signals
    connect(m_detectionClient, &DetectionClient::detectionComplete,
            this, &MainWindow::onDetectionComplete);
    connect(m_detectionClient, &DetectionClient::detectionError,
            this, &MainWindow::onDetectionError);
    
    connect(m_webcamTimer, &QTimer::timeout,
            this, &MainWindow::processCurrentFrame);
    
    setWindowTitle("YOLO Object Detection - C++ UI");
    resize(1200, 800);
}

MainWindow::~MainWindow()
{
    if (m_isProcessing) {
        stopProcessing();
    }
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);
    
    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal);
    m_rightSplitter = new QSplitter(Qt::Vertical);
    
    setupControlPanel();
    setupImageDisplay();
    setupResultsPanel();
    
    // Add panels to splitters
    m_mainSplitter->addWidget(m_controlGroup);
    m_rightSplitter->addWidget(m_imageScrollArea);
    m_rightSplitter->addWidget(m_resultsGroup);
    m_mainSplitter->addWidget(m_rightSplitter);
    
    // Set splitter proportions
    m_mainSplitter->setSizes({300, 900});
    m_rightSplitter->setSizes({600, 200});
    
    // Main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->addWidget(m_mainSplitter);
    mainLayout->setContentsMargins(10, 10, 10, 10);
}

void MainWindow::setupControlPanel()
{
    m_controlGroup = new QGroupBox("Controls");
    QVBoxLayout* controlLayout = new QVBoxLayout(m_controlGroup);
    
    // File operations
    QGroupBox* fileGroup = new QGroupBox("Input Source");
    QVBoxLayout* fileLayout = new QVBoxLayout(fileGroup);
    
    m_openImageBtn = new QPushButton("Open Image");
    m_openVideoBtn = new QPushButton("Open Video");
    m_webcamBtn = new QPushButton("Start Webcam");
    m_stopBtn = new QPushButton("Stop");
    m_stopBtn->setEnabled(false);
    
    fileLayout->addWidget(m_openImageBtn);
    fileLayout->addWidget(m_openVideoBtn);
    fileLayout->addWidget(m_webcamBtn);
    fileLayout->addWidget(m_stopBtn);
    
    // Detection settings
    QGroupBox* settingsGroup = new QGroupBox("Detection Settings");
    QGridLayout* settingsLayout = new QGridLayout(settingsGroup);
    
    settingsLayout->addWidget(new QLabel("Model:"), 0, 0);
    m_modelComboBox = new QComboBox();
    m_modelComboBox->addItems({"yolov5s", "yolov5m", "yolov5l", "yolov5x"});
    settingsLayout->addWidget(m_modelComboBox, 0, 1);
    
    settingsLayout->addWidget(new QLabel("Confidence:"), 1, 0);
    m_confidenceSpinBox = new QDoubleSpinBox();
    m_confidenceSpinBox->setRange(0.0, 1.0);
    m_confidenceSpinBox->setSingleStep(0.05);
    m_confidenceSpinBox->setValue(m_confidenceThreshold);
    m_confidenceSpinBox->setDecimals(2);
    settingsLayout->addWidget(m_confidenceSpinBox, 1, 1);
    
    settingsLayout->addWidget(new QLabel("IoU Threshold:"), 2, 0);
    m_iouSpinBox = new QDoubleSpinBox();
    m_iouSpinBox->setRange(0.0, 1.0);
    m_iouSpinBox->setSingleStep(0.05);
    m_iouSpinBox->setValue(m_iouThreshold);
    m_iouSpinBox->setDecimals(2);
    settingsLayout->addWidget(m_iouSpinBox, 2, 1);
    
    // Display options
    QGroupBox* displayGroup = new QGroupBox("Display Options");
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup);
    
    m_showLabelsCheckBox = new QCheckBox("Show Labels");
    m_showLabelsCheckBox->setChecked(m_showLabels);
    m_showConfidenceCheckBox = new QCheckBox("Show Confidence");
    m_showConfidenceCheckBox->setChecked(m_showConfidence);
    
    displayLayout->addWidget(m_showLabelsCheckBox);
    displayLayout->addWidget(m_showConfidenceCheckBox);
    
    // Progress bar
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    
    // Add all groups to control layout
    controlLayout->addWidget(fileGroup);
    controlLayout->addWidget(settingsGroup);
    controlLayout->addWidget(displayGroup);
    controlLayout->addWidget(m_progressBar);
    controlLayout->addStretch();
    
    // Connect signals
    connect(m_openImageBtn, &QPushButton::clicked, this, &MainWindow::openImage);
    connect(m_openVideoBtn, &QPushButton::clicked, this, &MainWindow::openVideo);
    connect(m_webcamBtn, &QPushButton::clicked, this, &MainWindow::startWebcam);
    connect(m_stopBtn, &QPushButton::clicked, this, &MainWindow::stopProcessing);
    
    connect(m_confidenceSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::updateConfidenceThreshold);
    connect(m_iouSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::updateIoUThreshold);
}

void MainWindow::setupImageDisplay()
{
    m_imageScrollArea = new QScrollArea();
    m_imageLabel = new QLabel();
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setMinimumSize(400, 300);
    m_imageLabel->setStyleSheet("QLabel { background-color: #f0f0f0; border: 2px dashed #ccc; }");
    m_imageLabel->setText("No image loaded\nClick 'Open Image' to start");
    
    m_imageScrollArea->setWidget(m_imageLabel);
    m_imageScrollArea->setWidgetResizable(true);
}

void MainWindow::setupResultsPanel()
{
    m_resultsGroup = new QGroupBox("Detection Results");
    QVBoxLayout* resultsLayout = new QVBoxLayout(m_resultsGroup);
    
    m_statsLabel = new QLabel("Ready");
    m_statsLabel->setStyleSheet("QLabel { font-weight: bold; color: #2c3e50; }");
    
    m_resultsText = new QTextEdit();
    m_resultsText->setMaximumHeight(150);
    m_resultsText->setReadOnly(true);
    m_resultsText->setPlainText("Detection results will appear here...");
    
    resultsLayout->addWidget(m_statsLabel);
    resultsLayout->addWidget(m_resultsText);
}

void MainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open Image",
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
        "Image Files (*.png *.jpg *.jpeg *.bmp *.tiff)"
    );
    
    if (!fileName.isEmpty()) {
        m_currentImagePath = fileName;
        
        // Load and display image
        QPixmap pixmap(fileName);
        if (!pixmap.isNull()) {
            updateImageDisplay(pixmap);
            
            // Start detection
            m_progressBar->setVisible(true);
            m_progressBar->setRange(0, 0); // Indeterminate progress
            m_isProcessing = true;
            
            DetectionRequest request;
            request.imagePath = fileName;
            request.confidenceThreshold = m_confidenceThreshold;
            request.iouThreshold = m_iouThreshold;
            request.modelName = m_selectedModel;
            
            m_detectionClient->detectObjects(request);
        } else {
            QMessageBox::warning(this, "Error", "Failed to load image: " + fileName);
        }
    }
}

void MainWindow::openVideo()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open Video",
        QStandardPaths::writableLocation(QStandardPaths::MoviesLocation),
        "Video Files (*.mp4 *.avi *.mov *.mkv *.wmv)"
    );
    
    if (!fileName.isEmpty()) {
        m_currentVideoPath = fileName;
        // Video processing implementation would go here
        QMessageBox::information(this, "Info", "Video processing not yet implemented");
    }
}

void MainWindow::startWebcam()
{
    // Webcam processing implementation would go here
    QMessageBox::information(this, "Info", "Webcam processing not yet implemented");
}

void MainWindow::stopProcessing()
{
    m_isProcessing = false;
    m_webcamTimer->stop();
    m_progressBar->setVisible(false);
    m_stopBtn->setEnabled(false);
    m_openImageBtn->setEnabled(true);
    m_openVideoBtn->setEnabled(true);
    m_webcamBtn->setEnabled(true);
}

void MainWindow::processCurrentFrame()
{
    // This would be called for video/webcam processing
}

void MainWindow::onDetectionComplete(const DetectionResult& result)
{
    m_progressBar->setVisible(false);
    m_isProcessing = false;
    
    // Update results display
    updateResultsText(result);
    
    // Update image with bounding boxes
    if (!m_currentImagePath.isEmpty()) {
        QPixmap annotatedPixmap = m_imageProcessor->drawBoundingBoxes(
            m_currentImagePath, result, m_showLabels, m_showConfidence
        );
        updateImageDisplay(annotatedPixmap);
    }
}

void MainWindow::onDetectionError(const QString& error)
{
    m_progressBar->setVisible(false);
    m_isProcessing = false;
    
    QMessageBox::critical(this, "Detection Error", error);
    m_statsLabel->setText("Error occurred");
    m_resultsText->setPlainText("Detection failed: " + error);
}

void MainWindow::updateConfidenceThreshold(double value)
{
    m_confidenceThreshold = value;
}

void MainWindow::updateIoUThreshold(double value)
{
    m_iouThreshold = value;
}

void MainWindow::toggleClassFilter(const QString& className, bool enabled)
{
    // Implementation for class filtering
}

void MainWindow::updateImageDisplay(const QPixmap& pixmap)
{
    if (!pixmap.isNull()) {
        // Scale pixmap to fit the display while maintaining aspect ratio
        QPixmap scaledPixmap = pixmap.scaled(
            m_imageLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        m_imageLabel->setPixmap(scaledPixmap);
        m_imageLabel->resize(scaledPixmap.size());
    }
}

void MainWindow::updateResultsText(const DetectionResult& result)
{
    QString resultsText;
    resultsText += QString("Detection completed in %1ms\n").arg(result.processingTime);
    resultsText += QString("Objects detected: %1\n\n").arg(result.detections.size());
    
    for (int i = 0; i < result.detections.size(); ++i) {
        const Detection& det = result.detections[i];
        resultsText += QString("Object %1:\n").arg(i + 1);
        resultsText += QString("  Class: %1\n").arg(det.className);
        resultsText += QString("  Confidence: %1%\n").arg(det.confidence * 100, 0, 'f', 1);
        resultsText += QString("  Box: (%1, %2, %3, %4)\n\n")
                          .arg(det.bbox.x())
                          .arg(det.bbox.y())
                          .arg(det.bbox.width())
                          .arg(det.bbox.height());
    }
    
    m_resultsText->setPlainText(resultsText);
    m_statsLabel->setText(QString("Detected %1 objects in %2ms")
                             .arg(result.detections.size())
                             .arg(result.processingTime));
}

void MainWindow::resetUI()
{
    m_imageLabel->clear();
    m_imageLabel->setText("No image loaded\nClick 'Open Image' to start");
    m_resultsText->clear();
    m_statsLabel->setText("Ready");
    m_progressBar->setVisible(false);
}

#include "mainwindow.moc"