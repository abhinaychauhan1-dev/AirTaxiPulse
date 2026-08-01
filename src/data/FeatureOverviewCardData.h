#ifndef FEATUREOVERVIEWCARDDATA_H
#define FEATUREOVERVIEWCARDDATA_H

#include <QObject>

class FeatureOverviewCardData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString body READ body CONSTANT)

public:
    FeatureOverviewCardData(const QString &title,
                            const QString &body,
                            QObject *parent = nullptr);

    QString title() const;
    QString body() const;

private:
    QString m_title;
    QString m_body;
};

#endif // FEATUREOVERVIEWCARDDATA_H