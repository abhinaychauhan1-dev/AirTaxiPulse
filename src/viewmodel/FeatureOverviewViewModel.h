#ifndef FEATUREOVERVIEWVIEWMODEL_H
#define FEATUREOVERVIEWVIEWMODEL_H

#include <QObject>
#include <QVariantList>

#include "../data/FeatureOverviewCardData.h"

class FeatureOverviewViewModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QVariantList cards READ cards CONSTANT)

public:
    FeatureOverviewViewModel(const QString &title,
                             const QString &description,
                             const QList<FeatureOverviewCardData*> &cards,
                             QObject *parent = nullptr);

    QString title() const;
    QString description() const;
    QVariantList cards() const;

private:
    QString m_title;
    QString m_description;
    QList<FeatureOverviewCardData*> m_cards;
};

#endif // FEATUREOVERVIEWVIEWMODEL_H