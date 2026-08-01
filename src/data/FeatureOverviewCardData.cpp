#include "FeatureOverviewCardData.h"

FeatureOverviewCardData::FeatureOverviewCardData(const QString &title,
                                                 const QString &body,
                                                 QObject *parent)
    : QObject(parent)
    , m_title(title)
    , m_body(body)
{
}

QString FeatureOverviewCardData::title() const
{
    return m_title;
}

QString FeatureOverviewCardData::body() const
{
    return m_body;
}