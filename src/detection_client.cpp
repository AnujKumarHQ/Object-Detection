#include "detection_client.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QJsonParseError>

DetectionClient::DetectionClient(QObject *parent)
    : QObject(parent)
    , m_pythonProcess(new QProcess(this))
    , m_timeoutTimer(new QTimer(this))
    , m_isProcessing(false)
{
    setupPythonEnvironment();
    
    // Setup process connections
    connect(m_pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DetectionClient::onProcessFinished);
    connect(m_pythonProcess, &QProcess::errorOccurred,
            this, &DetectionClient::onProcessError);
    
    // Setup timeout timer
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(30000); // 30 seconds timeout
    connect(m_timeoutTimer, &QTimer::timeout,
            this, &DetectionClient::onProcessTimeout);
}

DetectionClient::~DetectionClient()
{
    if (m_pythonProcess->state() != QProcess::NotRunning) {
        m_pythonProcess->kill();
        m_pythonProcess->waitForFinished(3000);
    }
}

void DetectionClient::setupPythonEnvironment()
{
    // Try to find Python executable
    QStringList pythonCandidates = {"python3", "python", "py"};
    
    for (const QString& candidate : pythonCandidates) {
        QProcess testProcess;
        testProcess.start(candidate, QStringList() << "--version");
        if (testProcess.waitForFinished(3000) && testProcess.exitCode() == 0) {
            m_pythonExecutable = candidate;
            break;
        }
    }
    
    if (m_pythonExecutable.isEmpty()) {
        m_pythonExecutable = "python"; // Fallback
    }
    
    // Set Python script path relative to executable
    QString appDir = QCoreApplication::applicationDirPath();
    m_pythonScriptPath = QDir(appDir).absoluteFilePath("../python/detection_server.py");
    
    qDebug() << "Python executable:" << m_pythonExecutable;
    qDebug() << "Python script path:" << m_pythonScriptPath;
}

void DetectionClient::detectObjects(const DetectionRequest& request)
{
    if (m_isProcessing) {
        emit detectionError("Detection already in progress");
        return;
    }
    
    m_currentRequest = request;
    m_isProcessing = true;
    
    // Create JSON request
    QString jsonRequest = createRequestJson(request);
    
    // Start Python process
    QStringList arguments;
    arguments << m_pythonScriptPath << jsonRequest;
    
    m_pythonProcess->start(m_pythonExecutable, arguments);
    m_timeoutTimer->start();
    
    if (!m_pythonProcess->waitForStarted(5000)) {
        m_isProcessing = false;
        m_timeoutTimer->stop();
        emit detectionError("Failed to start Python detection process");
    }
}

QString DetectionClient::createRequestJson(const DetectionRequest& request)
{
    QJsonObject jsonObj;
    jsonObj["image_path"] = request.imagePath;
    jsonObj["confidence_threshold"] = request.confidenceThreshold;
    jsonObj["iou_threshold"] = request.iouThreshold;
    jsonObj["model_name"] = request.modelName;
    jsonObj["save_annotated"] = request.saveAnnotated;
    
    QJsonDocument doc(jsonObj);
    return doc.toJson(QJsonDocument::Compact);
}

DetectionResult DetectionClient::parseResponse(const QByteArray& response)
{
    DetectionResult result;
    result.success = false;
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(response, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        result.errorMessage = "Failed to parse JSON response: " + parseError.errorString();
        return result;
    }
    
    QJsonObject jsonObj = doc.object();
    
    if (jsonObj.contains("error")) {
        result.errorMessage = jsonObj["error"].toString();
        return result;
    }
    
    result.success = jsonObj["success"].toBool();
    result.processingTime = jsonObj["processing_time"].toInt();
    
    QJsonArray detectionsArray = jsonObj["detections"].toArray();
    for (const QJsonValue& value : detectionsArray) {
        QJsonObject detObj = value.toObject();
        
        Detection detection;
        detection.className = detObj["class"].toString();
        detection.confidence = detObj["confidence"].toDouble();
        
        QJsonArray bboxArray = detObj["bbox"].toArray();
        if (bboxArray.size() == 4) {
            detection.bbox = QRect(
                bboxArray[0].toInt(),
                bboxArray[1].toInt(),
                bboxArray[2].toInt(),
                bboxArray[3].toInt()
            );
        }
        
        result.detections.append(detection);
    }
    
    return result;
}

void DetectionClient::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_timeoutTimer->stop();
    m_isProcessing = false;
    
    if (exitStatus == QProcess::CrashExit) {
        emit detectionError("Python process crashed");
        return;
    }
    
    if (exitCode != 0) {
        QString errorOutput = m_pythonProcess->readAllStandardError();
        emit detectionError("Python process failed with exit code " + 
                           QString::number(exitCode) + ": " + errorOutput);
        return;
    }
    
    // Parse successful response
    QByteArray output = m_pythonProcess->readAllStandardOutput();
    DetectionResult result = parseResponse(output);
    
    if (result.success) {
        emit detectionComplete(result);
    } else {
        emit detectionError(result.errorMessage);
    }
}

void DetectionClient::onProcessError(QProcess::ProcessError error)
{
    m_timeoutTimer->stop();
    m_isProcessing = false;
    
    QString errorString;
    switch (error) {
        case QProcess::FailedToStart:
            errorString = "Failed to start Python process. Make sure Python is installed and accessible.";
            break;
        case QProcess::Crashed:
            errorString = "Python process crashed during execution.";
            break;
        case QProcess::Timedout:
            errorString = "Python process timed out.";
            break;
        case QProcess::WriteError:
            errorString = "Failed to write to Python process.";
            break;
        case QProcess::ReadError:
            errorString = "Failed to read from Python process.";
            break;
        default:
            errorString = "Unknown process error occurred.";
    }
    
    emit detectionError(errorString);
}

void DetectionClient::onProcessTimeout()
{
    if (m_pythonProcess->state() != QProcess::NotRunning) {
        m_pythonProcess->kill();
    }
    m_isProcessing = false;
    emit detectionError("Detection process timed out after 30 seconds");
}