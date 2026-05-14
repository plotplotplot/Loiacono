#pragma once

#include <QDialog>

class QComboBox;
class QDoubleSpinBox;
class QSpinBox;

namespace loiacono {

struct SpectrumSettings {
    int multiple = 40;
    int bins = 200;
    int freqMin = 100;
    int freqMax = 3000;
    int sampleRate = 48000;
    double gain = 1.0;
    double gamma = 0.6;
    double floor = 0.05;
    double leakiness = 0.99995;
    int temporalWeightingMode = 0;
    int normalizationMode = 2;
    int windowLengthMode = 2;
    int algorithmMode = 0;
};

class SpectrumSettingsDialog final : public QDialog {
    Q_OBJECT
public:
    explicit SpectrumSettingsDialog(QWidget* parent = nullptr);

    void setSettings(const SpectrumSettings& settings);
    SpectrumSettings settings() const;

signals:
    void settingsChanged(const loiacono::SpectrumSettings& settings);

private:
    QSpinBox* m_multiple = nullptr;
    QSpinBox* m_bins = nullptr;
    QSpinBox* m_freqMin = nullptr;
    QSpinBox* m_freqMax = nullptr;
    QSpinBox* m_sampleRate = nullptr;
    QDoubleSpinBox* m_gain = nullptr;
    QDoubleSpinBox* m_gamma = nullptr;
    QDoubleSpinBox* m_floor = nullptr;
    QDoubleSpinBox* m_leakiness = nullptr;
    QComboBox* m_windowMode = nullptr;
    QComboBox* m_normalizationMode = nullptr;
    QComboBox* m_windowLengthMode = nullptr;
    QComboBox* m_algorithmMode = nullptr;
};

} // namespace loiacono
