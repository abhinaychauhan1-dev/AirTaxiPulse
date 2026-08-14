/**
 * @file    : src/viewmodel/FeatureOverviewViewModel.h
 * @brief   : Declares the view model for feature overview content.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#ifndef FEATUREOVERVIEWVIEWMODEL_H
#define FEATUREOVERVIEWVIEWMODEL_H

#include <QObject>
#include <QVariantList>

#include "../data/FeatureOverviewCardData.h"

/// @brief Exposes immutable feature overview content and cards to QML.
class FeatureOverviewViewModel : public QObject
{
    Q_OBJECT

    /// @brief Feature page title displayed by the UI.
    Q_PROPERTY(QString title READ title CONSTANT)

    /// @brief Descriptive text explaining the feature area.
    Q_PROPERTY(QString description READ description CONSTANT)

    /// @brief Overview cards exposed as QObject-backed variant values.
    Q_PROPERTY(QVariantList cards READ cards CONSTANT)

public:
    /// @brief Constructs an immutable feature overview and adopts its cards.
    /// @param title Feature page title.
    /// @param description Feature page description.
    /// @param cards Card objects to expose and own.
    /// @param parent Optional QObject parent for Qt ownership.
    FeatureOverviewViewModel(const QString &title,
                             const QString &description,
                             const QList<FeatureOverviewCardData*> &cards,
                             QObject *parent = nullptr);

    /// @brief Returns the feature page title.
    /// @return Title text displayed by the UI.
    QString title() const;

    /// @brief Returns the feature page description.
    /// @return Description text displayed by the UI.
    QString description() const;

    /// @brief Returns the overview cards in a QML-compatible list.
    /// @return Card objects wrapped as QVariant values.
    QVariantList cards() const;

private:
    QString m_title;
    QString m_description;
    QList<FeatureOverviewCardData*> m_cards;
};

#endif // FEATUREOVERVIEWVIEWMODEL_H