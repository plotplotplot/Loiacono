#include "spectrum_settings_dialog.h"

#include "loiacono_rolling.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QVBoxLayout>

namespace loiacono {

SpectrumSettingsDialog::SpectrumSettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Loiacono Spectrum Settings"));
    setModal(false);
    resize(520, 420);

    auto* root = new QVBoxLayout(this);

    auto* transform = new QGroupBox(QStringLiteral("Transform"), this);
    auto* form = new QFormLayout(transform);

    m_algorithmMode = new QComboBox(transform);
    m_algorithmMode->addItem(QStringLiteral("Loiacono"), static_cast<int>(LoiaconoRolling::AlgorithmMode::Loiacono));
    m_algorithmMode->addItem(QStringLiteral("FFT"), static_cast<int>(LoiaconoRolling::AlgorithmMode::FFT));
    m_algorithmMode->addItem(QStringLiteral("Goertzel"), static_cast<int>(LoiaconoRolling::AlgorithmMode::Goertzel));
    form->addRow(QStringLiteral("Algorithm"), m_algorithmMode);

    m_multiple = new QSpinBox(transform);
    m_multiple->setRange(2, 240);
    form->addRow(QStringLiteral("Win scale"), m_multiple);

    m_bins = new QSpinBox(transform);
    m_bins->setRange(32, 2400);
    form->addRow(QStringLiteral("Bins"), m_bins);

    m_freqMin = new QSpinBox(transform);
    m_freqMin->setRange(20, 2000);
    m_freqMin->setSuffix(QStringLiteral(" Hz"));
    form->addRow(QStringLiteral("Freq min"), m_freqMin);

    m_freqMax = new QSpinBox(transform);
    m_freqMax->setRange(500, 12000);
    m_freqMax->setSuffix(QStringLiteral(" Hz"));
    form->addRow(QStringLiteral("Freq max"), m_freqMax);

    m_sampleRate = new QSpinBox(transform);
    m_sampleRate->setRange(8000, 192000);
    m_sampleRate->setSuffix(QStringLiteral(" Hz"));
    form->addRow(QStringLiteral("Sample rate"), m_sampleRate);

    m_windowMode = new QComboBox(transform);
    m_windowMode->addItem(QStringLiteral("Rectangular"), static_cast<int>(LoiaconoRolling::WindowMode::RectangularWindow));
    m_windowMode->addItem(QStringLiteral("Hann"), static_cast<int>(LoiaconoRolling::WindowMode::HannWindow));
    m_windowMode->addItem(QStringLiteral("Hamming"), static_cast<int>(LoiaconoRolling::WindowMode::HammingWindow));
    m_windowMode->addItem(QStringLiteral("Blackman"), static_cast<int>(LoiaconoRolling::WindowMode::BlackmanWindow));
    m_windowMode->addItem(QStringLiteral("Blackman-Harris"), static_cast<int>(LoiaconoRolling::WindowMode::BlackmanHarrisWindow));
    m_windowMode->addItem(QStringLiteral("Leaky"), static_cast<int>(LoiaconoRolling::WindowMode::LeakyWindow));
    form->addRow(QStringLiteral("Windowing"), m_windowMode);

    m_normalizationMode = new QComboBox(transform);
    m_normalizationMode->addItem(QStringLiteral("Raw sum"), static_cast<int>(LoiaconoRolling::NormalizationMode::RawSum));
    m_normalizationMode->addItem(QStringLiteral("Coherent amplitude"), static_cast<int>(LoiaconoRolling::NormalizationMode::CoherentAmplitude));
    m_normalizationMode->addItem(QStringLiteral("Energy"), static_cast<int>(LoiaconoRolling::NormalizationMode::Energy));
    form->addRow(QStringLiteral("Normalization"), m_normalizationMode);

    m_windowLengthMode = new QComboBox(transform);
    m_windowLengthMode->addItem(QStringLiteral("Constant samples"), static_cast<int>(LoiaconoRolling::WindowLengthMode::ConstantSamples));
    m_windowLengthMode->addItem(QStringLiteral("Sqrt period"), static_cast<int>(LoiaconoRolling::WindowLengthMode::SqrtPeriod));
    m_windowLengthMode->addItem(QStringLiteral("Period multiple"), static_cast<int>(LoiaconoRolling::WindowLengthMode::PeriodMultiple));
    form->addRow(QStringLiteral("Window len"), m_windowLengthMode);
    root->addWidget(transform);

    auto* display = new QGroupBox(QStringLiteral("Display"), this);
    auto* displayForm = new QFormLayout(display);

