#include <QString>

#include "SpectrumWidget.h"

SpectrumWidget::SpectrumWidget(const QString& title, QWidget *parent)
    : QWidget(parent)
{
    smallestEigLabel = new QLabel("--", this);
    biggestEigLabel = new QLabel("--", this);
    spectrum = new BarWidget();

    QVBoxLayout *my_layout = new QVBoxLayout();

    QGroupBox *segb = new QGroupBox(title, this);
    QVBoxLayout *segb_layout = new QVBoxLayout;
    segb_layout->addWidget(smallestEigLabel);
    segb_layout->addWidget(biggestEigLabel);
    segb_layout->addWidget(spectrum);
    segb->setLayout(segb_layout);
    my_layout->addWidget(segb);
    setLayout(my_layout);
}

void
SpectrumWidget::setSpectrum(const Eigen::VectorXd& data)
{
    QString text = QString("Smallest: %1")
        .arg(data[1], 0, 'e', 5);
    smallestEigLabel->setText(text);
    text = QString("Biggest: %1")
        .arg(data[data.size()-1], 0, 'e', 5);
    biggestEigLabel->setText(text);

    spectrum->setData(data);
}