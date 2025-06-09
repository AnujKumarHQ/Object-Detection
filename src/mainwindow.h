#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QProgressBar>
#include <QTextEdit>
#include <QScrollArea>
#include <QPixmap>
#include <QTimer>
#include <QGroupBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QSplitter>

#include "detection_client.h"
#include "image_processor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openImage();
    void openVideo();
    void startWebcam();
    void stopProcessing();
    void processCurrentFrame();
    void onDetectionComplete(const DetectionResult& result);
    void onDetectionError(const QString& error);
    void updateConfidenceThreshold(double value);
    void updateIoUThreshold(double value);
    void toggleClassFilter(const QString& className, bool enabled);

private:
    void setupUI();
    void setupControlPanel();
    void setupImageDisplay();
    void setupResultsPanel();
    void updateImageDisplay(const QPixmap& pixmap);
    void updateResultsText(const DetectionResult& result);
    void resetUI();

    // UI Components
    QWidget* m_centralWidget;
    QSplitter* m_mainSplitter;
    QSplitter* m_rightSplitter;
    
    // Control Panel
    QGroupBox* m_controlGroup;
    QPushButton* m_openImageBtn;
    QPushButton* m_openVideoBtn;
    QPushButton* m_webcamBtn;
    QPushButton* m_stopBtn;
    QDoubleSpinBox* m_confidenceSpinBox;
    QDoubleSpinBox* m_iouSpinBox;
    QCheckBox* m_showLabelsCheckBox;
    QCheckBox* m_showConfidenceCheckBox;
    QComboBox* m_modelComboBox;
    
    // Image Display
    QScrollArea* m_imageScrollArea;
    QLabel* m_imageLabel;
    QProgressBar* m_progressBar;
    
    // Results Panel
    QGroupBox* m_resultsGroup;
    QTextEdit* m_resultsText;
    QLabel* m_statsLabel;
    
    // Backend components
    DetectionClient* m_detectionClient;
    ImageProcessor* m_imageProcessor;
    
    // State
    QString m_currentImagePath;
    QString m_currentVideoPath;
    bool m_isProcessing;
    QTimer* m_webcamTimer;
    
    // Settings
    double m_confidenceThreshold;
    double m_iouThreshold;
    bool m_showLabels;
    bool m_showConfidence;
    QString m_selectedModel;
};

#endif // MAINWINDOW_H