    m_gain = new QDoubleSpinBox(display);
    m_gain->setRange(0.1, 20.0);
    m_gain->setSingleStep(0.1);
    m_gain->setDecimals(1);
    m_gain->setSuffix(QStringLiteral("x"));
    displayForm->addRow(QStringLiteral("Gain"), m_gain);

    m_gamma = new QDoubleSpinBox(display);
    m_gamma->setRange(0.1, 2.0);
    m_gamma->setSingleStep(0.05);
    m_gamma->setDecimals(2);
    displayForm->addRow(QStringLiteral("Gamma"), m_gamma);

    m_floor = new QDoubleSpinBox(display);
    m_floor->setRange(0.0, 0.5);
    m_floor->setSingleStep(0.01);
    m_floor->setDecimals(2);
    displayForm->addRow(QStringLiteral("Floor"), m_floor);

    m_leakiness = new QDoubleSpinBox(display);
    m_leakiness->setRange(0.99, 1.0);
    m_leakiness->setSingleStep(0.00001);
    m_leakiness->setDecimals(5);
    displayForm->addRow(QStringLiteral("Leakiness"), m_leakiness);
    root->addWidget(display);

    auto emitChanged = [this]() { emit settingsChanged(settings()); };
    for (QSpinBox* box : {m_multiple, m_bins, m_freqMin, m_freqMax, m_sampleRate}) {
        connect(box, qOverload<int>(&QSpinBox::valueChanged), this, [emitChanged](int) { emitChanged(); });
    }
    for (QDoubleSpinBox* box : {m_gain, m_gamma, m_floor, m_leakiness}) {
        connect(box, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [emitChanged](double) { emitChanged(); });
    }
    for (QComboBox* box : {m_windowMode, m_normalizationMode, m_windowLengthMode, m_algorithmMode}) {
        connect(box, qOverload<int>(&QComboBox::currentIndexChanged), this, [emitChanged](int) { emitChanged(); });
    }

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::hide);
    root->addWidget(buttons);

    setSettings(SpectrumSettings{});
}

void SpectrumSettingsDialog::setSettings(const SpectrumSettings& s)
{
    const QSignalBlocker b0(m_multiple);
    const QSignalBlocker b1(m_bins);
    const QSignalBlocker b2(m_freqMin);
    const QSignalBlocker b3(m_freqMax);
    const QSignalBlocker b4(m_sampleRate);
    const QSignalBlocker b5(m_gain);
    const QSignalBlocker b6(m_gamma);
    const QSignalBlocker b7(m_floor);
    const QSignalBlocker b8(m_leakiness);
    const QSignalBlocker b9(m_windowMode);
    const QSignalBlocker b10(m_normalizationMode);
    const QSignalBlocker b11(m_windowLengthMode);
    const QSignalBlocker b12(m_algorithmMode);
    m_multiple->setValue(s.multiple);
    m_bins->setValue(s.bins);
    m_freqMin->setValue(s.freqMin);
    m_freqMax->setValue(s.freqMax);
    m_sampleRate->setValue(s.sampleRate);
    m_gain->setValue(s.gain);
    m_gamma->setValue(s.gamma);
    m_floor->setValue(s.floor);
    m_leakiness->setValue(s.leakiness);
    m_windowMode->setCurrentIndex(qMax(0, m_windowMode->findData(s.temporalWeightingMode)));
    m_normalizationMode->setCurrentIndex(qMax(0, m_normalizationMode->findData(s.normalizationMode)));
    m_windowLengthMode->setCurrentIndex(qMax(0, m_windowLengthMode->findData(s.windowLengthMode)));
    m_algorithmMode->setCurrentIndex(qMax(0, m_algorithmMode->findData(s.algorithmMode)));
}

SpectrumSettings SpectrumSettingsDialog::settings() const
{
    SpectrumSettings s;
    s.multiple = m_multiple->value();
    s.bins = m_bins->value();
    s.freqMin = m_freqMin->value();
    s.freqMax = qMax(m_freqMin->value() + 50, m_freqMax->value());
    s.sampleRate = m_sampleRate->value();
    s.gain = m_gain->value();
    s.gamma = m_gamma->value();
    s.floor = m_floor->value();
    s.leakiness = m_leakiness->value();
    s.temporalWeightingMode = m_windowMode->currentData().toInt();
    s.normalizationMode = m_normalizationMode->currentData().toInt();
    s.windowLengthMode = m_windowLengthMode->currentData().toInt();
    s.algorithmMode = m_algorithmMode->currentData().toInt();
    return s;
}

} // namespace loiacono
