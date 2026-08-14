/**
 * @file    : src/data/FeatureOverviewCardData.cpp
 * @brief   : Implements the feature overview card data object.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#include "FeatureOverviewCardData.h"

/// @brief Initializes a feature overview card with immutable display content.
/// @param title Title text displayed on the card.
/// @param body Descriptive body text displayed on the card.
/// @param parent Optional QObject parent for Qt ownership.
FeatureOverviewCardData::FeatureOverviewCardData(const QString &title,
                                                 const QString &body,
                                                 QObject *parent)
    : QObject(parent)
    , m_title(title)
    , m_body(body)
{
}

/// @brief Returns the card title.
QString FeatureOverviewCardData::title() const
{
    return m_title;
}

/// @brief Returns the descriptive card body.
QString FeatureOverviewCardData::body() const
{
    return m_body;
}