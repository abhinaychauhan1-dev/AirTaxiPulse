#include "FeatureOverviewViewModel.h"

FeatureOverviewViewModel::FeatureOverviewViewModel(const QString &title,
                                                   const QString &description,
                                                   const QList<FeatureOverviewCardData*> &cards,
                                                   QObject *parent)
    : QObject(parent)
    , m_title(title)
    , m_description(description)
    , m_cards(cards)
{
    for (FeatureOverviewCardData *card : m_cards) {
        if (card) {
            card->setParent(this);
        }
    }
}

QString FeatureOverviewViewModel::title() const
{
    return m_title;
}

QString FeatureOverviewViewModel::description() const
{
    return m_description;
}

QVariantList FeatureOverviewViewModel::cards() const
{
    QVariantList values;
    values.reserve(m_cards.size());
    for (FeatureOverviewCardData *card : m_cards) {
        values.append(QVariant::fromValue(static_cast<QObject *>(card)));
    }
    return values;
}