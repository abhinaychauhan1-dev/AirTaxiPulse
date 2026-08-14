/**
 * @file    : src/viewmodel/FeatureOverviewViewModel.cpp
 * @brief   : Implements feature overview content and card ownership.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#include "FeatureOverviewViewModel.h"

/// @brief Constructs a feature overview and transfers card ownership to the view model.
/// @param title Feature page title.
/// @param description Feature page description.
/// @param cards Card objects to expose to QML.
/// @param parent Optional QObject parent for Qt ownership.
FeatureOverviewViewModel::FeatureOverviewViewModel(const QString &title,
                                                   const QString &description,
                                                   const QList<FeatureOverviewCardData*> &cards,
                                                   QObject *parent)
    : QObject(parent)
    , m_title(title)
    , m_description(description)
    , m_cards(cards)
{
    // Parent each valid card to keep its lifetime aligned with the overview.
    for (FeatureOverviewCardData *card : m_cards) {
        if (card) {
            card->setParent(this);
        }
    }
}

/// @brief Returns the feature page title.
/// @return Title text displayed by the UI.
QString FeatureOverviewViewModel::title() const
{
    return m_title;
}

/// @brief Returns the feature page description.
/// @return Description text displayed by the UI.
QString FeatureOverviewViewModel::description() const
{
    return m_description;
}

/// @brief Converts the owned card pointers to a QML-compatible variant list.
/// @return Card objects wrapped as QVariant values.
QVariantList FeatureOverviewViewModel::cards() const
{
    QVariantList values;
    values.reserve(m_cards.size());
    for (FeatureOverviewCardData *card : m_cards) {
        values.append(QVariant::fromValue(static_cast<QObject *>(card)));
    }
    return values;
}