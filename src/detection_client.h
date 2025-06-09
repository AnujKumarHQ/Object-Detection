#ifndef DETECTION_CLIENT_H
#define DETECTION_CLIENT_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QRect>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

struct Detection {
    QString className;
    double confidence;
    QRect bbox;
};

struct DetectionResult {
    QList<Detection> detections;
    int processingTime;
    bool success;
    QString errorMessage;
};

struct DetectionRequest {
    QString imagePath;
    double confidenceThreshold;
    double iouThreshold;
    QString modelName;
    bool saveAnnotated;
};

class DetectionClient : public QObject
{
    Q_OBJECT

public:
    explicit DetectionClient(QObject *parent = nullptr);
    ~DetectionClient();

    void detectObjects(const DetectionRequest& request);
    bool isProcessing() const { return m_isProcessing; }

signals:
    void detectionComplete(const DetectionResult& result);
    void detectionError(const QString& error);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onProcessTimeout();

private:
    void setupPythonEnvironment();
    QString createRequestJson(const DetectionRequest& request);
    DetectionResult parseResponse(const QByteArray& response);

    QProcess* m_pythonProcess;
    QTimer* m_timeoutTimer;
    bool m_isProcessing;
    QString m_pythonScriptPath;
    QString m_pythonExecutable;
    DetectionRequest m_currentRequest;
};

#endif // DETECTION_CLIENT_H